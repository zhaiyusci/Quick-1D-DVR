objects=abstract_representation.o dvr.o sincdvr.o 
CXXFLAGS=-Ofast -Wall -Wextra -I ../eigen --std=c++11
libydvr.a: $(objects)
	$(AR) -cvq libydvr.a $(objects)
abstract_representation.o: abstract_representation.cc config.hh \
 abstract_representation.hh
dvr.o: dvr.cc config.hh dvr.hh representation.hh \
 abstract_representation.hh oscillator.hh
sincdvr.o: sincdvr.cc sincdvr.hh dvr.hh config.hh representation.hh \
 abstract_representation.hh oscillator.hh
.cc.o:
	$(CXX) -c $(CXXFLAGS) $<
.PHONY: clean realclean
clean:
	$(RM) *.o
realclean:
	$(RM) *.o *.a *.so
