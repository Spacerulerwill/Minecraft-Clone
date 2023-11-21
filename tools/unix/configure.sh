#! /bin/sh

cmake -DCMAKE_CXX_FLAGS="-Wall -Wextra -Wwrite-strings -Wno-parentheses -Wpedantic -Warray-bounds -Wconversion -fanalyzer -fsanitize=undefined" -S ../../ -B ../../build/ >> output.txt 2>&1