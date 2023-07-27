#!/usr/bin/env bash

LOGFILE="log_build.txt"

rm ${LOGFILE}
rm ContractionOptimizerCPP.cpython-*
rm *.o
rm a.out

g++ -c -g -DNDEBUG -O3 -Wall -std=c++17 -fPIC diagram.cc -o diagram.o >> ${LOGFILE} 2>&1
g++ -c -g -DNDEBUG -O3 -Wall -std=c++17 -fPIC $(python3 -m pybind11 --includes) contraction_optimizer.cc -o contraction_optimizer.o >> ${LOGFILE} 2>&1
g++ -c -g -DNDEBUG -O3 -Wall -std=c++17 -fPIC graph.cc -o graph.o >> ${LOGFILE} 2>&1
#g++ -g -DNDEBUG -O3 -Wall -std=c++17 $(python3 -m pybind11 --includes) driver.cc contraction_optimizer.o diagram.o graph.o >> ${LOGFILE} 2>&1

g++ -O3 -Wall -shared -std=c++11 -fPIC $(python3 -m pybind11 --includes) python_bindings.cpp graph.o diagram.o contraction_optimizer.o -o ContractionOptimizerCPP$(python3-config --extension-suffix) >> ${LOGFILE} 2>&1
