/*
Copyright (C) 2023 William Redding - All Rights Reserved
LICENSE: MIT
*/

#include <world/chunk/Constants.hpp>
#include <world/chunk/ChunkStack.hpp>
#include <world/Block.hpp>
#include <util/Log.hpp>
#include <random>
#include <fstream>

ChunkStack::ChunkStack(iVec2 pos) : mPos(pos)
{
    for (int y = 0; y < DEFAULT_CHUNK_STACK_HEIGHT; y++) {
        mChunks.emplace_back(std::make_shared<Chunk>(iVec3{ pos[0], y, pos[1] }));
    }
}

ChunkStack::iterator ChunkStack::begin() {
    return mChunks.begin();
}

ChunkStack::iterator ChunkStack::end() {
    return mChunks.end();
}

ChunkStack::const_iterator ChunkStack::cbegin() const {
    return mChunks.cbegin();
}

ChunkStack::const_iterator ChunkStack::cend() const {
    return mChunks.cend();
}

size_t ChunkStack::size() const {
    return mChunks.size();
}

void ChunkStack::GenerateTerrain(siv::PerlinNoise::seed_type seed, const siv::PerlinNoise& perlin) {
    std::mt19937 gen(seed);
    std::uniform_real_distribution<float> distribution(0.0, 1.0);

    for (std::size_t i = 0; i < mChunks.size(); i++) {
        mChunks.at(i)->AllocateMemory();
    }

    for (int x = 0; x < CS_P; x++) {
        for (int z = 0; z < CS_P; z++) {
            float heightMultiplayer = perlin.octave2D_01((mPos[0] * CS + x) * 0.001f, (mPos[1] * CS + z) * 0.001f, 4, 0.5);
            int height = MIN_WORLD_GEN_HEIGHT + (heightMultiplayer * MAX_MINUS_MIN_WORLD_GEN_HEIGHT);

            if (height < WATER_LEVEL) {
                for (int y = height; y < WATER_LEVEL; y++) {
                    SetBlock(iVec3{ x,y, z }, WATER);
                }
                SetBlock(iVec3{ x,height - 1,z }, DIRT);
            }
			else if (height < WATER_LEVEL + 2) {
				SetBlock(iVec3{x, height-1, z}, SAND);
			}
            else {
                SetBlock(iVec3{ x,height - 1,z }, GRASS);
				float rand = distribution(gen);
				if (rand < 0.01f) {
					if (height <= MAX_WORLD_GEN_HEIGHT - 1) {
						SetBlock(iVec3{ x, height, z }, ROSE);
					}
				}
				else if (rand < 0.02f) {
					if (height <= MAX_WORLD_GEN_HEIGHT -1) {
						SetBlock(iVec3{ x, height, z }, PINK_TULIP);
					}
				}
				else if (rand < 0.2f) {
					if (height <= MAX_WORLD_GEN_HEIGHT - 1) {
						SetBlock(iVec3{ x, height, z }, TALL_GRASS);
					}
				}	
            }
            for (int y = 0; y < height - 4; y++) {
                SetBlock(iVec3{ x, y, z }, STONE);
            }
            for (int y = height - 4; y < height - 1; y++) {
                SetBlock(iVec3{ x, y, z }, DIRT);
            }
            SetBlock(iVec3{ x, 0, z }, BEDROCK);
        }
    }

    for (std::size_t i = 0; i < DEFAULT_CHUNK_STACK_HEIGHT; i++) {
        std::shared_ptr<Chunk> currentChunk = mChunks.at(i);
        std::shared_ptr<Chunk> belowChunk = GetChunk(i - 1);
        std::shared_ptr<Chunk> aboveChunk = GetChunk(i + 1);

        if (belowChunk != nullptr) {
            for (int x = 1; x < CS_P_MINUS_ONE; x++) {
                for (int z = 1; z < CS_P_MINUS_ONE; z++) {
                    currentChunk->SetBlock(iVec3{ x, 0, z }, belowChunk->GetBlock(iVec3{ x, CS, z }));
                }
            }
        }

        if (aboveChunk != nullptr) {
            for (int x = 1; x < CS_P_MINUS_ONE; x++) {
                for (int z = 1; z < CS_P_MINUS_ONE; z++) {
                    currentChunk->SetBlock(iVec3{ x, CS_P_MINUS_ONE, z }, aboveChunk->GetBlock(iVec3{ x, 1, z }));
                }
            }
        }
    }
}

void ChunkStack::Load(const std::string& worldDirectory, siv::PerlinNoise::seed_type seed, const siv::PerlinNoise& perlin) {
	std::ifstream chunkStackFileStream(fmt::format("{}/chunk_stacks/{}.{}.stack", worldDirectory, mPos[0], mPos[1]));
	if (chunkStackFileStream.is_open()) {
		// chunk data found on disk, so we load that
		// read how many chunks in chunk stack
		std::size_t chunkCount;
		chunkStackFileStream.read(reinterpret_cast<char*>(&chunkCount), sizeof(std::size_t));
		
		for (std::size_t i = 0; i < chunkCount; i++) {
			mChunks[i]->AllocateMemory();
			chunkStackFileStream.read(reinterpret_cast<char*>(mChunks[i]->GetBlockDataPointer()), sizeof(BlockID) * CS_P3);
		}
			
	} else {
		// no chunk stack found on disk, generate
		GenerateTerrain(seed, perlin);
		SaveToFile(worldDirectory);
	}

	// Mesh all chunks
	for (auto it = mChunks.begin(); it != mChunks.end(); ++it) {
		std::shared_ptr<Chunk> chunk = (*it);
		chunk->CreateMesh();
	}
	isBeingLoaded = false;
}

void ChunkStack::Unload(const std::string& worldDirectory) {
	SaveToFile(worldDirectory);

	for (auto& chunk : mChunks) {
		chunk->ReleaseMemory();
	}
}

void ChunkStack::SaveToFile(const std::string& worldDirectory) {
	std::string file = fmt::format("{}/chunk_stacks/{}.{}.stack", worldDirectory, mPos[0], mPos[1]);
	std::fstream out(file, std::ios::binary | std::ios::out | std::ios::in);
	std::size_t chunkCount = mChunks.size();

	if (out.is_open()) {
		out.write(reinterpret_cast<char*>(&chunkCount), sizeof(std::size_t));
		for (std::size_t i = 0; i < chunkCount; i++) {
			if (mChunks[i]->needsSaving) {
				mChunks[i]->needsSaving = false;
				out.write(reinterpret_cast<const char*>(mChunks[i]->GetBlockDataPointer()), sizeof(BlockID) * CS_P3);
			} else {
				out.seekp(sizeof(BlockID) * CS_P3, std::ios::cur);
			}
		}
	} else {
		out.clear();
		out.open(file, std::ios::binary | std::ios::out | std::ios::trunc);
		out.write(reinterpret_cast<char*>(&chunkCount), sizeof(std::size_t));
		for (std::size_t i = 0; i < chunkCount; i++) {
			mChunks[i]->needsSaving = false;
			out.write(reinterpret_cast<const char*>(mChunks[i]->GetBlockDataPointer()), sizeof(BlockID) * CS_P3);	
		}
	}

	if (out.fail()) {
		LOG_ERROR("Failed to write chunk stack at {}, {} to disk!", mPos[0], mPos[1]);
	}
}

iVec2 ChunkStack::GetPosition() const
{
    return mPos;
}

void ChunkStack::Draw(const Frustum& frustum, Shader& shader, int* totalChunks, int* chunksDrawn)
{
    for (auto& chunk : mChunks) {
        chunk->Draw(frustum, shader, totalChunks, chunksDrawn);
    }
}

void ChunkStack::DrawWater(const Frustum& frustum, Shader& shader, int* totalChunks, int* chunksDrawn)
{
    for (auto& chunk : mChunks) {
        chunk->DrawWater(frustum, shader, totalChunks, chunksDrawn);
    }
}

void ChunkStack::DrawCustomModel(const Frustum& frustum, Shader& shader, int* totalChunks, int* chunksDrawn)
{
    for (auto& chunk : mChunks) {
        chunk->DrawCustomModel(frustum, shader, totalChunks, chunksDrawn);
    }
}

void ChunkStack::SetBlock(iVec3 pos, BlockID block) {
    mChunks.at(pos[1] / CS)->SetBlock(iVec3{ pos[0], 1 + pos[1] % CS, pos[2] }, block);
}

BlockID ChunkStack::GetBlock(iVec3 pos) const {
    return mChunks.at(pos[1] / CS)->GetBlock(iVec3{ pos[0], 1 + pos[1] % CS, pos[2] });
}

std::shared_ptr<Chunk>ChunkStack::GetChunk(std::size_t y) const {
    if (y >= mChunks.size()) {
        return nullptr;
    }
    else {
        return mChunks.at(y);
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
