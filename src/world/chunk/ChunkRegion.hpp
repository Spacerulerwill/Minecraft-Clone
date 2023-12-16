/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#ifndef CHUNK_REGION_H
#define CHUNK_REGION_H

#include <world/chunk/ChunkStack.hpp>
#include <world/chunk/Chunk.hpp>
#include <opengl/Shader.hpp>
#include <PerlinNoise.hpp>
#include <concurrentqueue.h>
#include <BS_thread_pool.hpp>
#include <math/Vector.hpp>
#include <vector>
#include <array>
#include <atomic>

inline int ChunkStackIndex(int x, int z) {
    return z + (x << CHUNK_REGION_SIZE_EXP);
}

class ChunkRegion {
private:
    iVec2 mPos;
    std::vector<ChunkStack> mChunkStacks;
    std::array<Chunk*, CHUNK_BUFFER_PER_FRAME> mChunkBufferDequeResult{};
<<<<<<< HEAD
    moodycamel::ConcurrentQueue<Chunk*> mChunkBufferQueue;
=======
>>>>>>> tmp
    BS::thread_pool mTaskPool;
    bool mHasStartedTerrainGeneration = false;
    bool mHasStartedChunkMerging = false;
    bool mHasStartedChunkMeshing = false;
public:
    ChunkRegion(iVec2 pos);
    iVec2 GetPosition() const;
    void GenerateChunks(const siv::PerlinNoise& perlin);
    void Draw(Shader& shader);
    void BufferChunks();
    Chunk* GetChunk(iVec3 pos);
    ChunkStack* GetChunkStack(iVec2 pos);
    void PrepareForDeletion();
    std::atomic<bool> startedDeletion = false;
    std::atomic<bool> readyForDeletion = false;
};

#endif // !CHUNK_REGION_H

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