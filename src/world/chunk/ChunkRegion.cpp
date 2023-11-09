/*
Copyright (C) 2023 William Redding - All Rights Reserved
LICENSE: MIT
*/

#include <world/chunk/ChunkRegion.hpp>
#include <util/Log.hpp>

engine::ChunkRegion::ChunkRegion(Vec2<int> pos): m_Pos(pos) {
    m_ChunkStacks.reserve(CHUNK_REGION_SIZE_SQUARED);
    for (int x = 0; x < CHUNK_REGION_SIZE; x++) {
        for (int z = 0; z < CHUNK_REGION_SIZE; z++) {
            m_ChunkStacks.emplace_back(Vec2<int>((pos.x * CHUNK_REGION_SIZE) + x,(pos.y * CHUNK_REGION_SIZE) + z));
        }
    }
}

engine::Vec2<int> engine::ChunkRegion::GetPos() const {
    return m_Pos;
}

void engine::ChunkRegion::DrawOpaque(Shader& opaqueShader){
    for (ChunkStack& chunkStack : m_ChunkStacks) {
        chunkStack.DrawOpaque(opaqueShader);
    }
}

void engine::ChunkRegion::DrawWater(Shader& waterShader) {
    for (ChunkStack& chunkStack : m_ChunkStacks) {
        chunkStack.DrawWater(waterShader);
    }
}

void engine::ChunkRegion::DrawCustomModel(Shader& customModelShader) {
    for (ChunkStack& chunkStack : m_ChunkStacks) {
        chunkStack.DrawCustomModel(customModelShader);
    }
}

engine::Chunk* engine::ChunkRegion::GetChunk(Vec3<int> pos) {
    if (pos.x < 0 || pos.x >= CHUNK_REGION_SIZE || pos.z < 0 || pos.z >= CHUNK_REGION_SIZE) {
        return nullptr;
    }
    else {
        int chunkStackIndex = ChunkStackIndex(pos.x, pos.z);
        return m_ChunkStacks.at(chunkStackIndex).GetChunk(pos.y);
    }
}

void engine::ChunkRegion::GenerateChunks(const siv::PerlinNoise& perlin, std::mt19937& gen, std::uniform_int_distribution<>& distrib) {
    if (!startedTerrainGeneration) {
        for (ChunkStack& chunkStack : m_ChunkStacks) {
            m_TerrainGenPool.push_task([&chunkStack, &perlin, &gen, &distrib]{
                chunkStack.GenerateTerrain(perlin, gen, distrib);
            });
        }
        startedTerrainGeneration = true;
        return;
    } else if (m_TerrainGenPool.get_tasks_total() != 0) {
        return;
    }

    if (!startedChunkMerging) {
        for (int x = 0; x < CHUNK_REGION_SIZE; x++) {
            for (int z = 0; z < CHUNK_REGION_SIZE; z++) {
                ChunkStack* stack = &m_ChunkStacks.at(ChunkStackIndex(x, z));
                for (int y = 0; y < stack->size(); y++) {
                    Chunk* chunk = stack->GetChunk(y);
                    m_ChunkMergePool.push_task([chunk, this]{
                        chunk->CopyNeighborData(this);                    
                    });
                }
            }
        }
        startedChunkMerging = true;
        return;
    } else if (m_ChunkMergePool.get_tasks_total() != 0){
        return;
    }

    if (!startedChunkMeshing) {
        for (ChunkStack& chunkStack : m_ChunkStacks) {
            for (auto it = chunkStack.begin(); it != chunkStack.end(); ++it) {
                m_ChunkMeshPool.push_task([it, this]{
                    (*it).CreateMesh();
                    m_ChunkBufferQueue.enqueue(&(*it));
                });
            }
        }
        startedChunkMeshing = true;
    }
}

void engine::ChunkRegion::BufferChunksPerFrame() {
    std::size_t dequeued = m_ChunkBufferQueue.try_dequeue_bulk(m_ChunkBufferQueueDequeueResult, MAX_BUFFER_PER_FRAME);
    for (size_t i = 0; i < dequeued; i++){
        m_ChunkBufferQueueDequeueResult[i]->BufferData();
    }
}

/*
MIT Licese

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