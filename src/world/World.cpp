/*
Copyright (C) 2023 William Redding - All Rights Reserved
LICENSE: MIT
*/

#include <world/World.hpp>
#include <GLFW/glfw3.h>
#include <util/Log.hpp>
#include <util/IO.hpp>
#include <fmt/format.h>
#include <util/Exceptions.hpp>
#include <world/Player.hpp>
#include <util/Log.hpp>
#include <chrono>
#include <coroutine>

engine::World::World(const char* worldName) : m_WorldName(worldName) {
    bool success;

    WorldSave worldSave;
    success = ReadStructFromDisk(fmt::format("worlds/{}/world.dat", worldName), worldSave);
    if (!success)
        throw WorldCorruptException(fmt::format("Failed to read world data. File worlds/{}/world.dat is corrupted or missing!", worldName));

    PlayerSave playerSave;
    success = ReadStructFromDisk(fmt::format("worlds/{}/player.dat", worldName), playerSave);
    if (!success)
        throw WorldCorruptException(fmt::format("Failed to read world data. File worlds/{}/player.data is corrupted or missing!", worldName));
    m_Player = Player(playerSave.position, playerSave.pitch, playerSave.yaw);

    m_Noise.reseed(worldSave.seed);
}

engine::Player& engine::World::GetPlayer() {
    return m_Player;
}

const char* engine::World::GetName() {
    return m_WorldName;
}

void engine::World::CreateSpawnChunks(int radius) {

    for (int x = -radius; x <= radius; x++){
        for (int y = 0; y < 3; y++){
            for (int z = -radius; z <= radius; z++) {
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
                        }
                        rf.close();
                    });
                } else {
                    std::ifstream rf(fmt::format("worlds/{}/chunks/{}.{}.{}.chunk", m_WorldName, chunk->m_Pos.x, chunk->m_Pos.y, chunk->m_Pos.z), std::ios::in | std::ios::binary);
                    if (rf) {
                        rf.read((char*)&chunk->m_Voxels[0], CS_P3 * sizeof(BlockInt));
                    } else {
                        chunk->TerrainGen(STONE);
                    }
                    rf.close();
                }
            }
        }
    }
    m_TerrainGenPool.wait_for_tasks();
    
    for (auto& [key, chunk]: m_ChunkMap) {
        m_MergePool.push_task([&chunk,this]{
            SetNeighborsEdgeData(&chunk, nullptr);
        });
    }
    m_MergePool.wait_for_tasks();

    for (int x = -radius; x <= radius; x++){
        for (int y = 0; y < 3; y++){
            for (int z = -radius; z <= radius; z++) {
                Chunk* chunk = GetChunk(Vec3<int>(x,y,z));
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


void engine::World::Draw(Shader& chunkShader, Shader& waterShader, Shader& customModelShader) {
    Mat4<float> perspective = m_Player.GetCamera().GetPerspectiveMatrix();
    Mat4<float> view = m_Player.GetCamera().GetViewMatrix();
    const VoxelRaycastResult& raycastResult = m_Player.GetBlockRaycastResult();

    chunkShader.Bind();
    chunkShader.setMat4("projection", perspective);
    chunkShader.setMat4("view", view);
    chunkShader.SetInt("tex_array", 0);
    chunkShader.SetInt("grass_mask", 2);
    for (Chunk* chunk : m_ChunkDrawVector){
        chunk->Draw(chunkShader);
    }

    //glDisable(GL_CULL_FACE);
    customModelShader.Bind();
    customModelShader.setMat4("projection", perspective);
    customModelShader.setMat4("view", view);
    customModelShader.SetInt("tex_array", 0);
    for (Chunk* chunk : m_ChunkDrawVector){
        chunk->DrawCustomModelBlocks(customModelShader);
    }
    //glEnable(GL_CULL_FACE);

    glDepthFunc(GL_LEQUAL);
    m_Skybox.Draw(perspective, translationRemoved(view));
    glDepthFunc(GL_LESS);

    glEnable(GL_BLEND);
    waterShader.Bind();
    waterShader.setMat4("projection", perspective);
    waterShader.setMat4("view", view);
    waterShader.SetInt("tex_array", 0);
    for (Chunk* chunk : m_ChunkDrawVector){
        chunk->DrawWater(waterShader);
    }
    glDisable(GL_BLEND);
}

engine::Chunk* engine::World::CreateChunk(Vec3<int> pos) {
    auto result = m_ChunkMap.try_emplace(pos, pos);
    return  &(result.first->second);   
}

engine::Chunk* engine::World::GetChunk(Vec3<int> pos) {
    auto find = m_ChunkMap.find(pos);
    auto end = m_ChunkMap.end();

    if (find == end) {
        return nullptr;
    } else {
        return &(find->second);
    }
}

void engine::World::AddChunkToDrawVector(Chunk* chunk) {
    m_ChunkDrawVector.push_back(chunk);
}

engine::World::~World() {
    // Save player position and view direction
    PlayerSave playerSave {
        .position = m_Player.GetCamera().GetPosition(),
        .pitch = m_Player.GetCamera().GetPitch(),
        .yaw = m_Player.GetCamera().GetYaw()
    };
    WriteStructToDisk(fmt::format("worlds/{}/player.dat", m_WorldName), playerSave);
}

void engine::World::SetNeighborsEdgeData(Chunk* chunk, ChunkGroup* chunkGroup) {
    Vec3<int> chunkStripOrientation;
    if (chunkGroup != nullptr) {
        chunkStripOrientation = chunkGroup->orientation;
    }
    else {
        chunkStripOrientation = Vec3<int>(0,0,0);
    }

    Chunk* aboveChunk = GetChunk(chunk->m_Pos + Vec3<int>(0,1,0));
    if (aboveChunk != nullptr) {
        for (int x = 1; x < CS_P_MINUS_ONE; x++){
            for (int z = 1; z < CS_P_MINUS_ONE; z++){
                aboveChunk->SetBlock(chunk->GetBlock(x, CS, z), x, 0, z);
            }
        }
        aboveChunk->needsRemeshing = true;
    }

    Chunk* belowChunk = GetChunk(chunk->m_Pos - Vec3<int>(0,1,0));
    if (belowChunk != nullptr) {
        for (int x = 1; x < CS_P_MINUS_ONE; x++){
            for (int z = 1; z < CS_P_MINUS_ONE; z++){
                belowChunk->SetBlock(chunk->GetBlock(x, 1, z), x, CS_P_MINUS_ONE, z);
            }
        }
        belowChunk->needsRemeshing = true;
    }

    Chunk* leftChunk = GetChunk(chunk->m_Pos - Vec3<int>(1, 0, 0));
    if (leftChunk != nullptr) {
        for (int y = 1; y < CS_P_MINUS_ONE; y++){
            for(int z = 1; z < CS_P_MINUS_ONE; z++) {
                leftChunk->SetBlock(chunk->GetBlock(1, y, z), CS_P_MINUS_ONE, y, z);
                if (chunkStripOrientation.x == 1) {
                    chunk->SetBlock(leftChunk->GetBlock(CS, y, z), 0, y, z);
                }
            }
        }
        leftChunk->needsRemeshing = true;
    }

    Chunk* rightChunk = GetChunk(chunk->m_Pos + Vec3<int>(1, 0, 0));
    if (rightChunk != nullptr) {
        for (int y = 1; y < CS_P_MINUS_ONE; y++){
            for(int z = 1; z < CS_P_MINUS_ONE; z++) {
                rightChunk->SetBlock(chunk->GetBlock(CS, y, z), 0, y, z);
                if (chunkStripOrientation.x == -1) {
                    chunk->SetBlock(rightChunk->GetBlock(1, y, z), CS_P_MINUS_ONE, y, z);
                }
            }
        }
        rightChunk->needsRemeshing = true;
    }

    Chunk* behindChunk = GetChunk(chunk->m_Pos + Vec3<int>(0, 0, 1));
    if (behindChunk != nullptr) {
        for (int x = 1; x < CS_P_MINUS_ONE; x++){
            for(int y = 1; y < CS_P_MINUS_ONE; y++) {
                behindChunk->SetBlock(chunk->GetBlock(x, y, CS), x, y, 0);
                if (chunkStripOrientation.z == -1) {
                    chunk->SetBlock(behindChunk->GetBlock(x, y, 1), x, y, CS_P_MINUS_ONE);
                }
        
            }
        }
        behindChunk->needsRemeshing = true;
    }

    Chunk* frontChunk = GetChunk(chunk->m_Pos - Vec3<int>(0, 0, 1));
    if (frontChunk != nullptr) {
        for (int x = 1; x < CS_P_MINUS_ONE; x++){
            for(int y = 1; y < CS_P_MINUS_ONE; y++) {
                frontChunk->SetBlock(chunk->GetBlock(x,y,1), x, y, CS_P_MINUS_ONE);
                if (chunkStripOrientation.z == 1) {
                    chunk->SetBlock(frontChunk->GetBlock(x, y, CS), x, y, 0);
                }
            }
        }
        frontChunk->needsRemeshing = true;
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