/*
Copyright (C) 2023 William Redding - All Rights Reserved
LICENSE: MIT
*/

#include <world/World.hpp>

engine::World::World(siv::PerlinNoise::seed_type seed) {
    m_Noise.reseed(seed);
}

void engine::World::CreateChunks(int chunkX, int chunkZ, int radius) {

    m_ChunkDrawVector.clear();
    for (int iz = - radius; iz < radius; iz++) {
        int dx = static_cast<int>(round(sqrt(radius * radius - iz * iz)));
        for (int ix = - dx; ix <  dx; ix++) {
            Vec3 vec = Vec3(chunkX + ix, 0, chunkZ + iz);
            if (m_ChunkMap.find(vec) == m_ChunkMap.end()) {
                Chunk* chunk = new Chunk(chunkX + ix, 0, chunkZ + iz);
                chunk->TerrainGen(m_Noise);
                chunk->CreateMesh();
                chunk->BufferData();
                m_ChunkMap[vec] = chunk;
            } else {
                m_ChunkDrawVector.push_back(m_ChunkMap[vec]);
            }
        }
    }
}

void engine::World::DrawOpaque(Shader& chunkShader) {
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

engine::World::~World() {
    for (const auto p : m_ChunkMap){
        delete p.second;
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