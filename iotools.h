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
#include<cstdlib>
#include<utility>
#include<cstdio>

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

#endif

