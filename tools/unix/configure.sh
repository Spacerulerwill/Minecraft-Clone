#! /bin/sh

cmake -DCMAKE_CXX_FLAGS="-Wall -Wextra -Wwrite-strings -Wno-parentheses -Wpedantic -Warray-bounds -Wconversion" -S ../../ -B ../../build/
