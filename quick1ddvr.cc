// This file is part of Quick 1D DVR.
//
// Copyright (C) 2020 Yu Zhai <yuzhai@mail.huiligroup.org>
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include<ydvr.h>
#include<utility>
#include<cstdio>
#include"iotools.h"
#include"spline.h"
#include"physical_constants.h"


using namespace std;
using namespace yDVR;


int main(int argc, char* argv[]){

  cout << "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=" << endl;
  cout << "*                                                                      =" << endl;
  cout << "*                        Quick 1D DVR v 0.1.0                          =" << endl;
  cout << "*                                                                      =" << endl;
  cout << "*              An interface of yDVR for non-programmer                 =" << endl;
  cout << "*                                                                      =" << endl;
  cout << "*                Yu Zhai <yuzhai@mail.huiligroup.org>                  =" << endl;
  cout << "*                 Jilin University, Changchun, China                   =" << endl;
  cout << "*                                                                      =" << endl;
  cout << "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=" << endl;

  // 1. Parse the file name...
  if(argc != 2){
    printUsage();
  } 
  string filename(argv[1]);
  size_t tmp = filename.rfind(".inp");
  if (tmp == string::npos){
    printUsage();
  }
  filename = filename.substr(0,tmp);
  cout << filename <<endl;

  // 2. Read the input file and print them out...
  string line;
  ifstream input(filename+".inp");
  getlinecmt(input, line);
  Scalar mass = 0.;
  int n_levels = 0;
  {
    std::stringstream ss(line);
    ss >> mass >> n_levels;
  }

  mass *= dalton;

  getlinecmt(input, line); // blank line

  class vpoint{
    public:
      Scalar q_, v_;
      vpoint(double q, double v): q_(q), v_(v) {}
      bool operator< (vpoint rhs) const {return this->q_ < rhs.q_;}
      bool operator== (vpoint rhs) const {return fabs(this->q_ - rhs.q_) < 1e-8;}
  };
  vector<vpoint> potvec;
  while(getlinecmt(input,line)){
    Scalar q, v;
    std::stringstream ss(line);
    if(ss >> q >> v) potvec.push_back(vpoint(q,v));
  }

  {
    sort(potvec.begin(), potvec.end());
    auto tmp = unique(potvec.begin(), potvec.end());
    if (tmp != potvec.end()){
      potvec.erase(tmp, potvec.end());
      cout << "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=" << endl;
      cout << "!!!WARNING!!! Duplicate potential points found. Check your input!!!" << endl;
      cout << "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=" << endl;
    }
  }

  vector<Scalar> qs, vs;
  for (auto && i : potvec){
    qs.push_back(i.q_*angstrom);
    vs.push_back(i.v_*cm_1);
    // cout << "pot " << i.q_ << "   "<< i.v_ << endl;
  }


  // 3. Do some calculation...
  Log::set(filename+".ydvr.log");
  CubicSpline1d pot(qs.size(), qs, vs, 1e99, 1e99); // The large numbers are for auto calculating tangent

  Oscillator osc(mass, pot);

  // The sinc DVR adn PODVR representation
  Scalar oldE = 1.e99;
  int nsinc = n_levels;

  Vector energies;
  Vector grids;
  Matrix eigenvectors;
  cout << "!!!Try different number of grids..." << endl;
  cout << "________________________________________________________________________" << endl;
  printf("%10s %20s %20s\n", "#Sinc-DVR", "E/cm-1", "delta E/cm-1");
  cout << "________________________________________________________________________" << endl;
  for (nsinc = n_levels+10; nsinc <=1000; nsinc+=50){
    SincDVR sinc(osc, qs.front(), qs.back(), nsinc);
    Scalar E = sinc.energyLevel(n_levels);
    Scalar deltaE = E-oldE;
    if (fabs(deltaE) < 10000*cm_1){
      printf("%10d %20.4f %20.4f\n", nsinc, E/cm_1, deltaE/cm_1);
    } else {
      printf("%10d %20.4f %20s\n", nsinc, E/cm_1, "N/A");
    }
    if(fabs(deltaE) < 0.1*cm_1){ 
      cout << "________________________________________________________________________" << endl;
      cout << "!!!SincDVR converged with " << nsinc << " points!!!" << endl;
      energies = sinc.energyLevels();
      eigenvectors = sinc.energyStates();
      grids = sinc.grids();
      break;
    }
    oldE = E;
  }

  printf("%3s  %20s\n", "v", "energies(v)/cm_1");
  for(int i =0; i <= n_levels; ++ i){
    printf("%3d  %20.4f\n", i, energies(i)/cm_1);
  }

  {
    FILE* file = fopen((filename+".energies.txt").c_str(), "w");
    fprintf(file, "%3s  %20s\n", "v", "Energies(i)/cm_1");
    for(int i =0; i <= n_levels; ++ i){
      fprintf(file, "%3d  %20.8f\n", i, energies(i)/cm_1);
    }
    cout << "Energies are written to " << filename << ".energies.txt" << endl;
    fclose(file);
  }

  {
    FILE* file = fopen((filename+".eigenvectors.txt").c_str(), "w");
    fprintf(file, "%20s  %20s\n", "Grids/Ang", "Eigenvectors");
    for(int j =0; j < nsinc; ++j){
      fprintf(file, "%20.8f ", grids(j)/angstrom);
      for(int i =0; i <= n_levels; ++ i){
        fprintf(file, "%20.8f ", eigenvectors(j,i));
      }
      fprintf(file, "\n");
    }
    cout << "Eigenvectors are written to " << filename << ".eigenvectors.txt" << endl;
    fclose(file);
  }

  {
    vector<Scalar> maxwf;
    for(int i =0; i<= n_levels; ++i){
      Scalar min = eigenvectors.col(i).minCoeff();
      Scalar max = eigenvectors.col(i).maxCoeff();
      Scalar maxabs = fabs(min)>fabs(max) ? fabs(min) : fabs(max);
      maxwf.push_back(maxabs);
    }
    FILE* file = fopen((filename+".plot.txt").c_str(), "w");
    fprintf(file, "%20s  %20s %20s\n", "Grids/Ang", "Potential/cm-1", "Eigenvectors shifted");
    for(int j =0; j < nsinc; ++j){
      fprintf(file, "%20.8f %20.8f ", grids(j)/angstrom, osc.potential(grids(j))/cm_1);
      for(int i =0; i <= n_levels; ++ i){
        fprintf(file, "%20.8f ", energies(i)/cm_1 + eigenvectors(j,i)/maxwf[i]*(energies(i+1)-energies(i))*0.3/cm_1);
      }
      fprintf(file, "\n");
    }
    cout << "Plot are written to " << filename << ".plot.txt" << endl;
    fclose(file);
  }

  if (energies.size() == 0){
    cout << "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=" << endl;
    cout << "!!!ERROR!!! SincDVR fail to converge within 1000 grids." << endl;
    cout << "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=" << endl;
    return -1;
  }

  cout << "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=" << endl;
  cout << "*                          SANITY CHECKS                               =" << endl;
  cout << "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=" << endl;
  cout << endl;
  cout << "1. Is the integral range large enough?" << endl;
  {
    Scalar range = qs.back() - qs.front();
    SincDVR sinc(osc, qs.front()+range*0.01, qs.back()-range*0.01, nsinc);
    Scalar E = sinc.energyLevel(n_levels);
    if(fabs(energies(n_levels) - E) < 0.1*cm_1){
      cout << "    PASS!!!" << endl;
    }else{
      cout << "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=" << endl;
      cout << "    !!!WARNING!!!" << endl
        << "        A slightly smaller range integral gives different energy at " << endl 
        << "        highest vibrational state wanted." << endl
        << "        Full-range integral gives " << energies(n_levels)/cm_1 << " cm-1, "<<endl 
        << "        while a 98\% one gives " << E/cm_1 <<" cm-1."<< endl 
        << "        I suggest to enlarge the range of potential scanning." << endl;
      cout << "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=" << endl;
    }
  }
  cout << "2. Does the frequencies look normal?" << endl;
  {
    if((energies[1] - energies[0])/cm_1 < 4000.){
      cout << "    PASS!!!" << endl;
    }else{
      cout << "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=" << endl;
      cout << "    !!!WARNING!!!" << endl
        << "        Normally for a molecular system, fundamental frequency should be" << endl
        << "        less than 4000 cm-1.  If you are trying calculate some system " << endl 
        << "        which is not a regular molecule, ignore this warning. However, " << endl
        << "        if you are working on a regular molecule, no matter it is" << endl
        << "        a diatomic molecule or a normal mode, double check (a) the " << endl 
        << "        units you use (b) the quantum chemistry calculation." << endl;
      cout << "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=" << endl;
    }
  }
  cout << "3. Does the anharmonic behaviour look normal?" << endl;
  {
    if(energies[2] - energies[1] < energies[1] - energies[0] ){
      cout << "    PASS!!!" << endl;
    }else{
      cout << "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=" << endl;
      cout << "    !!!WARNING!!!" << endl
        << "        Normally for a diatomic molecule, fundamental frequency should be" << endl
        << "        larger than hot bands. If you are working on a normal mode, ignore" << endl
        << "        this warning. " << endl;
      cout << "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=" << endl;
    }
  }
  cout << endl
    << "Disclaimer: These checks may or may NOT be reliable. They are NOT peer-" << endl
    << "    reviewed methods. However, if WARNING(s) are reported," << endl
    << "    the user should NOT use the results without further thinking." << endl
    << endl;

  cout << "Quick 1D DVR ends normally." << endl << endl;
  return 0;
}

