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
    Chunk regions are 2D section of chunk stacks.
    */
    class ChunkRegion {
    private:
        Vec2<int> m_Pos;
        std::vector<ChunkStack> m_ChunkStacks;

        // Thread pools to speed up generation tasks
        BS::thread_pool m_TerrainGenPool;
        BS::thread_pool m_ChunkMergePool;
        BS::thread_pool m_ChunkMeshPool;
        
        /* A concurrent queue to store chunks that need buffering 
        AFTER they have been terrain generatied, merged and meshed.
        */
        moodycamel::ConcurrentQueue<Chunk*> m_ChunkBufferQueue;

        // Flags indiciating the state of the chunk generation process
        bool startedTerrainGeneration = false;
        bool startedChunkMerging = false;
        bool startedChunkMeshing = false;

        // A temporary storage space to store chunks when bulk dequeuing from m_ChunkBufferQueue
        Chunk* m_ChunkBufferQueueDequeueResult[MAX_BUFFER_PER_FRAME] {};
    public:
        ChunkRegion(Vec2<int> pos);

        // Get the world space chunk region position
        Vec2<int> GetPos() const;

        // Generate chunks, tries to progress through the chunk generation pipeline
        void GenerateChunks(const siv::PerlinNoise& perlin, std::mt19937& gen, std::uniform_int_distribution<>& distrib);

        /*
        Dequeues at most MAX_BUFFER_PER_FRAME chunks from m_ChunkBufferQueue and sends their data
        to the GPU for drawing. 
        */ 
        void BufferChunksPerFrame();
        
        /*
        Drawing functions for the 3 different components of the chunk mesh.
        They iterate over the chunk stacks in the region and call their respective identically named functions
        */
        void DrawOpaque(Shader& opaqueShader);
        void DrawWater(Shader& waterShader);
        void DrawCustomModel(Shader& customModelShader);   

        // Get a chunk from the region using its region local chunk position
        Chunk* GetChunk(Vec3<int> pos); 
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