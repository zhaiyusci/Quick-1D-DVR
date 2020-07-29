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
#include<algorithm>
#include<cctype>
#include<string>


using namespace std;
using namespace yDVR;

Scalar mass = 0.;
Scalar a = 0.;
Scalar b = 0.;
Scalar threshold = 0.;
Scalar lengthunit = angstrom;
string lengthunit_str("Ang");
Scalar energyunit = cm_1;
string energyunit_str("cm-1");
Scalar massunit = dalton;
string massunit_str("amu");
bool fixboundary = false;
int n_levels = 5;

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
  cout << "!!!Try different number of grids with boundary (" << a/lengthunit << "," << b/lengthunit << ") (in "<< lengthunit_str <<")..." << endl;
  cout << "" << endl;
  cout << "________________________________________________________________________" << endl;
  printf("%10s %18s %18s %18s\n", "#Grids", "E", "b avg. dE", "boundary chk");
  printf("%10s %18s %18s %18s\n", "", energyunit_str.c_str(), energyunit_str.c_str(), energyunit_str.c_str());
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
        printf("%10d %18.4f %18.8f %18.8f\n", nsinc, E/energyunit, deltaE/energyunit, bc/energyunit);
      }else{
        printf("%10d %18.4f %18.8f %18s\n", nsinc, E/energyunit, deltaE/energyunit, "---");
      }
    } else {
      printf("%10d %18.4f %18s %18s\n", nsinc, E/energyunit, "---", "---");
    }
    if(fabs(deltaE) < threshold && (fabs(boundarycheck(sinc,a, b, n_levels)) < threshold || nobc)){
      cout << "________________________________________________________________________" << endl;
      cout << "!!!SincDVR converged with " << nsinc << " points (threshold = " << threshold/energyunit << " " << energyunit_str << ")!!!" << endl;
      return sinc;
    }
    oldEs.erase(oldEs.begin());
    oldEs.push_back(E);
    delete sinc;
  }
  warning("SincDVR fail to converge within " + string(to_string(itend))+ " grids.");
  return nullptr;
}

int main(int argc, char* argv[]){

  banner();

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
  section(filename);

  // 2. Read the input file and print them out...
  string line;
  string word;
  ifstream input(filename+".inp");

  class vpoint{
    public:
      Scalar q_, v_;
      vpoint(double q, double v): q_(q), v_(v) {}
      bool operator< (vpoint rhs) const {return this->q_ < rhs.q_;}
      bool operator== (vpoint rhs) const {return fabs(this->q_ - rhs.q_) < 1e-8;}
  };
  vector<vpoint> potvec;

  while(getlinecmt(input, line)) {
    std::transform(line.begin(), line.end(), line.begin(), [](unsigned char c){ return std::tolower(c); });
    std::stringstream ss(line);
    while(ss >> word){
      if(word == string("mass")){
        ss >> mass;
      }else if(word == string("threshold")){
        ss >> threshold;
      }else if(word == string("nlevels")){
        ss >> n_levels;
      }else if(word == string("angstrom")){
        lengthunit = angstrom;
        lengthunit_str = "Ang";
      }else if(word == string("bohr")){
        lengthunit = 1.;
        lengthunit_str = "bohr";
      }else if(word == string("hartree")){
        energyunit = 1.;
        energyunit_str = "Eh";
      }else if(word == string("cm-1")){
        energyunit = cm_1;
        energyunit_str = "cm-1";
      }else if(word == string("cm_1")){
        energyunit = cm_1;
        energyunit_str = "cm-1";
      }else if(word == string("kelvin")){
        energyunit = kelvin;
        energyunit_str = "Kelvin";
      }else if(word == string("ev")){
        energyunit = eV;
        energyunit_str = "eV";
      }else if(word == string("fixboundary")){
        fixboundary = true;
        ss >> a >> b;
      }else if(word == string("pec")){
        while(getlinecmt(input,line)){
          size_t tmp = line.rfind("endpec");
          if (tmp == string::npos){
            Scalar q, v;
            std::stringstream ss(line);
            if(ss >> q >> v) potvec.push_back(vpoint(q,v));
          }else{
            break;
          }
        }
        sort(potvec.begin(), potvec.end());
        auto tmp = unique(potvec.begin(), potvec.end());
        if (tmp != potvec.end()){
          potvec.erase(tmp, potvec.end());
          warning("Duplicate potential points found. Check your input!!!");
        }
      }else{
        fatal(string("Unknown keyword \"") + word + string("\"."));
      }
    }
  }

  mass *= dalton;
  a *= lengthunit;
  b *= lengthunit;
  threshold *= energyunit;
  if(threshold == 0.){
    threshold = 0.1 * cm_1;
  }


  vector<Scalar> qs, vs;
  for (auto && i : potvec){
    qs.push_back(i.q_*lengthunit);
    vs.push_back(i.v_*energyunit);
    // cout << "pot " << i.q_ << "   "<< i.v_ << endl;
  }

  // 3. Do some calculation...
  Log::set(filename+".ydvr.log");
  CubicSpline1d pot(qs.size(), qs, vs, 1e99, 1e99); // The large numbers are for auto calculating tangent

  Oscillator osc(mass, pot);

  // 3.1. First try largest range 
  section("Step 1. Try the largest possible range defined by potential given.");

  a=qs.front();
  b=qs.back();
  // The magic number 10 are from my personal exprience
  // The thing is Sinc-DVR is not the smart one, more grids are needed
  int ngrids = n_levels+10; 
  Scalar oldE = 1.e99;
  SincDVR* sinc = iteration(osc, a, b, ngrids, 300, n_levels, threshold, oldE);
  if(sinc == nullptr){
    fatal(vector<string>({
          "The sinc-DVR calculation at largest possible range did not converge.",
          "This can be resulted from ",
          "  (a) Even 300 grids are not enough for this system, or",
          "  (b) The calculation did not pass the boundary check.", 
          "I suggest to enlarge the range of potential scanning."
          }));
  }
  writetofile(*sinc, filename+"_0", n_levels);
  ngrids = sinc->grids().size();
  oldE = sinc->energyLevel(n_levels);

  section("Step 2. Further check boundary.");
  subsection("Step 2.1. Left boundary.");
  Scalar contriba=0;
  int ai = 0;
  for (int i = 0; i < ngrids; ++i){
    contriba += fabs(sinc->hamiltonianMatrix()(i,i)*pow(sinc->energyState(n_levels)(i),2)); 
    if (contriba> 0.1*threshold){ 
      if (i <= 1){
        fatal(vector<string>({
              "Left boundary check did not pass. ",
              "Lengthen the PEC to give a long tail for the wave function."
              }));
      }else{
        cout << "PASS!" << endl;
      }
      ai=i;
      break;
    }
  }
  subsection("Step 2.2. Right boundary.");
  Scalar contribb=0;
  int bi = 0;
  for (int i = ngrids -1 ; i >= 0; --i){
    contribb += fabs(sinc->hamiltonianMatrix()(i,i)*pow(sinc->energyState(n_levels)(i),2)); 
    if (contribb > 0.1*threshold){
      if (i >= ngrids-2){
        fatal(vector<string>({
              "Right boundary check did not pass. ",
              "Lengthen the PEC to give a long tail for the wave function."
              }));
      }else{
        cout << "PASS!" << endl;
      }
      bi=i;
      break;
    }
  }
  a=sinc->grids()(ai-1);
  b=sinc->grids()(bi+1);
  cout << "The following step(s) will be based on new boundary without check." << endl;


  section("Step 3. Recalculate the energy levels on a shortened range.");
  ngrids =bi-ai+1;;
  delete sinc;
  sinc = iteration(osc, a, b, ngrids, ngrids+3, n_levels, threshold, oldE, true);
  if(sinc == nullptr){
    warning(vector<string>({
          "This iteration should converge within 1 step or 2, but it did not.",
          "If anything else is good, just use result from Step 1 (Files ",
          filename+"_0.*.txt ) ."
          }));
  }

  writetofile(*sinc, filename, n_levels);

  cout << "Quick 1D DVR ends normally." << endl << endl;
  return 0;
}

