# This file is part of yDVR.
#
# Copyright (C) 2017-2020 Yu Zhai <me@zhaiyusci.net>
#
# This Source Code Fo$(RM) is subject to the te$(RM)s of the Mozilla
# Public License v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

CXX=em++
export CXX
EIGENROOT=./eigen
CCFLAGS=-O3 -Wall -Wextra --std=c++11 -I$(EIGENROOT) 
LDFLAGS=-static
YDVR=./ydvr

quick1ddvr.html: quick1ddvr.cc spline.h physical_constants.h iotools.h $(YDVR)/libydvr.a
	$(CXX) -o $@ $(CCFLAGS) $(LDFLAGS) -I$(YDVR) quick1ddvr.cc $(YDVR)/libydvr.a --embed-file ./sample/hcl/sample_hcl.inp@sample_hcl.inp

$(YDVR)/libydvr.a: ydvr
	$(MAKE) -C $(YDVR) 

.PHONY: clean

clean:
	$(MAKE) -C $(YDVR) realclean
	$(RM) quick1ddvr *.o *.a


