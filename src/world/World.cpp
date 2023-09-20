/*
Copyright (C) 2023 William Redding - All Rights Reserved
LICENSE: MIT
*/

#include <world/World.hpp>
#include <GLFW/glfw3.h>
#include <atomic>

engine::World::World(siv::PerlinNoise::seed_type seed) {
    m_Noise.reseed(seed);
}

void engine::World::CreateChunks(int chunkX, int chunkZ, int radius, int bufferPerFrame) {
	int chunksBuffered = 0;
	int chunksRemeshed = 0;
    m_ChunkDrawVector.clear();

    for (int iz = -radius; iz < radius; iz++) {
		for (int y = 0; y < 4; y++) {
			int dx = static_cast<int>(round(sqrt(radius * radius - iz * iz)));
			for (int ix = -dx; ix < dx; ix++) {
				Vec3 vec = Vec3(chunkX + ix, y, chunkZ + iz);
				Chunk* chunk = nullptr;
				if (m_ChunkMap.find(vec) == m_ChunkMap.end()) {
					chunk = new Chunk(chunkX + ix, y, chunkZ + iz);
					m_ChunkMap[vec] = chunk;
				}
				else {
					chunk = m_ChunkMap[vec];
					
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

					if (y == 3) {
						m_ThreadPool.push_task([chunk, this] {
							chunk->TerrainGen(this->m_Noise);
							chunk->CreateMesh();
                            chunk->firstBufferTime = static_cast<float>(glfwGetTime());
						});
					}
					else {
						m_ThreadPool.push_task([chunk, this] {
							chunk->TerrainGen(STONE);
							chunk->CreateMesh();
                            chunk->firstBufferTime = static_cast<float>(glfwGetTime());
						});
					}
				}
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