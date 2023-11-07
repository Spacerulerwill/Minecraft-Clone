/*
Copyright (C) 2023 William Redding - All Rights Reserved
LICENSE: MIT
*/

#ifndef CHUNKREGION_HPP
#define CHUNKREGION_HPP

#include <vector>
#include <util/Constants.hpp>
#include <world/chunk/Chunk.hpp>
#include <world/chunk/ChunkStack.hpp>
#include <core/Shader.hpp>
#include <PerlinNoise.hpp>
#include <concurrentqueue.h>
#include <BS_thread_pool.hpp>
#include <chrono>

namespace engine {

    // Calculate the index of a ChunkStack in a ChunkRegion
    inline int ChunkStackIndex(int x, int z) {
        return z + (x << CHUNK_REGION_SIZE_EXP);
    }

    /*
    Chunk regions are 2D section of chunk stacks. Their size is
    dictated by CHUNK_REGION_SIZE (See util/constants.hpp)
    */
    class ChunkRegion {
    private:
        std::vector<ChunkStack> m_ChunkStacks;
        BS::thread_pool m_TerrainGenPool;
        BS::thread_pool m_ChunkMergePool;
        BS::thread_pool m_ChunkMeshPool;
        
        moodycamel::ConcurrentQueue<Chunk*> m_ChunkBufferQueue;

        bool startedTerrainGeneration = false;
        bool startedChunkMerging = false;
        bool startedChunkMeshing = false;
    public:
        ChunkRegion();
        void GenerateChunks(const siv::PerlinNoise& perlin, std::mt19937& gen, std::uniform_int_distribution<>& distrib);
        void BufferChunksPerFrame(size_t perFrame);
        void DrawOpaque(Shader& opaqueShader);
        void DrawWater(Shader& waterShader);
        void DrawCustomModel(Shader& customModelShader);   
        Chunk* GetChunk(int x, int y, int z); 
    };
}

#endif // !CHUNKREGION_HPP

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