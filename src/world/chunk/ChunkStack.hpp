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
#include <math/Frustum.hpp>
#include <PerlinNoise.hpp>
#include <world/Block.hpp>
#include <atomic>
#include <memory>

enum class ChunkStackState {
    NOT_INITIALISED,
    UNLOADED,
    PARTIALLY_LOADED,
    LOADED
};

class ChunkStack {
private:
    iVec2 mPos{};
    std::vector<std::shared_ptr<Chunk>> mChunks;
    void SaveToFile(const std::string& worldDirectory);
public:
    using iterator = std::vector<std::shared_ptr<Chunk>>::iterator;
    using const_iterator = std::vector<std::shared_ptr<Chunk>>::const_iterator;
    iterator begin();
    iterator end();
    const_iterator cbegin() const;
    const_iterator cend() const;
    size_t size() const;
    static constexpr std::size_t DEFAULT_SIZE = 8;
    ChunkStack(iVec2 pos);
    void GenerateTerrain(siv::PerlinNoise::seed_type seed, const siv::PerlinNoise& perlin);
    void Draw(Shader& shader, int* totalChunks, int* chunksDrawn);
    void DrawWater(Shader& shader, int* totalChunks, int* chunksDrawn);
    void DrawCustomModel(Shader& shader, int* totalChunks, int* chunksDrawn);
    iVec2 GetPosition() const;
    std::shared_ptr<Chunk> GetChunk(std::size_t y) const;
    Block RawGetBlock(iVec3 pos) const;
    void RawSetBlock(iVec3 pos, Block block);
    Block GetBlock(iVec3 pos) const;
    void SetBlock(iVec3 pos, Block block);
    void FullyLoad(const std::string& worldDirectory, siv::PerlinNoise::seed_type seed, const siv::PerlinNoise& perlin);
    void PartiallyLoad(const std::string& worldDirectory, siv::PerlinNoise::seed_type seed, const siv::PerlinNoise& perlin);
    void Unload(const std::string& worldDirectory);;
    std::atomic<ChunkStackState> state = ChunkStackState::NOT_INITIALISED;
    std::atomic<bool> is_in_task = false;
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
