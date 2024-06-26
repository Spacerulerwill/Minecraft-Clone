/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include <world/World.hpp>
#include <cmath>
#include <util/Log.hpp>
#include <fmt/format.h>
#include <util/IO.hpp>
#include <chrono>
#include <glm/gtc/matrix_transform.hpp>

glm::ivec3 GetWorldBlockPosFromGlobalPos(glm::vec3 globalPosition)
{
    return glm::ivec3(
        static_cast<int>(std::floor(globalPosition.x)),
        static_cast<int>(std::floor(globalPosition.y)),
        static_cast<int>(std::floor(globalPosition.z))
     );
}

glm::ivec3 GetChunkPosFromGlobalBlockPos(glm::ivec3 globalBlockPos)
{
    return glm::ivec3(
        globalBlockPos.x < 0 ? ((globalBlockPos.x + 1) / Chunk::SIZE) - 1 : globalBlockPos.x / Chunk::SIZE,
        globalBlockPos.y < 0 ? ((globalBlockPos.y + 1) / Chunk::SIZE) - 1 : globalBlockPos.y / Chunk::SIZE,
        globalBlockPos.z < 0 ? ((globalBlockPos.z + 1) / Chunk::SIZE) - 1 : globalBlockPos.z / Chunk::SIZE
    );
}

glm::ivec3 GetChunkBlockPosFromGlobalBlockPos(glm::ivec3 globalBlockPos)
{
    return glm::ivec3(
        globalBlockPos.x < 0 ? Chunk::SIZE_PADDED_SUB_1 - (1 + (abs(globalBlockPos.x) - 1) % Chunk::SIZE) : 1 + (globalBlockPos.x % Chunk::SIZE),
        globalBlockPos.y < 0 ? Chunk::SIZE_PADDED_SUB_1 - (1 + (abs(globalBlockPos.y) - 1) % Chunk::SIZE) : 1 + (globalBlockPos.y % Chunk::SIZE),
        globalBlockPos.z < 0 ? Chunk::SIZE_PADDED_SUB_1 - (1 + (abs(globalBlockPos.z) - 1) % Chunk::SIZE) : 1 + (globalBlockPos.z % Chunk::SIZE)
    );
}


World::World(std::string worldDirectory) : mWorldDirectory(worldDirectory)
{
    // Load world data
    WorldSave worldSave;
    if (!ReadStructFromDisk(fmt::format("{}/world.data", worldDirectory), worldSave)) {
        throw WorldCorruptionException("Could not read world data from disk. World may be corrupted!");
    }

    mSeed = worldSave.seed;
    mPerlin.reseed(mSeed);
    mWorldStartTime = worldSave.elapsedTime;
    mCurrentTime = worldSave.elapsedTime;
    
    // Load player data
    PlayerSave playerSave;
    if (!ReadStructFromDisk(fmt::format("{}/player.data", worldDirectory), playerSave)) {
        throw WorldCorruptionException("Could not read player data from disk. World may be corrupted!");
    }
    mPlayer.camera.position = playerSave.pos;
    mPlayer.camera.pitch = playerSave.pitch;
    mPlayer.camera.yaw = playerSave.yaw;
    
    // Load texture atlases
    LOG_INFO("Loading texture atlases...");
    for (std::size_t i = 0; i < MAX_ANIMATION_FRAMES; i++) {
        mTextureAtlases[i] = TexArray2D(fmt::format("textures/atlases/atlas{}.png", i), TEXTURE_SIZE, GL_TEXTURE0);
    }

    // Load spawn chunks
    LOG_INFO("Loading spawn chunks...");
    glm::vec3 playerPos = mPlayer.camera.position;
    glm::ivec2 playerChunkPos{
        static_cast<int>(floor(playerPos.x) / Chunk::SIZE),
        static_cast<int>(floor(playerPos.z) / Chunk::SIZE)
    };

    // Load all chunks
    int totalRenderDistance = mChunkLoadDistance + mChunkPartialLoadDistance;
    for (int x = -totalRenderDistance; x <= totalRenderDistance; x++) {
        for (int z = -totalRenderDistance; z <= totalRenderDistance; z++) {
            int radius = static_cast<int>(std::round(sqrtf(x * x + z * z)));
            glm::ivec2 pos = playerChunkPos + glm::ivec2(x,z);
            if (radius < mChunkLoadDistance) {
                auto emplace = mChunkStacks.emplace(pos, pos);
                ChunkStack& chunkStack = emplace.first->second;
                chunkStack.state = ChunkStackState::LOADED;
                mTaskPool.push_task([this, worldDirectory, &chunkStack]() {
                    chunkStack.FullyLoad(worldDirectory, mSeed, mPerlin);
                    });
            }
            else if (radius <= totalRenderDistance) {
                auto emplace = mChunkStacks.emplace(pos, pos);
                ChunkStack& chunkStack = emplace.first->second;
                chunkStack.state = ChunkStackState::PARTIALLY_LOADED;
                mTaskPool.push_task([this, worldDirectory, &chunkStack]() {
                    chunkStack.PartiallyLoad(worldDirectory, mSeed, mPerlin);
                    });
            }
        }
    }
    mTaskPool.wait_for_tasks();

    // Buffer all chunks
    for (auto& [pos, stack] : mChunkStacks) {
        for (auto it = stack.begin(); it != stack.end(); ++it) {
            (*it)->BufferData();
        }
    }
    mWorldLoadedTime = glfwGetTime();
}

World::~World() {
    mTaskPool.purge();
    mTaskPool.wait_for_tasks();

    // Save to disk
    if (!WriteStructToDisk(fmt::format("{}/world.data", mWorldDirectory), WorldSave{
        .seed = mSeed,
        .elapsedTime = mCurrentTime
        })) {
        LOG_ERROR("Failed to write save world data");
    }

    if (!WriteStructToDisk(fmt::format("{}/player.data", mWorldDirectory), PlayerSave {
        .pos = mPlayer.camera.position,
        .pitch = mPlayer.camera.pitch,
        .yaw = mPlayer.camera.yaw
        })) {
        LOG_ERROR("Failed to write player data");
    };

    // Unload all remaining chunks 
    for (auto& [pos, stack] : mChunkStacks) {
        mUnloadPool.push_task([&stack, this] {
            stack.Unload(mWorldDirectory);
            });

    }
    mUnloadPool.wait_for_tasks();
}

void World::Draw(const Frustum& frustum, int* totalChunks, int* chunksDrawn)
{
    double glfwTime = glfwGetTime();
    mCurrentTime = mWorldStartTime + glfwTime - mWorldLoadedTime;
    double day = mCurrentTime / World::DAY_DURATION;
    double currentDay;
    double currentDayProgress = std::modf(day, &currentDay);

    glm::mat4 perspective = mPlayer.camera.perspectiveMatrix;
    glm::mat4 view = mPlayer.camera.GetViewMatrix();
    float ambientTerrainLight = 1.0f;

    if (currentDayProgress < 0.5) {
        ambientTerrainLight = 0.7f;
    }
    else if (currentDayProgress < 0.6f ) {
        ambientTerrainLight = 0.2f + (1.0f - ((static_cast<float>(currentDayProgress) - 0.5f) / 0.1f)) * 0.5f;
    } 
    else if (currentDayProgress < 0.91f) {
        ambientTerrainLight = 0.2f;
    } else {
        ambientTerrainLight = 0.2f + ((static_cast<float>(currentDayProgress) - 0.91f) / 0.09f) * 0.5f;
    }

    // Update chunk visibilities by performing frustum culling
    for (auto& [pos, chunkStack] : mChunkStacks) {
        for (auto it = chunkStack.begin(); it != chunkStack.end(); ++it) {
            (*it)->UpdateVisiblity(frustum);
        }
    }

    glDepthFunc(GL_LEQUAL);
    glm::mat4 model = glm::rotate(glm::mat4(1.0f), static_cast<float>(mCurrentTime) * 0.01f, glm::vec3(0.0f, 1.0f, 0.0f));
    mSkybox.Draw(perspective, glm::mat4(glm::mat3((view))), model, currentDayProgress);
    glDepthFunc(GL_LESS);

    // Draw opaque
    mChunkShader.Bind();
    mChunkShader.SetMat4("projection", perspective);
    mChunkShader.SetMat4("view", view);
    mChunkShader.SetVec3("grass_color", mGrassColor);
    mChunkShader.SetFloat("ambient", ambientTerrainLight);
    glActiveTexture(GL_TEXTURE0); 
    mTextureAtlases[mCurrentAtlasID].Bind();
    mChunkShader.SetInt("tex_array", 0);

    glActiveTexture(GL_TEXTURE1);
    mGrassSideMask.Bind();
    mChunkShader.SetInt("grass_mask", 1);
 
    for (auto& [pos, stack] : mChunkStacks) {
        stack.Draw(mChunkShader, totalChunks, chunksDrawn);
    }

    // Draw custom models
    glDisable(GL_CULL_FACE);
    mCustomModelShader.Bind();
    mCustomModelShader.SetMat4("projection", perspective);
    mCustomModelShader.SetMat4("view", view);
    glActiveTexture(GL_TEXTURE0);
    mCustomModelShader.SetInt("tex_array", 0);
    mCustomModelShader.SetVec3("foliage_color", mFoliageColor);
    mCustomModelShader.SetFloat("ambient", ambientTerrainLight);
    for (auto& [pos, stack] : mChunkStacks) {
        stack.DrawCustomModel(mCustomModelShader, totalChunks, chunksDrawn);
    }
    glEnable(GL_CULL_FACE);

    // Draw water
    mWaterShader.Bind();
    mWaterShader.SetMat4("projection", perspective);
    mWaterShader.SetMat4("view", view);
    mWaterShader.SetVec3("color", mWaterColor);
    mWaterShader.SetFloat("ambient", ambientTerrainLight);
    glActiveTexture(GL_TEXTURE0);
    mWaterShader.SetInt("tex_array", 0);
    glEnable(GL_BLEND);
    for (auto& [pos, stack] : mChunkStacks) {
        stack.DrawWater(mWaterShader, totalChunks, chunksDrawn);
    }
    glDisable(GL_BLEND);
}

void World::GenerateChunks()
{
    int totalRenderDistance = mChunkLoadDistance + mChunkPartialLoadDistance;
    int tasks = 0;

    // Detect what chunk the player is in
    glm::vec3 playerPos = mPlayer.camera.position;
    glm::ivec2 playerChunkPos = glm::ivec2(
        static_cast<int>(floor(playerPos.x) / Chunk::SIZE),
        static_cast<int>(floor(playerPos.z) / Chunk::SIZE)
    );

    // Unload chunks out of distance
    std::vector<std::unordered_map<glm::ivec2, ChunkStack>::iterator> iteratorsToRemove;
    for (auto it = mChunkStacks.begin(); it != mChunkStacks.end();) {
        ChunkStack& stack = it->second;
        glm::ivec2 stackPos = stack.GetPosition();
        glm::ivec2 distFromPlayer = stackPos - playerChunkPos;
        int dist = static_cast<int>(std::roundf(glm::length(glm::vec2(distFromPlayer))));
        if (dist > totalRenderDistance && !stack.is_in_task) {
            if (tasks < mMaxTasksPerFrame) {
                tasks++;
                mUnloadPool.push_task([&stack, this] {
                    stack.Unload(mWorldDirectory);
                    });
                iteratorsToRemove.push_back(it);
            }
            else {
                break;
            }
        } 
        ++it;
    }
    mUnloadPool.wait_for_tasks();
    for (auto it : iteratorsToRemove) {
        mChunkStacks.erase(it);
    }

    if (tasks >= mMaxTasksPerFrame) {
        return;
    }
    
    // Iterate over our chunk circle
    for (int x = -totalRenderDistance; x <= totalRenderDistance; x++) {
        for (int z = -totalRenderDistance; z <= totalRenderDistance; z++) {
            int radius = static_cast<int>(std::roundf(sqrtf(x * x + z * z)));
            // If stack is outside our max radius - skip
            if (radius > totalRenderDistance) {
                continue;
            }

            glm::ivec2 pos = playerChunkPos + glm::ivec2( x, z );
            auto find = mChunkStacks.find(pos);

            // Inner radius (chunk loading)
            if (radius < mChunkLoadDistance) {
                // If no chunk found, create chunk and fully load
                if (find == mChunkStacks.end()) {
                    if (tasks < mMaxTasksPerFrame) {
                        auto emplace = mChunkStacks.emplace(pos, pos);
                        ChunkStack& stack = emplace.first->second;
                        tasks++;
                        stack.is_in_task = true;
                        mTaskPool.push_task([this, &stack] {
                            stack.FullyLoad(mWorldDirectory, mSeed, mPerlin);
                            stack.is_in_task = false;
                            });
                    }
                    else {
                        return;
                    }
                }
                else {
                    // If chunk is found, upgrade from partially loaded to fully loaded
                    ChunkStack& stack = find->second;
                    if (stack.state == ChunkStackState::PARTIALLY_LOADED && !stack.is_in_task) {
                        if (tasks < mMaxTasksPerFrame) {
                            tasks++;
                            stack.is_in_task = true;
                            mTaskPool.push_task([this, &stack] {
                                stack.FullyLoad(mWorldDirectory, mSeed, mPerlin);
                                stack.is_in_task = false;
                                });
                        }
                        else {
                            return;
                        }
                    }

                    // Buffer any chunks in it if necessary
                    for (auto& chunk : stack) {
                        if (chunk->needsBuffering) {
                            if (tasks < mMaxTasksPerFrame) {
                                tasks++;
                                chunk->BufferData();
                            }
                            else {
                                return;
                            }
                        }
                    }
                }
            }
            // Outer radius (partial chunk loading)
            else {
                // If no chunk found, create chunk and partially load
                if (find == mChunkStacks.end()) {
                    if (tasks < mMaxTasksPerFrame) {
                        auto emplace = mChunkStacks.emplace(pos, pos);
                        ChunkStack& stack = emplace.first->second;
                        tasks++;
                        stack.is_in_task = true;
                        mTaskPool.push_task([this, &stack] {
                            stack.PartiallyLoad(mWorldDirectory, mSeed, mPerlin);
                            stack.is_in_task = false;
                            });
                    }
                    else {
                        return;
                    }
                }
                else {
                    // If chunk is found, downgrade from loaded loaded to partially loaded
                    ChunkStack& stack = find->second;
                    if (stack.state == ChunkStackState::LOADED && !stack.is_in_task) {
                        if (tasks < mMaxTasksPerFrame) {
                            tasks++;
                            stack.is_in_task = true;
                            mTaskPool.push_task([this, &stack] {
                                stack.PartiallyLoad(mWorldDirectory, mSeed, mPerlin);
                                stack.is_in_task = false;
                                });
                        }
                        else {
                            return;
                        }
                    }

                    // Buffer any chunks in it if necessary
                    for (auto& chunk : stack) {
                        if (chunk->needsBuffering) {
                            if (tasks < mMaxTasksPerFrame) {
                                tasks++;
                                chunk->BufferData();
                            }
                            else {
                                return;
                            }
                        }    
                    }
                }
            }
        }
    }
}

void World::TrySwitchToNextTextureAtlas()
{
    double currentTime = glfwGetTime();
    if (currentTime - mLastAtlasSwitch > 0.2) {
        mCurrentAtlasID = (mCurrentAtlasID + 1) % MAX_ANIMATION_FRAMES;
        glActiveTexture(GL_TEXTURE0);
        mTextureAtlases[mCurrentAtlasID].Bind();
        mLastAtlasSwitch = currentTime;
    }
}

const ChunkStack* World::GetChunkStack(glm::ivec2 pos) const
{
    auto find = mChunkStacks.find(pos);
    if (find == mChunkStacks.end()) {
        return nullptr;
    }
    return &find->second;
}

std::shared_ptr<Chunk> World::GetChunk(glm::ivec3 pos) const
{
    const ChunkStack* chunkStack = GetChunkStack(glm::ivec2( pos.x, pos.z ));
    if (chunkStack == nullptr || chunkStack->state != ChunkStackState::LOADED) {
        return nullptr;
    }
    return chunkStack->GetChunk(pos[1]);
}

Block World::GetBlock(glm::ivec3 pos) const
{
    glm::ivec3 chunkPos = GetChunkPosFromGlobalBlockPos(pos);
    std::shared_ptr<Chunk> chunk = GetChunk(chunkPos);
    if (chunk != nullptr) {
        glm::ivec3 blockPos = GetChunkBlockPosFromGlobalBlockPos(pos);
        return chunk->GetBlock(blockPos);
    }
    return Block(BlockType::AIR, 0, false);
}

void World::SetBlock(glm::ivec3 pos, Block block)
{
    glm::ivec3 chunkPos = GetChunkPosFromGlobalBlockPos(pos);
    std::shared_ptr<Chunk> chunk = GetChunk(chunkPos);

    if (chunk != nullptr) {
        glm::ivec3 blockPos = GetChunkBlockPosFromGlobalBlockPos(pos);
        chunk->SetBlock(blockPos, block);
    }
}

void World::SetBlockAndRemesh(glm::ivec3 pos, Block block)
{
    glm::ivec3 chunkPos = GetChunkPosFromGlobalBlockPos(pos);
    std::shared_ptr<Chunk> chunk = GetChunk(chunkPos);

    if (chunk != nullptr) {
        glm::ivec3 blockPos = GetChunkBlockPosFromGlobalBlockPos(pos);
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
