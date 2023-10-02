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

void engine::World::CreateChunks(int chunkX, int chunkZ, int radius, int bufferPerFrame) {
    int chunksBuffered = 0;
	int chunksRemeshed = 0;
    m_ChunkDrawVector.clear();

    for (auto it = m_ChunkMap.begin(); it != m_ChunkMap.end();) {
        Chunk* chunk = &((*it).second);
        if (
            chunk->pos.x < chunkX - radius ||
            chunk->pos.x > chunkX + radius - 1 ||
            chunk->pos.z < chunkZ - radius ||
            chunk->pos.z > chunkZ + radius - 1
        ) {
            if (chunk->needsUnloading) {
                chunk->UnloadToFile(m_WorldName);
            }
            m_ChunkMap.erase(it++);
        } else {
            ++it;
        }
    }

    for (int iz = -radius; iz < radius; iz++) {
        for (int ix = -radius; ix < radius; ix++) {
            
            Vec2 vec = Vec2(chunkX + ix, chunkZ + iz);
            Chunk* chunk = nullptr;

            auto find = m_ChunkMap.find(vec);

            if (find == m_ChunkMap.end()) {
                auto result = m_ChunkMap.try_emplace(vec, chunkX + ix, 0, chunkZ + iz);
                chunk = &(result.first->second);
            }
            else {
                chunk = &(find->second);

                if (chunk->needsBuffering) {
                    if (chunksBuffered < bufferPerFrame) {
                        chunk->BufferData();
                        chunksBuffered++;
                    }
                }
                else if (!chunk->needsRemeshing) {
                    m_ChunkDrawVector.push_back(chunk);
                }
            }

            if (chunksRemeshed < bufferPerFrame && chunk->needsRemeshing) {
                chunksRemeshed++;

                m_ThreadPool.push_task([chunk, this] {
                    std::ifstream rf(fmt::format("worlds/{}/chunks/{}.{}.{}.chunk", m_WorldName, chunk->pos.x, chunk->pos.y, chunk->pos.z), std::ios::in | std::ios::binary);
                    if (!rf) {
                        chunk->TerrainGen(this->m_Noise, this->gen, this->distrib);
                    }else {
                        rf.read((char *)&chunk->m_Voxels[0], CS_P3 * sizeof(BlockInt));
                        rf.close();
                    }
                    chunk->CreateMesh();
                    chunk->firstBufferTime = static_cast<float>(glfwGetTime());
                });
            }
        }
    }
    m_ThreadPool.wait_for_tasks();
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

engine::Chunk* engine::World::GetChunk(int chunkX, int chunkY) {
    auto find = m_ChunkMap.find(Vec2(chunkX, chunkY));

    if (find == m_ChunkMap.end()) {
        return nullptr;
    } else {
        return &(find->second);
    }
}

engine::World::~World() {

    // Unload all remaining chunks
    for (auto& [key, chunk]: m_ChunkMap) {
        if (chunk.needsUnloading) {
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