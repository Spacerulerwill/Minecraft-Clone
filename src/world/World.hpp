/*
Copyright (C) 2023 William Redding - All Rights Reserved
LICENSE: MIT
*/

#ifndef WORLD_H
#define WORLD_H

#include <unordered_map>
#include <vector>
#include <queue>
#include <world/Chunk.hpp>
#include <math/Vec3.hpp>
#include <PerlinNoise.hpp>
#include <core/Shader.hpp>
#include <BS_thread_pool.hpp>

namespace engine {
    class World {
    public:
        World(siv::PerlinNoise::seed_type seed);
        void CreateChunks(int chunkX, int chunkZ, int radius, int bufferPerFrame);
        void DrawOpaque(Shader& chunkShader);
        void DrawWater(Shader& waterShader);
        void DrawCustomModelBlocks(Shader& customModelShader);
        ~World();
    private:
        std::unordered_map<Vec3, Chunk*> m_ChunkMap;
        siv::PerlinNoise::seed_type m_Seed;
        siv::PerlinNoise m_Noise;
        std::vector<Chunk*> m_ChunkDrawVector;
        BS::thread_pool m_ThreadPool = BS::thread_pool(50);
        std::queue<Chunk*> m_ChunkMeshQueue;
        std::queue<Chunk*> m_ChunkBufferQueue;
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