# This file is part of yDVR.
#
# Copyright (C) 2017-2020 Yu Zhai <me@zhaiyusci.net>
#
# This Source Code Fo$(RM) is subject to the te$(RM)s of the Mozilla
# Public License v. 2.0. If a copy of the MPL was not distributed
# with this file, You can obtain one at http://mozilla.org/MPL/2.0/.

# CXX=clang++
export CXX
EIGENROOT=./eigen
CCFLAGS=-Ofast -Wall -Wextra --std=c++11 -I$(EIGENROOT) 
LDFLAGS=-static
YDVR=./ydvr

quick1ddvr: quick1ddvr.cc spline.h physical_constants.h iotools.h $(YDVR)/libydvr.a
	$(CXX) -o $@ $(CCFLAGS) $(LDFLAGS) -I$(YDVR) quick1ddvr.cc $(YDVR)/libydvr.a

$(YDVR)/libydvr.a: ydvr
	$(MAKE) -C $(YDVR) 

.PHONY: clean

clean:
	$(MAKE) -C $(YDVR) realclean
	$(RM) quick1ddvr *.o *.a


