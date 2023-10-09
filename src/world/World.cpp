/*
Copyright (C) 2023 William Redding - All Rights Reserved
LICENSE: MIT
*/

#include <world/World.hpp>
#include <GLFW/glfw3.h>
#include <util/Log.hpp>
#include <fmt/format.h>
#include <yaml-cpp/yaml.h>

engine::World::World(const char* worldName) : m_WorldName(worldName) {
    YAML::Node worldYaml = YAML::LoadFile(fmt::format("worlds/{}/world.yml", worldName));
    siv::PerlinNoise::seed_type seed = worldYaml["seed"].as<siv::PerlinNoise::seed_type>();
    m_Noise.reseed(seed);
}

engine::Chunk* engine::World::CreateChunk(Vec3<int> pos) {
    auto result = m_ChunkMap.try_emplace(pos, pos);
    return  &(result.first->second);   
}

void engine::World::CreateChunks(int chunkX, int chunkY, int chunkZ, int radius, int bufferPerFrame) {

    int chunksCreated = 0;

    // erase chunks no longer within the view distance from the m_ChunkDrawVector
    m_ChunkDrawVector.erase(std::remove_if(m_ChunkDrawVector.begin(), m_ChunkDrawVector.end(), [chunkX, chunkY, chunkZ, radius](Chunk* chunk) { 
        return chunk->m_Pos.x < chunkX-radius ||
        chunk->m_Pos.x > chunkX+radius - 1||
        chunk->m_Pos.y < chunkY-radius ||
        chunk->m_Pos.y > chunkY+radius-1 ||
        chunk->m_Pos.z < chunkZ-radius ||
        chunk->m_Pos.z > chunkZ+radius - 1;
    }), m_ChunkDrawVector.end());
    
    std::vector<std::unordered_map<engine::Vec3<int>, engine::Chunk>::iterator> iterators;

    // Lock mutex for map access and unload each chunk outside view distance (single threaded)
    for (auto it = m_ChunkMap.begin(); it != m_ChunkMap.end();) {
        Chunk* chunk = &((*it).second);
        if (
            (chunk->m_Pos.x < chunkX - radius ||
            chunk->m_Pos.x > chunkX + radius - 1 ||
            chunk->m_Pos.y < chunkY - radius ||
            chunk->m_Pos.y > chunkY + radius - 1 ||
            chunk->m_Pos.z < chunkZ - radius ||
            chunk->m_Pos.z > chunkZ + radius - 1)
            && !chunk->isBeingMeshed && !chunk->isInBufferQueue
        ) {
            if (chunk->dirty) {
                m_UnloadPool.push_task([chunk, this]{
                    chunk->UnloadToFile(m_WorldName);
                });
            }
            iterators.push_back(it);
            ++it;
        } else {
            ++it;
        }
    } 
    m_UnloadPool.wait_for_tasks();
    for (const auto& it: iterators)
        m_ChunkMap.erase(it);
    
    // Generate new chunks
    for (int iz = -radius; iz < radius; iz++) {
        for (int iy = -radius; iy < radius; iy++) {
            for (int ix = -radius; ix < radius; ix++) {
                
                Vec3 vec = Vec3(chunkX + ix, chunkY + iy, chunkZ + iz);
                Chunk* chunk = nullptr;

                auto find = m_ChunkMap.find(vec); 
                auto end = m_ChunkMap.end();  

                if (find == end && chunksCreated < bufferPerFrame) {
                    if (chunkY + iy < 3 && chunkY + iy >= 0) {
                        chunksCreated++;
                        auto result = m_ChunkMap.try_emplace(vec, chunkX + ix, chunkY + iy, chunkZ + iz);
                        chunk = &(result.first->second);        

                        chunk->isBeingMeshed = true;
                        m_MeshPool.push_task([chunk, chunkY, iy, this] {
                            std::ifstream rf(fmt::format("worlds/{}/chunks/{}.{}.{}.chunk", m_WorldName, chunk->m_Pos.x, chunk->m_Pos.y, chunk->m_Pos.z), std::ios::in | std::ios::binary);
                            if (!rf) {
                                if (chunkY + iy < 2) {
                                    chunk->TerrainGen(STONE);
                                } else {
                                    chunk->TerrainGen(m_Noise, gen, distrib);
                                }
                                chunk->UnloadToFile(m_WorldName);
                            }else {
                                rf.read((char*)&chunk->m_Voxels[0], CS_P3 * sizeof(BlockInt));
                                rf.close();
                            }
                            chunk->CreateMesh();
                            chunk->isInBufferQueue = true;
                            m_ChunkBufferQueue.enqueue(chunk);
                            chunk->isBeingMeshed = false;
                        });
                    } 
                }
            }
        }
    }
    
    // Buffer chunks that
    for (int i = 0; i < bufferPerFrame; i++){
        Chunk* chunk = nullptr;
        bool success = m_ChunkBufferQueue.try_dequeue(chunk);
        if (success) {
            chunk->isInBufferQueue = false;
            chunk->BufferData();
            m_ChunkDrawVector.push_back(chunk);
        }
    }
}

void engine::World::DrawBlocks(Shader& chunkShader) {
    for (Chunk* chunk : m_ChunkDrawVector){
        chunk->Draw(chunkShader);
    }
}
void engine::World::DrawWater(Shader& waterShader) {
     for (Chunk* chunk : m_ChunkDrawVector){
        chunk->DrawWater(waterShader);
    }
}
void engine::World::DrawCustomModelBlocks(Shader& customModelShader) {
     for (Chunk* chunk : m_ChunkDrawVector){
        chunk->DrawCustomModelBlocks(customModelShader);
    }
}

engine::Chunk* engine::World::GetChunk(int chunkX, int chunkY, int chunkZ) {
    auto find = m_ChunkMap.find(Vec3(chunkX, chunkY, chunkZ));

    if (find == m_ChunkMap.end()) {
        return nullptr;
    } else {
        return &(find->second);
    }
}

engine::Chunk* engine::World::GetChunk(Vec3<int> pos) {
    auto find = m_ChunkMap.find(pos);

    if (find == m_ChunkMap.end()) {
        return nullptr;
    } else {
        return &(find->second);
    }
}

engine::World::~World() {

    // Unload all remaining chunks
    for (auto& [key, chunk]: m_ChunkMap) {
        if (chunk.dirty) {
            chunk.UnloadToFile(m_WorldName);
        }
    }
}
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