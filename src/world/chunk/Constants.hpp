/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/


#ifndef CHUNK_CONSTANTS_H
#define CHUNK_CONSTANTS_H

// Chunk size exponents (used for bit shifts when calculating array index to find block at (x,y,z)
constexpr const int CHUNK_SIZE_EXP = 6;
constexpr const int CHUNK_SIZE_EXP_X2 = CHUNK_SIZE_EXP * 2;

// Chunk sizes with padding
constexpr const int CS_P = 1 << CHUNK_SIZE_EXP;
constexpr const int CS_P2 = 1 << CHUNK_SIZE_EXP_X2;
constexpr const int CS_P3 = CS_P2 * CS_P;

constexpr const int CS_P_MINUS_ONE = CS_P - 1;

// Chunk size without paddings
constexpr const int CS = CS_P - 2;
constexpr const int CS_MINUS_ONE = CS - 1;
constexpr const float CS_OVER_2 = CS / 2.0f;

// World gen constants
constexpr const int DEFAULT_CHUNK_STACK_HEIGHT = 8;
constexpr const int DEFAULT_CHUNK_STACK_HEIGHT_MINUS_ONE = DEFAULT_CHUNK_STACK_HEIGHT - 1;

constexpr const int MAX_WORLD_GEN_HEIGHT = (DEFAULT_CHUNK_STACK_HEIGHT * CS) - 1;
constexpr const int MIN_WORLD_GEN_HEIGHT = (1 * CS) - 1;
constexpr const int MAX_MINUS_MIN_WORLD_GEN_HEIGHT = MAX_WORLD_GEN_HEIGHT - MIN_WORLD_GEN_HEIGHT;

constexpr const int WATER_LEVEL = MIN_WORLD_GEN_HEIGHT + CS_P * 4;

#endif // !CHUNK_CONSTANTS_H

/*
MIT License

Copyright (c) 2023 William Redding

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/