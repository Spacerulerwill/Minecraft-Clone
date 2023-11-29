/*
Copyright (C) 2023 William Redding - All Rights Reserved
LICENSE: MIT
*/

#include <fmt/format.h>
#include <GLFW/glfw3.h>
#include <util/Exceptions.hpp>
#include <util/IO.hpp>
#include <util/Log.hpp>
#include <util/Log.hpp>
#include <world/Player.hpp>
#include <world/World.hpp>

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

engine::ChunkRegion* engine::World::GetRegion(Vec2<int> pos) const {
    auto find = m_ChunkRegions.find(pos);
    if (find == m_ChunkRegions.end()) {
        return nullptr;
    }
    else {
        return (*find).second;
    }

}

void engine::World::Draw(Shader& chunkShader, Shader& waterShader, Shader& customModelShader) {
    Mat4<float> perspective = m_Player.GetCamera().GetPerspectiveMatrix();
    Mat4<float> view = m_Player.GetCamera().GetViewMatrix();
    const VoxelRaycastResult& raycastResult = m_Player.GetBlockRaycastResult();

    chunkShader.Bind();
    chunkShader.SetMat4("projection", perspective);
    chunkShader.SetMat4("view", view);
    chunkShader.SetInt("tex_array", 0);
    chunkShader.SetInt("grass_mask", 2);
    for (auto& it : m_ChunkRegions) {
        it.second->DrawOpaque(chunkShader);
    }

    glDisable(GL_CULL_FACE);
    customModelShader.Bind();
    customModelShader.SetMat4("projection", perspective);
    customModelShader.SetMat4("view", view);
    customModelShader.SetInt("tex_array", 0);
    for (auto& it : m_ChunkRegions) {
        it.second->DrawCustomModel(customModelShader);
    }
    glEnable(GL_CULL_FACE);

    glDepthFunc(GL_LEQUAL);
    m_Skybox.Draw(perspective, translationRemoved(view));
    glDepthFunc(GL_LESS);

    glEnable(GL_BLEND);
    waterShader.Bind();
    waterShader.SetMat4("projection", perspective);
    waterShader.SetMat4("view", view);
    waterShader.SetInt("tex_array", 0);
    for (auto& it : m_ChunkRegions) {
        it.second->DrawWater(waterShader);
    }
    glDisable(GL_BLEND);
}

void engine::World::GenerateChunks() {
    Vec3<float> playerPos = m_Player.GetCamera().GetPosition();

    Vec2<int> chunkRegionPos = Vec2<int>(static_cast<int>(playerPos.x) / CHUNK_REGION_BLOCK_SIZE, static_cast<int>(playerPos.z) / CHUNK_REGION_BLOCK_SIZE);
    if (playerPos.x < 0)
        chunkRegionPos.x--;
    if (playerPos.z < 0)
        chunkRegionPos.y--;

    auto find = m_ChunkRegions.find(chunkRegionPos);
    if (find == m_ChunkRegions.end()) {
        std::ifstream rf(fmt::format("worlds/{}/regions/{}.{}.region", m_WorldName, chunkRegionPos.x, chunkRegionPos.y), std::ios::in | std::ios::binary);
        auto result = m_ChunkRegions.insert(std::make_pair(chunkRegionPos, new ChunkRegion(chunkRegionPos)));
        m_CurrentRegion = (*result.first).second;
        if (rf) {
            // Load data from file into region
            m_CurrentRegion->startedTerrainGeneration = true;
            m_CurrentRegion->startedChunkMerging = true;
            for (int x = 0; x < CHUNK_REGION_SIZE; x++) {
                for (int z = 0; z < CHUNK_REGION_SIZE; z++) {
                    ChunkStack* stack = m_CurrentRegion->GetChunkStack(Vec2<int>(x, z));
                    size_t size;
                    rf.read(reinterpret_cast<char*>(&size), sizeof(size_t));
                    for (size_t i = 0; i < size; i++) {
                        Chunk* chunk = stack->GetChunk(i);
                        chunk->Allocate();
                        rf.read(reinterpret_cast<char*>(chunk->GetBlockDataPointer()), CS_P3 * sizeof(BlockInt));
                    }
                }
            }
        }
        rf.close();
    }
    else {
        m_CurrentRegion = (*find).second;
    }

    m_CurrentRegion->GenerateChunks(m_WorldName, m_Noise, gen, distrib);
    m_CurrentRegion->BufferChunksPerFrame();

}

engine::World::~World() {
    // Save player position and view direction
    PlayerSave playerSave{
        .position = m_Player.GetCamera().GetPosition(),
        .pitch = m_Player.GetCamera().GetPitch(),
        .yaw = m_Player.GetCamera().GetYaw()
    };
    WriteStructToDisk(fmt::format("worlds/{}/player.dat", m_WorldName), playerSave);

    // Delete chunk and unload chunk regions
    for (const auto& [pos, region] : m_ChunkRegions) {
        m_RegionUnloadPool.push_task([region, this] {
            region->UnloadToFile(m_WorldName);
            });
    }
    m_RegionUnloadPool.wait_for_tasks();
    for (const auto& [pos, region] : m_ChunkRegions) {
        delete region;
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