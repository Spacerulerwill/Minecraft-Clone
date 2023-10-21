/*
Copyright (C) 2023 William Redding - All Rights Reserved
LICENSE: MIT
*/

#ifndef WORLD_H
#define WORLD_H

#include <unordered_map>
#include <concurrentqueue.h>
#include <BS_thread_pool.hpp>
#include <PerlinNoise.hpp>
#include <world/Chunk.hpp>
#include <world/Skybox.hpp>
#include <vector>
#include <mutex>
#include <world/Player.hpp>
#include <util/MutexHolder.hpp>

namespace engine {
    struct WorldSave {
        siv::PerlinNoise::seed_type seed;
    };

    struct PlayerSave {
        Vec3<float> position;
        float pitch;
        float yaw;
    };

    struct ChunkGroup {
        mutex_holder mHolder;
        std::vector<Chunk*> group;
        Vec3<int> orientation;
    };

    class World {
    public:
        World(const char* worldName);
        Chunk* CreateChunk(Vec3<int> pos);
        Chunk* GetChunk(Vec3<int> pos);
        void AddChunkToDrawVector(Chunk* chunk);
        Player& GetPlayer();
        const char* GetName();
        void CreateSpawnChunks(int radius);
        void GenerateNewChunks(int64_t remainingFrameTime, int radius, Vec3<int> prevChunkPos, Vec3<int> newChunkPos);
        void MeshNewChunks(int radius, Vec3<int> chunkPos, const int bufferPerFrames);
        void Draw(Shader& chunkShader, Shader& waterShader, Shader& customModelShader);
        ~World();
    private:
        Player m_Player;
        Skybox m_Skybox;

        const char* m_WorldName = nullptr;
        Vec3<int> playerChunkPos;

        std::vector<Chunk*> m_ChunkDrawVector;
        std::queue<ChunkGroup*> m_ChunkGroups;
        BS::thread_pool m_TerrainGenPool;
        BS::thread_pool m_MergePool;
        BS::thread_pool m_MeshPool;
        BS::thread_pool m_UnloadPool;
        moodycamel::ConcurrentQueue<Chunk*> m_ChunkBufferQueue;

        std::mutex m_MapMutex;
        std::unordered_map<Vec3<int>, Chunk> m_ChunkMap;
        siv::PerlinNoise m_Noise = siv::PerlinNoise(0);
        
        std::random_device rd;
        std::mt19937 gen = std::mt19937(rd());
        std::uniform_int_distribution<> distrib = std::uniform_int_distribution<>(1, 100);

        void SetNeighborsEdgeData(Chunk* chunk, const Vec3<int>& stripOrientation);
        void EraseChunk(Vec3<int>pos);
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