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

Scalar boundarycheck(SincDVR* sinc, Scalar a, Scalar b, int n_levels){
  int nsinc = sinc->grids().size();
  // Scalar a = sinc->grids()(0);
  // Scalar b = sinc->grids()(nsinc-1);
  // SincDVR check(sinc->oscillator(), a, b, nsinc-2);
  Scalar range = b-a;
  SincDVR check(sinc->oscillator(), a+0.025*range, b-0.025*range, nsinc);
  Scalar E = check.energyLevel(n_levels);
  return sinc->energyLevel(n_levels) - E;
}

SincDVR* iteration(Oscillator& osc, Scalar a, Scalar b, int itbegin, int itend, int n_levels, Scalar threshold, Scalar oldE, bool nobc = false){
  int nsinc = n_levels;
  vector<Scalar> oldEs(5, oldE);

  SincDVR* sinc = nullptr;
  cout << "!!!Try different number of grids with boundary (" << a/angstrom << "," << b/angstrom << ")..." << endl;
  cout << "________________________________________________________________________" << endl;
  printf("%10s %18s %18s %18s\n", "#Grids", "E/cm-1", "b avg. dE/cm-1", "boundary chk/cm-1");
  cout << "________________________________________________________________________" << endl;
  for (nsinc = itbegin; nsinc <= itend; ++nsinc){
    sinc = new SincDVR(osc, a, b, nsinc);
    Scalar E = sinc->energyLevel(n_levels);
    Scalar oldE=0;
    for(auto && i : oldEs){
      oldE += i;
    }
    oldE /= oldEs.size();
    Scalar deltaE = E-oldE;
    if (fabs(deltaE) < 10000*cm_1){
      if (fabs(deltaE) < threshold && (!nobc)){
        Scalar bc=boundarycheck(sinc,a, b, n_levels);
        printf("%10d %18.4f %18.8f %18.8f\n", nsinc, E/cm_1, deltaE/cm_1, bc/cm_1);
      }else{
        printf("%10d %18.4f %18.8f %18s\n", nsinc, E/cm_1, deltaE/cm_1, "---");
      }
    } else {
      printf("%10d %18.4f %18s %18s\n", nsinc, E/cm_1, "---", "---");
    }
    if(fabs(deltaE) < threshold && (fabs(boundarycheck(sinc,a, b, n_levels)) < threshold || nobc)){
      cout << "________________________________________________________________________" << endl;
      cout << "!!!SincDVR converged with " << nsinc << " points (threshold = " << threshold/cm_1 << " cm-1)!!!" << endl;
      return sinc;
    }
    oldEs.erase(oldEs.begin());
    oldEs.push_back(E);
    delete sinc;
  }
  cout << "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=" << endl;
  cout << "!!!WARNING!!! SincDVR fail to converge within " << itend << " grids." << endl;
  cout << "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=" << endl;
  return nullptr;
}

int main(int argc, char* argv[]){

  cout << "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=" << endl;
  cout << "*                                                                      =" << endl;
  cout << "*                        Quick 1D DVR v 0.2.0                          =" << endl;
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

  // 3.1. First try largest range 
  cout << endl;
  cout << "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=" << endl;
  cout << "Step 1. Try the largest possible range defined by potential given." << endl;
  cout << "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=" << endl;

  Scalar threshold = 0.1*cm_1;
  Scalar a=qs.front();
  Scalar b=qs.back();
  // The magic number 10 are from my personal exprience
  // The thing is Sinc-DVR is not the smart one, more grids are needed
  int ngrids = n_levels+10; 
  Scalar oldE = 1.e99;
  SincDVR* sinc = iteration(osc, a, b, ngrids, 300, n_levels, threshold, oldE);
  if(sinc == nullptr){
    cout << "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=" << endl;
    cout << "!!!ERROR!!!" << endl
      << "  The sinc-DVR calculation at largest possible range did not converge." << endl 
      << "  This can be resulted from " << endl
      << "    (a) Even 300 grids are not enough for this system, or"<<endl 
      << "    (b) The calculation did not pass the boundary check."<< endl 
      << "  I suggest to enlarge the range of potential scanning." << endl;
    cout << "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=" << endl;
    exit(0);
  }
  writetofile(*sinc, filename+"_0", n_levels);
  ngrids = sinc->grids().size();
  oldE = sinc->energyLevel(n_levels);

  cout << endl;
  cout << "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=" << endl;
  cout << "Step 2. Further check boundary." << endl;
  cout << "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=" << endl;
  cout << "Step 2.1. Left boundary." << endl;
  Scalar contriba=0;
  int ai = 0;
  for (int i = 0; i < ngrids; ++i){
    contriba += fabs(sinc->hamiltonianMatrix()(i,i)*pow(sinc->energyState(n_levels)(i),2)); 
    if (contriba> 0.1*threshold){ 
      if (i <= 1){
        cout << "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=" << endl;
        cout << "!!!ERROR!!!" << endl
          << "  Left boundary check did not pass. " << endl 
          << "  Lengthen the PEC to give a long tail for the wave function." << endl;
        cout << "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=" << endl;
        exit(0);
      }
      ai=i;
      break;
    }
  }
  cout << "Step 2.2. Right boundary." << endl;
  Scalar contribb=0;
  int bi = 0;
  for (int i = ngrids -1 ; i >= 0; --i){
    contribb += fabs(sinc->hamiltonianMatrix()(i,i)*pow(sinc->energyState(n_levels)(i),2)); 
    if (contribb > 0.1*threshold){
      if (i >= ngrids-2){
        cout << "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=" << endl;
        cout << "!!!ERROR!!!" << endl
          << "  Right boundary check did not pass. " << endl 
          << "  Lengthen the PEC to give a long tail for the wave function." << endl;
        cout << "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=" << endl;
        exit(0);
      }
      bi=i;
      break;
    }
  }
  a=sinc->grids()(ai-1);
  b=sinc->grids()(bi+1);
  cout << "The following step(s) will be based on new boundary without check." << endl;


  cout << endl;
  cout << "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=" << endl;
  cout << "Step 3. Recalculate the energy levels on a shortened range." << endl;
  cout << "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=" << endl;
  ngrids =bi-ai+1;;
  delete sinc;
  sinc = iteration(osc, a, b, ngrids, ngrids+3, n_levels, threshold, oldE, true);
  if(sinc == nullptr){
    cout << "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=" << endl;
    cout << "!!!WARNING!!!" << endl
      << "  This iteration should converge within 1 step or 2, but it did not." << endl 
      << "  If anything else is good, just use result from Step 1 (Files " << endl
      << "  " << filename << "_0.*.txt ) ." << endl;
    cout << "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=" << endl;
  }

  writetofile(*sinc, filename, n_levels);

  cout << "Quick 1D DVR ends normally." << endl << endl;
  return 0;
}

