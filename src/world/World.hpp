/*
Copyright (C) 2023 William Redding - All Rights Reserved
LICENSE: MIT
*/

#ifndef WORLD_H
#define WORLD_H

#include <unordered_map>
#include <BS_thread_pool.hpp>
#include <concurrentqueue.h>
#include <PerlinNoise.hpp>
#include <core/Shader.hpp>
#include <world/Chunk.hpp>
#include <math/Vec2.hpp>
#include <algorithm>
#include <memory>
#include <vector>
#include <mutex>

namespace engine {
    class World {
    public:
        World(const char* worldName);
        void DrawBlocks(Shader& shader);
        void DrawWater(Shader& shader);
        void DrawCustomModelBlocks(Shader& shader);
        void CreateChunks(int chunkX, int chunkY, int chunkZ, int radius, int bufferPerFrame);
        Chunk* GetChunk(int chunkX, int chunkY, int chunkZ);
        ~World();
    private:
        std::mutex mtx;
        const char* m_WorldName = nullptr;
        BS::thread_pool m_MeshPool;
        moodycamel::ConcurrentQueue<Chunk*> m_ChunkBufferQueue;
        std::vector<Chunk*> m_ChunkDrawVector;
        std::unordered_map<Vec3<int>, Chunk> m_ChunkMap;
        siv::PerlinNoise m_Noise = siv::PerlinNoise(0);
        
        std::random_device rd;
        std::mt19937 gen = std::mt19937(rd());
        std::uniform_int_distribution<> distrib = std::uniform_int_distribution<>(1, 100);
    };
}

#endif // !WORLD_H

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