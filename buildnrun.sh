#!/bin/bash
# Compiles and runs all .cpp files
g++ --std c++11 -fdiagnostics-color=always ./**.cpp -O3 -o a.out
bash run.sh