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
        m_MergePool.push_task([&chunk,this]{
            SetNeighborsEdgeData(&chunk, Vec3<int>(0,0,0));
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

void engine::World::MeshNewChunks(int radius, Vec3<int> chunkPos, int bufferPerFrame) {
    // Merge chunk sides tog    ether in our chunk group
    if (m_ChunkGroups.size() > 0) {
        ChunkGroup* nextGroup = m_ChunkGroups.front();
        if (nextGroup != nullptr) {
            nextGroup->mHolder.mutex.lock();
            int groupSize = nextGroup->group.size();
            nextGroup->mHolder.mutex.unlock();
            if (groupSize == 3 * ((radius * 2) + 1)) {
                nextGroup->mHolder.mutex.lock();
                
                m_MapMutex.lock();
                for (auto& chunk : nextGroup->group) {
                    m_MergePool.push_task([chunk, nextGroup, this]{
                        SetNeighborsEdgeData(chunk, nextGroup->orientation);
                    });
                }
                m_MergePool.wait_for_tasks();
                m_MapMutex.unlock();

                // Create meshing tasks for new chunks
                for (auto& [key, chunk]: m_ChunkMap) {
                    if (chunk.needsRemeshing) {
                        m_MeshPool.push_task([&chunk, this]{
                            chunk.CreateMesh();
                            m_ChunkBufferQueue.enqueue(&chunk);
                        });
                    }
                }

                m_ChunkGroups.pop();
                nextGroup->mHolder.mutex.unlock();
                delete nextGroup;
            }
        }
    }

    // try and buffer chunks
    Chunk* chunksToBuffer[bufferPerFrame];
    int dequeued = m_ChunkBufferQueue.try_dequeue_bulk(chunksToBuffer, bufferPerFrame);
    for (int i = 0; i < dequeued; i++){
        Chunk* chunk = chunksToBuffer[i];
        chunk->BufferData();
        if (!chunk->canBeDrawn) {
            m_ChunkDrawVector.push_back(chunk);
        }
    }
}

void engine::World::GenerateNewChunks(int64_t remainingFrameTime, int radius, Vec3<int> prevChunkPos, Vec3<int> newChunkPos) {
    m_ChunkDrawVector.erase(std::remove_if(m_ChunkDrawVector.begin(), m_ChunkDrawVector.end(), [newChunkPos, radius](Chunk* chunk) { 
        return 
        chunk->m_Pos.x < newChunkPos.x-radius ||
        chunk->m_Pos.x > newChunkPos.x+radius ||
        chunk->m_Pos.z < newChunkPos.z-radius ||
        chunk->m_Pos.z > newChunkPos.z+radius ;
    }), m_ChunkDrawVector.end());

    Vec3<int> diff = newChunkPos - prevChunkPos;

    if (abs(diff.z) == 1) {
        ChunkGroup* chunkGroup = new ChunkGroup;
        chunkGroup->orientation = diff;
        m_ChunkGroups.push(chunkGroup);

        for (int x = newChunkPos.x - radius; x <= newChunkPos.x + radius; x++){
            for (int y = 0; y < 3; y++){
                // Create new chunks
                Vec3<int> chunkPos(x,y,newChunkPos.z + (radius * diff.z));
                m_MapMutex.lock();
                auto result = m_ChunkMap.try_emplace(chunkPos, chunkPos);
                Chunk* chunk = &(result.first->second);
                m_MapMutex.unlock();
                if (y == 2) { 
                    m_TerrainGenPool.push_task([chunk, chunkGroup, this]{
                        chunk->TerrainGen(m_Noise, gen, distrib);
                        std::lock_guard<std::mutex> guard(chunkGroup->mHolder.mutex);
                        chunkGroup->group.push_back(chunk);
                    });
                } else {
                    m_TerrainGenPool.push_task([chunk, chunkGroup, this]{
                        chunk->TerrainGen(STONE);
                        std::lock_guard<std::mutex> guard(chunkGroup->mHolder.mutex);
                        chunkGroup->group.push_back(chunk);
                    });
                }
            }
            m_MapMutex.lock();
            for (int y = 0; y < 3; y++){
                EraseChunk(Vec3<int>(x, y, (prevChunkPos.z + (radius * -diff.z))));
            }
            m_MapMutex.unlock();
        }
    }

    if (abs(diff.x) == 1) {
        ChunkGroup* chunkGroup = new ChunkGroup;
        chunkGroup->orientation = diff;
        m_ChunkGroups.push(chunkGroup);

        for (int z = newChunkPos.z - radius; z <= newChunkPos.z + radius; z++){
            for (int y = 0; y < 3; y++){
                // Create new chunks
                Vec3<int> chunkPos(newChunkPos.x + (radius * diff.x),y,z);
                auto result = m_ChunkMap.try_emplace(chunkPos, chunkPos);
                Chunk* chunk = &(result.first->second);
                if (y == 2) { 
                    m_TerrainGenPool.push_task([chunk, chunkGroup, this]{
                        chunk->TerrainGen(m_Noise, gen, distrib);
                        std::lock_guard<std::mutex> guard(chunkGroup->mHolder.mutex);
                        chunkGroup->group.push_back(chunk);
                    });
                } else {
                    m_TerrainGenPool.push_task([chunk, chunkGroup, this]{
                        chunk->TerrainGen(STONE);
                        std::lock_guard<std::mutex> guard(chunkGroup->mHolder.mutex);
                        chunkGroup->group.push_back(chunk);
                    });
                }
            }

            m_MapMutex.lock();
            for (int y = 0; y < 3; y++) {
                EraseChunk(Vec3<int>((prevChunkPos.x + (radius * -diff.x)), y, z));
            }
            m_MapMutex.unlock();
        }
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

void engine::World::EraseChunk(Vec3<int> pos) {
    m_ChunkMap.erase(pos);
} 

void engine::World::AddChunkToDrawVector(Chunk* chunk) {
    m_ChunkDrawVector.push_back(chunk);
    chunk->canBeDrawn = true;
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

void engine::World::SetNeighborsEdgeData(Chunk* chunk, const Vec3<int>& chunkStripOrientation) {
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
            }
        }
        leftChunk->needsRemeshing = true;
    }

    Chunk* rightChunk = GetChunk(chunk->m_Pos + Vec3<int>(1, 0, 0));
    if (rightChunk != nullptr) {
        for (int y = 1; y < CS_P_MINUS_ONE; y++){
            for(int z = 1; z < CS_P_MINUS_ONE; z++) {
                rightChunk->SetBlock(chunk->GetBlock(CS, y, z), 0, y, z);
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