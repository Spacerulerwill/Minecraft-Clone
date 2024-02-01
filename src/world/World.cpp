/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include <world/World.hpp>
#include <cmath>
#include <util/Log.hpp>
#include <fmt/format.h>
#include <util/IO.hpp>

iVec3 GetWorldBlockPosFromGlobalPos(Vec3 globalPosition)
{
    iVec3 result;
    for (int i = 0; i < 3; i++) {
        result[i] = static_cast<iVec3::value_type>(std::floor(globalPosition[i]));
    }
    return result;
}

iVec3 GetChunkPosFromGlobalBlockPos(iVec3 globalBlockPos)
{
    iVec3 result;
    for (int i = 0; i < 3; i++) {
        if (globalBlockPos[i] < 0) {
            result[i] = ((globalBlockPos[i] + 1) / CS) - 1;
        }
        else {
            result[i] = globalBlockPos[i] / CS;
        }
    }
    return result;
}

iVec3 GetChunkBlockPosFromGlobalBlockPos(iVec3 globalBlockPos)
{
    iVec3 result;
    for (int i = 0; i < 3; i++) {
        if (globalBlockPos[i] >= 0) {
            result[i] = 1 + (globalBlockPos[i] % CS);
        }
        else {
            result[i] = CS_P_MINUS_ONE - (1 + (abs(globalBlockPos[i]) - 1) % CS);
        }
    }
    return result;
}


World::World(std::string worldDirectory) : worldDirectory(worldDirectory)
{
	// Load world data
	WorldSave worldSave;
	ReadStructFromDisk(fmt::format("{}/world.data", worldDirectory), worldSave);
	seed = worldSave.seed;
    mPerlin.reseed(seed);
	
	// Load player data
	PlayerSave playerSave;
	ReadStructFromDisk(fmt::format("{}/player.data", worldDirectory), playerSave);
	player.camera.position = playerSave.pos;
	player.camera.pitch = playerSave.pitch;
	player.camera.yaw = playerSave.yaw;
	
	// Load texture atlases
    LOG_INFO("Loading texture atlases...");
    for (std::size_t i = 0; i < MAX_ANIMATION_FRAMES; i++) {
        mTextureAtlases[i] = TexArray2D(fmt::format("textures/atlases/atlas{}.png", i), TEXTURE_SIZE, GL_TEXTURE0);
    }
	
	// Load spawn chunks
	Vec3 playerPos = player.camera.position;
    iVec2 playerChunkPos{
        static_cast<int>(floor(playerPos[0]) / CS),
        static_cast<int>(floor(playerPos[2]) / CS)
    };

	LOG_INFO("Loading spawn chunks for {} (Seed: {})", worldDirectory, seed);
	for (int x = -mRenderDistance; x <= mRenderDistance; x++) {
        for (int z = -mRenderDistance; z <= mRenderDistance; z++) {
			iVec2 pos = playerChunkPos + iVec2{ x,z };
			auto emplace = mChunkStacks.emplace(pos, pos);
			ChunkStack& chunkStack = emplace.first->second;
			chunkStack.isBeingLoaded = true;
			mLoadPool.push_task([&chunkStack, this]{
				chunkStack.Load(this->worldDirectory, this->seed, this->mPerlin);
			});
		}
	}
	mLoadPool.wait_for_tasks();

	for (int x = -mRenderDistance; x <= mRenderDistance; x++) {
        for (int z = -mRenderDistance; z <= mRenderDistance; z++) {
			auto find = mChunkStacks.find(playerChunkPos + iVec2{x,z});
			if (find == mChunkStacks.end()) {
				// this should never happen
				LOG_ERROR("Failed to buffer spawn chunk ({}, {})", x, z);
			} else {
				ChunkStack& stack = find->second;
				for (auto it = stack.begin(); it != stack.end(); ++it) {
					std::shared_ptr<Chunk> chunk = *it;
					chunk->BufferData();
				}
			}
		}
	}
}

World::~World() {
	// Save to disk
	WorldSave worldSave {
		.seed = seed
	};
	WriteStructToDisk(fmt::format("{}/world.data", worldDirectory), worldSave);
	PlayerSave playerSave {
		.pos = player.camera.position,
		.pitch = player.camera.pitch,
		.yaw = player.camera.yaw
	};
	WriteStructToDisk(fmt::format("{}/player.data", worldDirectory), playerSave);
	
	// Unload all remaining chunks 
	for (auto it = mChunkStacks.begin(); it != mChunkStacks.end(); ++it) {
		ChunkStack& chunkStack = it->second;
		mUnloadPool.push_task([&chunkStack, this] {
				chunkStack.Unload(this->worldDirectory);
				});
	}
	mUnloadPool.wait_for_tasks();
}

void World::Draw(const Frustum& frustum, int* totalChunks, int* chunksDrawn)
{
    Mat4 perspective = player.camera.perspectiveMatrix;
    Mat4 view = player.camera.GetViewMatrix();
	float time = static_cast<float>(glfwGetTime());
	float ambientTerrainLight = (((0.5 * cos(time) + 0.5) * 0.8) + 0.5);
	float skyboxBrightness = 0.5 * cos(time) + 0.5;

    glDepthFunc(GL_LEQUAL);
    mSkybox.Draw(perspective, translationRemoved(view), skyboxBrightness);
    glDepthFunc(GL_LESS);

    chunkShader.Bind();
    chunkShader.SetMat4("projection", perspective);
    chunkShader.SetMat4("view", view);
    chunkShader.SetVec3("grass_color", mGrassColor);
	chunkShader.SetFloat("ambient", ambientTerrainLight);

    glActiveTexture(GL_TEXTURE0);
    mTextureAtlases[currentAtlasID].Bind();
    chunkShader.SetInt("tex_array", 0);

    glActiveTexture(GL_TEXTURE1);
    mGrassSideMask.Bind();
    chunkShader.SetInt("grass_mask", 1);

    for (auto& [pos, stack] : mChunkStacks) {
        stack.Draw(frustum, chunkShader, totalChunks, chunksDrawn);
    }

    waterShader.Bind();
    waterShader.SetMat4("projection", perspective);
    waterShader.SetMat4("view", view);
    waterShader.SetVec3("color", mWaterColor);
	waterShader.SetFloat("ambient", ambientTerrainLight);
    glActiveTexture(GL_TEXTURE0);
    waterShader.SetInt("tex_array", 0);
    glEnable(GL_BLEND);
    for (auto& [pos, stack] : mChunkStacks) {
        stack.DrawWater(frustum, waterShader, totalChunks, chunksDrawn);
    }
    glDisable(GL_BLEND);

    // Draw custom models
    glDisable(GL_CULL_FACE);
    customModelShader.Bind();
    customModelShader.SetMat4("projection", perspective);
    customModelShader.SetMat4("view", view);
    glActiveTexture(GL_TEXTURE0);
    customModelShader.SetInt("tex_array", 0);
    customModelShader.SetVec3("foliage_color", mFoliageColor);
	customModelShader.SetFloat("ambient", ambientTerrainLight);
    for (auto& [pos, stack] : mChunkStacks) {
        stack.DrawCustomModel(frustum, customModelShader, totalChunks, chunksDrawn);
    }
    glEnable(GL_CULL_FACE);
}

void World::GenerateChunks()
{
	// Detect what chunk the player is in
    Vec3 playerPos = player.camera.position;
    iVec2 playerChunkPos{
        static_cast<int>(floor(playerPos[0]) / CS),
        static_cast<int>(floor(playerPos[2]) / CS)
    };
	
	// Unload chunks out of distance
	std::vector<std::unordered_map<iVec2, ChunkStack>::iterator> iteratorsToRemove;
    for (auto it = mChunkStacks.begin(); it != mChunkStacks.end();) {
        ChunkStack& stack = it->second;

        iVec2 stackPos = stack.GetPosition();
        if ((stackPos[0] < playerChunkPos[0] - mRenderDistance ||
            stackPos[0] > playerChunkPos[0] + mRenderDistance ||
            stackPos[1] < playerChunkPos[1] - mRenderDistance ||
            stackPos[1] > playerChunkPos[1] + mRenderDistance) &&
			!stack.isBeingLoaded) {
			
			mUnloadPool.push_task([&stack, this] {
				stack.Unload(this->worldDirectory);					
			});
			iteratorsToRemove.push_back(it);
		} 
		++it;
    }

	mUnloadPool.wait_for_tasks();
	for (auto it : iteratorsToRemove) {
		mChunkStacks.erase(it);
	}
	
	// Load and generate new chunks
    std::size_t chunksBuffered = 0;
    std::size_t chunksCreated = 0;
    for (int x = -mRenderDistance; x <= mRenderDistance; x++) {
        for (int z = -mRenderDistance; z <= mRenderDistance; z++) {
			if (chunksBuffered < mBufferPerFrame && chunksCreated < mBufferPerFrame) {
				iVec2 pos = playerChunkPos + iVec2{ x,z };
				auto find = mChunkStacks.find(pos);
				if (find == mChunkStacks.end()) {
					if (chunksCreated < mBufferPerFrame) {
						auto emplace = mChunkStacks.emplace(pos, pos);
						ChunkStack& chunkStack = emplace.first->second;
						chunkStack.isBeingLoaded = true;
						mLoadPool.push_task([&chunkStack, this]{
							chunkStack.Load(this->worldDirectory, this->seed, this->mPerlin);
						});
						chunksCreated++;
					}
				}
				else {
					ChunkStack& stack = find->second;
					for (auto it = stack.begin(); it != stack.end(); ++it) {
						std::shared_ptr<Chunk> chunk = *it;
						if (chunksBuffered < mBufferPerFrame) {
							if (chunk->needsBuffering) {
								chunk->BufferData();
								chunksBuffered++;
							}
						}
						else {
							return;
						}
					}
				}
			} else {
				return;
			}
        }
    }
}

void World::TrySwitchToNextTextureAtlas()
{
    double currentTime = glfwGetTime();
    if (currentTime - lastAtlasSwitch > 0.2) {
        currentAtlasID = (currentAtlasID + 1) % MAX_ANIMATION_FRAMES;
        glActiveTexture(GL_TEXTURE0);
        mTextureAtlases[currentAtlasID].Bind();
        lastAtlasSwitch = currentTime;
    }
}

const ChunkStack* World::GetChunkStack(iVec2 pos) const
{
    auto find = mChunkStacks.find(pos);
    if (find == mChunkStacks.end()) {
        return nullptr;
    }
    return &find->second;
}

std::shared_ptr<Chunk> World::GetChunk(iVec3 pos) const
{
    const ChunkStack* chunkStack = GetChunkStack(iVec2{ pos[0], pos[2] });
    if (chunkStack == nullptr) {
        return nullptr;
    }
    return chunkStack->GetChunk(pos[1]);
}

BlockID World::GetBlock(iVec3 pos) const
{
    iVec3 chunkPos = GetChunkPosFromGlobalBlockPos(pos);
    std::shared_ptr<Chunk> chunk = GetChunk(chunkPos);
    if (chunk != nullptr && chunk->loaded) {
        iVec3 blockPos = GetChunkBlockPosFromGlobalBlockPos(pos);
        return chunk->GetBlock(blockPos);
    }
    return AIR;
}

void World::SetBlock(iVec3 pos, BlockID block)
{
    iVec3 chunkPos = GetChunkPosFromGlobalBlockPos(pos);
    std::shared_ptr<Chunk> chunk = GetChunk(chunkPos);

    if (chunk != nullptr && chunk->loaded) {
        iVec3 blockPos = GetChunkBlockPosFromGlobalBlockPos(pos);
        chunk->SetBlock(blockPos, block);
    }
}

void World::SetBlockAndRemesh(iVec3 pos, BlockID block)
{
    iVec3 chunkPos = GetChunkPosFromGlobalBlockPos(pos);
    std::shared_ptr<Chunk> chunk = GetChunk(chunkPos);

    if (chunk != nullptr && chunk->loaded) {
        iVec3 blockPos = GetChunkBlockPosFromGlobalBlockPos(pos);
        chunk->SetBlock(blockPos, block);
        chunk->CreateMesh();
        chunk->BufferData();
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
