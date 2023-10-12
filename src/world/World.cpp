/*
Copyright (C) 2023 William Redding - All Rights Reserved
LICENSE: MIT
*/

#include <world/World.hpp>
#include <GLFW/glfw3.h>
#include <util/Log.hpp>
#include <util/IO.hpp>
#include <fmt/format.h>

engine::World::World(const char* worldName) : m_WorldName(worldName) {
    WorldSave save;
    ReadStructFromDisk(fmt::format("worlds/{}/world.dat", worldName), save);
    m_Noise.reseed(save.seed);
}

void engine::World::CreateSpawnChunks(int radius) {

    for (int x = -radius; x < radius; x++){
        for (int y = 0; y < 3; y++){
            for (int z = -radius; z < radius; z++) {
                Vec3<int> chunkPos(x,y,z);
                auto result = m_ChunkMap.try_emplace(chunkPos, chunkPos);
                Chunk* chunk = &(result.first->second);  
                if (y == 2) { 
                    m_TerrainGenPool.push_task([chunk, this] {
                        std::ifstream rf(fmt::format("worlds/{}/chunks/{}.{}.{}.chunk", m_WorldName, chunk->m_Pos.x, chunk->m_Pos.y, chunk->m_Pos.z), std::ios::in | std::ios::binary);
                        if (rf) {
                            rf.read((char*)&chunk->m_Voxels[0], CS_P3 * sizeof(BlockInt));
                        } else {
                            chunk->TerrainGen(m_Noise, gen, distrib);
                            chunk->UnloadToFile(m_WorldName);
                        }
                        rf.close();
                    });
                } else {
                    std::ifstream rf(fmt::format("worlds/{}/chunks/{}.{}.{}.chunk", m_WorldName, chunk->m_Pos.x, chunk->m_Pos.y, chunk->m_Pos.z), std::ios::in | std::ios::binary);
                    if (rf) {
                        rf.read((char*)&chunk->m_Voxels[0], CS_P3 * sizeof(BlockInt));
                    } else {
                        chunk->TerrainGen(STONE);
                        chunk->UnloadToFile(m_WorldName);
                    }
                    rf.close();
                }
            }
        }
    }
    m_TerrainGenPool.wait_for_tasks();
    
    for (auto& [key, chunk]: m_ChunkMap) {
        Chunk* aboveChunk = GetChunk(chunk.m_Pos + Vec3<int>(0,1,0));
        
        if (aboveChunk != nullptr) {
            for (int x = 1; x < CS_P_MINUS_ONE; x++){
                for (int z = 1; z < CS_P_MINUS_ONE; z++){
                    chunk.SetBlock(aboveChunk->GetBlock(x, 1, z), x, CS_P_MINUS_ONE, z);
                }
            }
        }

        Chunk* belowChunk = GetChunk(chunk.m_Pos - Vec3<int>(0,1,0));
        
        if (belowChunk != nullptr) {
            for (int x = 1; x < CS_P_MINUS_ONE; x++){
                for (int z = 1; z < CS_P_MINUS_ONE; z++){
                    chunk.SetBlock(belowChunk->GetBlock(x, CS, z), x, 0, z);
                }
            }
        }

        Chunk* leftChunk = GetChunk(chunk.m_Pos - Vec3<int>(1, 0, 0));

        if (leftChunk != nullptr) {
            for (int y = 1; y < CS_P_MINUS_ONE; y++){
                for(int z = 1; z < CS_P_MINUS_ONE; z++) {
                    chunk.SetBlock(leftChunk->GetBlock(CS, y, z), 0, y, z);
                }
            }
        }

        Chunk* rightChunk = GetChunk(chunk.m_Pos + Vec3<int>(1, 0, 0));

        if (rightChunk != nullptr) {
            for (int y = 1; y < CS_P_MINUS_ONE; y++){
                for(int z = 1; z < CS_P_MINUS_ONE; z++) {
                    chunk.SetBlock(rightChunk->GetBlock(1, y, z), CS_P_MINUS_ONE, y, z);
                }
            }
        }

        Chunk* behindChunk = GetChunk(chunk.m_Pos + Vec3<int>(0, 0, 1));

        if (behindChunk != nullptr) {
            for (int x = 1; x < CS_P_MINUS_ONE; x++){
                for(int y = 1; y < CS_P_MINUS_ONE; y++) {
                    chunk.SetBlock(behindChunk->GetBlock(x, y, 1), x, y, CS_P_MINUS_ONE);
                }
            }
        }

        Chunk* frontChunk = GetChunk(chunk.m_Pos - Vec3<int>(0, 0, 1));

        if (frontChunk != nullptr) {
            for (int x = 1; x < CS_P_MINUS_ONE; x++){
                for(int y = 1; y < CS_P_MINUS_ONE; y++) {
                    chunk.SetBlock(frontChunk->GetBlock(x, y, CS), x, y, 0);
                }
            }
        }
    }

    for (int x = -radius; x < radius; x++){
        for (int y = 0; y < 3; y++){
            for (int z = -radius; z < radius; z++) {
                Chunk* chunk = GetChunk(x,y,z);
                m_MeshPool.push_task([chunk]{
                    chunk->CreateMesh();
                });
            }
        }
    }

    m_MeshPool.wait_for_tasks();
    
    for (auto& [key, chunk]: m_ChunkMap) {
        chunk.BufferData();
        m_ChunkDrawVector.push_back(&chunk);
    }
}

engine::Chunk* engine::World::CreateChunk(Vec3<int> pos) {
    auto result = m_ChunkMap.try_emplace(pos, pos);
    return  &(result.first->second);   
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