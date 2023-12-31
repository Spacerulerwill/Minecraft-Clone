/*
Copyright (C) 2023 William Redding - All Rights Reserved
LICENSE: MIT
*/

#ifndef CHUNK_STACK_H
#define CHUNK_STACK_H

#include <vector>
#include <world/chunk/Chunk.hpp>
#include <world/Block.hpp>
#include <opengl/Shader.hpp>
#include <math/Vector.hpp>
#include <PerlinNoise.hpp>
#include <world/Block.hpp>

class ChunkStack {
private:
    iVec2 mPos{};
    std::vector<std::shared_ptr<Chunk>> mChunks;
public:
    using iterator = std::vector<std::shared_ptr<Chunk>>::iterator;
    using const_iterator = std::vector<std::shared_ptr<Chunk>>::const_iterator;
    iterator begin();
    iterator end();
    const_iterator cbegin() const;
    const_iterator cend() const;
    size_t size() const;

    ChunkStack(iVec2 pos);
    void GenerateTerrain(const siv::PerlinNoise& perlin);
    void Draw(Shader& shader, float currentTime);
    iVec2 GetPosition() const;
    std::atomic_bool isBeingmMeshed = false;

    std::shared_ptr<Chunk> GetChunk(std::size_t y) const;
    BlockID GetBlock(iVec3 pos) const;
    void SetBlock(iVec3 pos, BlockID block);

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