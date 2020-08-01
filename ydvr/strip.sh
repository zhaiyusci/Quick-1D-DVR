#!/bin/bash

FULLYDVR="../../ydvr" # Can be change

CCFILE="abstract_representation dvr sincdvr"
HHFILE="config abstract_representation dvr sincdvr oscillator representation"

echo -n "" > Makefile
echo -n "objects=" >> Makefile
for i in ${CCFILE} 
do
  echo -n "${i}.o " >> Makefile
done
echo "" >> Makefile

echo "CXXFLAGS=-Ofast -Wall -Wextra -I ../eigen --std=c++11" >> Makefile
echo "libydvr.a: \$(objects)" >> Makefile
echo -e "\t\$(AR) -cvq libydvr.a \$(objects)" >> Makefile

dealwith(){
  echo -n "Copy $1 from a full version of yDVR ..."
  cp ${FULLYDVR}/src/$1 .
  echo "... done."
  echo -n "Remove it's dependent on Log system ..."
  sed -i "s/\<LOG\>/\/\//g" $1
  sed -i "s/#include\"log.hh\"//g" $1
  sed -i "s/Log/\/\/ /g" $1
  echo "... done."
}

for i in ${CCFILE}
do
  dealwith $i.cc
  g++ -MM $i.cc >> Makefile
done
echo -n "" >> ydvr.hh
for i in ${HHFILE}
do
  dealwith $i.hh
  echo "#include\"$i.hh\"" >> ydvr.hh
done

echo ".cc.o:" >> Makefile
echo -e "\t\$(CXX) -c \$(CXXFLAGS) \$<" >> Makefile
echo ".PHONY: clean realclean" >> Makefile
echo "clean:" >> Makefile
echo -e "\t\$(RM) *.o" >> Makefile
echo "realclean:" >> Makefile
echo -e "\t\$(RM) *.o *.a *.so" >> Makefile
