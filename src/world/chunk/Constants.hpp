/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/


#ifndef CHUNK_CONSTANTS_H
#define CHUNK_CONSTANTS_H

#include <cstdint>

// Chunk size exponents (used for bit shifts when calculating array index to find block at x,y,z)
constexpr int CHUNK_SIZE_EXP = 6;
constexpr int CHUNK_SIZE_EXP_X2 = CHUNK_SIZE_EXP * 2;

// Chunk sizes with padding
constexpr int CS_P = 1 << CHUNK_SIZE_EXP;
constexpr int CS_P2 = 1 << CHUNK_SIZE_EXP_X2;
constexpr int CS_P3 = CS_P2 * CS_P;

constexpr int CS_P_MINUS_ONE = CS_P - 1;

// Chunk size without paddings
constexpr int CS = CS_P - 2;
constexpr int CS_MINUS_ONE = CS - 1;

using BlockID = uint8_t;

// Chunk region exponents (used for bitshifts once again)
constexpr int CHUNK_REGION_SIZE_EXP = 4;

// Chunk region sizes
constexpr int CHUNK_REGION_SIZE = 1 << CHUNK_REGION_SIZE_EXP;
constexpr int CHUNK_REGION_SIZE_SQUARED = CHUNK_REGION_SIZE * CHUNK_REGION_SIZE;
constexpr int CHUNK_REGION_SIZE_MINUS_ONE = CHUNK_REGION_SIZE - 1;
constexpr int CHUNK_REGION_BLOCK_SIZE = CS * CHUNK_REGION_SIZE;
// World gen constants
constexpr int DEFAULT_CHUNK_STACK_HEIGHT = 8;
constexpr int DEFAULT_CHUNK_REGION_CHUNK_SIZE = CHUNK_REGION_SIZE_SQUARED * DEFAULT_CHUNK_STACK_HEIGHT;
constexpr int DEFAULT_CHUNK_STACK_HEIGHT_MINUS_ONE = DEFAULT_CHUNK_STACK_HEIGHT - 1;

constexpr int MAX_WORLD_GEN_HEIGHT = (DEFAULT_CHUNK_STACK_HEIGHT * CS) - 1;
constexpr int MIN_WORLD_GEN_HEIGHT = (1 * CS) - 1;
constexpr int MAX_MINUS_MIN_WORLD_GEN_HEIGHT = MAX_WORLD_GEN_HEIGHT - MIN_WORLD_GEN_HEIGHT;
constexpr int WATER_LEVEL = MIN_WORLD_GEN_HEIGHT + CS;

constexpr int CHUNK_BUFFER_PER_FRAME = 20;

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