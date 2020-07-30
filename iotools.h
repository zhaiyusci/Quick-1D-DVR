// This file is part of Quick 1D DVR.
//
// Copyright (C) 2020 Yu Zhai <yuzhai@mail.huiligroup.org>
//
// This Source Code Form is subject to the terms of the Mozilla
// Public License v. 2.0. If a copy of the MPL was not distributed
// with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

#ifndef Q_IOTOOLS_H_
#define Q_IOTOOLS_H_
#include<ydvr.h>
#include<iostream>
#include<cstdlib>
#include<utility>
#include<cstdio>
#include"physical_constants.h"

void printUsage(){
  std::cerr << std::endl;
  std::cerr << "Usage: /path/to/quick1ddvr <filename>.inp" << std::endl;
  std::cerr << "  The input file should be a plain text file with format shown in sample_hcl.inp" << std::endl;
  std::cerr << std::endl;
  exit(EXIT_FAILURE);
}

bool getlinecmt(std::istream& s, std::string& line){
  if(getline(s,line)){
    size_t tmp = line.rfind("#");
    if ( tmp != std::string::npos ){
      line = line.substr(0,tmp);
    }
    return true;
  }
  return false;
}

void writetofile(yDVR::DVR& dvr, const std::string& filename, int n_levels){
  yDVR::Vector grids = dvr.grids();
  yDVR::Vector energies = dvr.energyLevels();
  yDVR::Matrix eigenvectors = dvr.energyStates();
  int nsinc = dvr.grids().size();
  {
    std::cout << "Vibraional energy levels" << std::endl;
    std::cout << "________________________________________________________________________" << std::endl;
    printf("%3s  %20s  %20s\n", "v", "Energies(v)/cm_1", "Gap(v)/cm_1");
    std::cout << "________________________________________________________________________" << std::endl;
    for(int i =0; i <= n_levels; ++ i){
      printf("%3d  %20.8f  %20.8f\n", i, energies(i)/cm_1, (energies(i)-energies(0))/cm_1);
    }
    std::cout << "________________________________________________________________________" << std::endl;
  }

  {
    FILE* file = fopen((filename+".energies.txt").c_str(), "w");
    fprintf(file, "%3s  %20s\n", "v", "Energies(v)/cm_1");
    for(int i =0; i <= n_levels; ++ i){
      fprintf(file, "%3d  %20.8f\n", i, energies(i)/cm_1);
    }
    std::cout << "Energies are written to " << filename << ".energies.txt" << std::endl;
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
    std::cout << "Eigenvectors are written to " << filename << ".eigenvectors.txt" << std::endl;
    fclose(file);
  }

  {
    std::vector<yDVR::Scalar> maxwf;
    for(int i =0; i<= n_levels; ++i){
      yDVR::Scalar min = eigenvectors.col(i).minCoeff();
      yDVR::Scalar max = eigenvectors.col(i).maxCoeff();
      yDVR::Scalar maxabs = fabs(min)>fabs(max) ? min : max;
      maxwf.push_back(maxabs);
    }
    FILE* file = fopen((filename+".plot.txt").c_str(), "w");
    fprintf(file, "%20s  %20s %20s\n", "Grids/Ang", "Potential/cm-1", "Eigenvectors shifted");
    for(int j =0; j < nsinc; ++j){
      fprintf(file, "%20.8f %20.8f ", grids(j)/angstrom, dvr.oscillator().potential(grids(j))/cm_1);
      for(int i =0; i <= n_levels; ++ i){
        fprintf(file, "%20.8f ", energies(i)/cm_1 + eigenvectors(j,i)/maxwf[i]*(energies(i+1)-energies(i))*0.3/cm_1);
      }
      fprintf(file, "\n");
    }
    std::cout << "Plot are written to " << filename << ".plot.txt" << std::endl;
    fclose(file);
  }

  return;
  
}

void banner(){
  std::cout << "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=" << std::endl;
  std::cout << "*                                                                      =" << std::endl;
  std::cout << "*                             Quick 1D DVR                             =" << std::endl;
  std::cout << "*                                v0.3.1                                =" << std::endl;
  std::cout << "*                                                                      =" << std::endl;
  std::cout << "*               An interface of yDVR for non-programmer                =" << std::endl;
  std::cout << "*                                                                      =" << std::endl;
  std::cout << "*                 Yu Zhai <yuzhai@mail.huiligroup.org>                 =" << std::endl;
  std::cout << "*                  Jilin University, Changchun, China                  =" << std::endl;
  std::cout << "*                                                                      =" << std::endl;
  std::cout << "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=" << std::endl;
}

void warning(const std::vector<std::string>& info){
  std::cout << std::endl;
  std::cout << "*=WARNING=*=WARNING=*=WARNING=*=WARNING=*=WARNING=*=WARNING=*=WARNING=*=" << std::endl;
  for(auto && i : info){
    std::cout << i << std::endl;
  }
  std::cout << "*=WARNING=*=WARNING=*=WARNING=*=WARNING=*=WARNING=*=WARNING=*=WARNING=*=" << std::endl;
  std::cout << std::endl;
}
void warning(const std::string& info){
  warning(std::vector<std::string>({info}));
}
void fatal(const std::vector<std::string>& info){
  std::cout << std::endl;
  std::cout << "*=FATAL=*=*=FATAL=*=*=FATAL=*=*=FATAL=*=*=FATAL=*=*=FATAL=*=*=FATAL=*=*=" << std::endl;
  for(auto && i : info){
    std::cout << i << std::endl;
  }
  std::cout << "*=FATAL=*=*=FATAL=*=*=FATAL=*=*=FATAL=*=*=FATAL=*=*=FATAL=*=*=FATAL=*=*=" << std::endl;
  std::cout << std::endl;
  exit(0);
}
void fatal(const std::string& info){
  fatal(std::vector<std::string>({info}));
}
void section(const std::string& title){
  std::cout << std::endl;
  std::cout << "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=" << std::endl;
  std::cout << title << std::endl;
  std::cout << "*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=*=" << std::endl;
  std::cout << std::endl;
}
void subsection(const std::string& title){
  std::cout << " - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -" << std::endl;
  std::cout << "  " << title << std::endl;
  std::cout << " - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -" << std::endl;
}

#endif

