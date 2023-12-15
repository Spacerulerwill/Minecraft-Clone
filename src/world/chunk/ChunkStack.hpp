/*
Copyright (C) 2023 William Redding - All Rights Reserved
LICENSE: MIT
*/

#ifndef CHUNK_STACK_H
#define CHUNK_STACK_H

#include <vector>
#include <world/chunk/Chunk.hpp>
#include <opengl/Shader.hpp>
#include <math/Vector.hpp>
#include <PerlinNoise.hpp>

class ChunkStack {
public:
    using iterator = std::vector<Chunk>::iterator;
    using const_iterator = std::vector<Chunk>::const_iterator;
    iterator begin();
    iterator end();
    const_iterator cbegin() const;
    const_iterator cend() const;
    size_t size() const;

    ChunkStack(iVec2 pos);
    void GenerateTerrain(const siv::PerlinNoise& perlin);
    void Draw(Shader& shader);
    void SetBlock(BlockID block, iVec3 pos);
    BlockID GetBlock(iVec3 pos) const;
    Chunk* GetChunk(int y);
private:

    iVec2 mPos{};
    std::vector<Chunk> mChunks;
};

#endif // !CHUNK_STACK_H

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