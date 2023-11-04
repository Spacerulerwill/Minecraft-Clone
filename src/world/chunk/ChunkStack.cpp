/*
Copyright (C) 2023 William Redding - All Rights Reserved
LICENSE: MIT
*/

#include <world/chunk/ChunkStack.hpp>
#include <util/Constants.hpp>


engine::ChunkStack::ChunkStack(Vec2<int> pos): m_Pos(pos) {
    for (int y = 0; y < DEFAULT_CHUNK_STACK_HEIGHT; y++) {
        m_Chunks.emplace_back(Vec3<int>(pos.x, y, pos.y));
    }
}

engine::Vec2<int> engine::ChunkStack::GetPos() const {
    return m_Pos;
}

void engine::ChunkStack::DrawOpaque(Shader& opaqueShader) {
    for (Chunk& chunk : m_Chunks) {
        chunk.DrawOpaque(opaqueShader);
    }
}

void engine::ChunkStack::DrawWater(Shader& waterShader) {
    for (Chunk& chunk : m_Chunks) {
        chunk.DrawWater(waterShader);
    }
}

void engine::ChunkStack::DrawCustomModel(Shader& customModelShader) {
    for (Chunk& chunk : m_Chunks) {
        chunk.DrawCustomModelBlocks(customModelShader);
    }
}

void engine::ChunkStack::GenerateTerrain(const siv::PerlinNoise& perlin, std::mt19937& gen, std::uniform_int_distribution<>& distrib) {
    for (int x = 1; x < CS_P_MINUS_ONE; x++) {
        for (int z = 1; z < CS_P_MINUS_ONE; z++) {
            float heightMultiplayer = perlin.octave2D_01((m_Pos.x * CS + x) * 0.0025 , (m_Pos.y * CS + z) * 0.0025, 4, 0.5);
            int height = MIN_WORLD_GEN_HEIGHT + (heightMultiplayer * MAX_MINUS_MIN_WORLD_GEN_HEIGHT);

            SetBlockVerticalColumn(STONE, Vec3<int>(x,0,z), height-4);
            SetBlockVerticalColumn(DIRT, Vec3<int>(x,height-4,z), 3);
            SetBlock(GRASS, x, height-1, z);
            if (height < WATER_LEVEL) {
                SetBlockVerticalColumn(WATER, Vec3<int>(x, height, z), WATER_LEVEL - height);
            }
            SetBlock(BEDROCK, x, 0, z);
        }
    }
}

void engine::ChunkStack::MeshAndBufferChunks() {
    for (Chunk& chunk: m_Chunks) {
        chunk.CreateMesh();
        chunk.BufferData();
    }
}

void engine::ChunkStack::SetBlockVerticalColumn(BlockInt block, Vec3<int> start, int length) {
    int startChunkY = start.y / CS;
    int endChunkY = (start.y + length - 1) / CS;
    
    if (startChunkY == endChunkY) {
        m_Chunks.at(startChunkY).SetBlockVerticalColumn(block, Vec3<int>(start.x, start.y - (startChunkY * CS) + 1, start.z), length);
        return;
    }

    m_Chunks.at(startChunkY).SetBlockVerticalColumn(block, Vec3<int>(start.x, (start.y - (startChunkY * CS) + 1), start.z), CS - (start.y - (startChunkY * CS)));
    for (int i = startChunkY + 1; i <= endChunkY -1; i++) {
        m_Chunks.at(i).SetBlockVerticalColumn(block, Vec3<int>(start.x, 1, start.z), CS);
    }
    m_Chunks.at(endChunkY).SetBlockVerticalColumn(block, Vec3<int>(start.x, 1, start.z), length - ((endChunkY - startChunkY - 1)* CS)- (CS - (start.y - (startChunkY * CS))));
}

void engine::ChunkStack::SetBlock(BlockInt block, int x, int y, int z) {
    m_Chunks[y / CS].SetBlock(block, x, 1 + y % CS, z);
}

engine::BlockInt engine::ChunkStack::GetBlock(int x, int y, int z) const {
    return m_Chunks[y / CS].GetBlock(x, 1 + y % CS, z);
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