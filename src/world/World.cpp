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
            result[i] = ((globalBlockPos[i] + 1) / Chunk::SIZE) - 1;
        }
        else {
            result[i] = globalBlockPos[i] / Chunk::SIZE;
        }
    }
    return result;
}

iVec3 GetChunkBlockPosFromGlobalBlockPos(iVec3 globalBlockPos)
{
    iVec3 result;
    for (int i = 0; i < 3; i++) {
        if (globalBlockPos[i] >= 0) {
            result[i] = 1 + (globalBlockPos[i] % Chunk::SIZE);
        }
        else {
            result[i] = Chunk::SIZE_PADDED_SUB_1 - (1 + (abs(globalBlockPos[i]) - 1) % Chunk::SIZE);
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
    worldStartTime = worldSave.elapsedTime;
    currentTime = worldSave.elapsedTime;
    
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
    LOG_INFO("Loading spawn chunks...");
    Vec3 playerPos = player.camera.position;
    iVec2 playerChunkPos{
        static_cast<int>(floor(playerPos[0]) / Chunk::SIZE),
        static_cast<int>(floor(playerPos[2]) / Chunk::SIZE)
    };

    // Load all chunks
    int totalRenderDistance = mChunkLoadDistance + mChunkPartialLoadDistance;
    for (int x = -totalRenderDistance; x <= totalRenderDistance; x++) {
        for (int z = -totalRenderDistance; z <= totalRenderDistance; z++) {
            int radius = static_cast<int>(std::round(sqrtf(x * x + z * z)));
            iVec2 pos = playerChunkPos + iVec2{ x,z };
            if (radius < mChunkLoadDistance) {
                auto emplace = mChunkStacks.emplace(pos, pos);
                ChunkStack& chunkStack = emplace.first->second;
                chunkStack.state = ChunkStackState::LOADED;
                mTaskPool.push_task([this, worldDirectory, &chunkStack]() {
                    chunkStack.FullyLoad(worldDirectory, seed, mPerlin);
                    });
            }
            else if (radius <= totalRenderDistance) {
                auto emplace = mChunkStacks.emplace(pos, pos);
                ChunkStack& chunkStack = emplace.first->second;
                chunkStack.state = ChunkStackState::PARTIALLY_LOADED;
                mTaskPool.push_task([this, worldDirectory, &chunkStack]() {
                    chunkStack.PartiallyLoad(worldDirectory, seed, mPerlin);
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
    worldLoadedTime = glfwGetTime();
}

World::~World() {
    mTaskPool.purge();
    mTaskPool.wait_for_tasks();

    // Save to disk
    WorldSave worldSave {
        .seed = seed,
        .elapsedTime = currentTime
    };
    WriteStructToDisk(fmt::format("{}/world.data", worldDirectory), worldSave);

    PlayerSave playerSave {
        .pos = player.camera.position,
        .pitch = player.camera.pitch,
        .yaw = player.camera.yaw
    };
    WriteStructToDisk(fmt::format("{}/player.data", worldDirectory), playerSave);  

    // Unload all remaining chunks 
    for (auto& [pos, stack] : mChunkStacks) {
        mUnloadPool.push_task([&stack, this] {
            stack.Unload(worldDirectory);
            });

    }
    mUnloadPool.wait_for_tasks();
}

void World::Draw(const Frustum& frustum, int* totalChunks, int* chunksDrawn)
{
    double glfwTime = glfwGetTime();
    currentTime = worldStartTime + glfwTime - worldLoadedTime;
    double day = currentTime / World::DAY_DURATION;
    double currentDay;
    double currentDayProgress = std::modf(day, &currentDay);

    Mat4 perspective = player.camera.perspectiveMatrix;
    Mat4 view = player.camera.GetViewMatrix();
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
    Mat4 model = rotate(Vec3{0.0f, 1.0f, 0.0f}, currentTime * 0.01f); 
    mSkybox.Draw(perspective, translationRemoved(view), model, currentDayProgress);
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
        stack.Draw(chunkShader, totalChunks, chunksDrawn);
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
        stack.DrawWater(waterShader, totalChunks, chunksDrawn);
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
        stack.DrawCustomModel(customModelShader, totalChunks, chunksDrawn);
    }
    glEnable(GL_CULL_FACE);
}

void World::GenerateChunks()
{
    int totalRenderDistance = mChunkLoadDistance + mChunkPartialLoadDistance;
    int chunksBuffered = 0;

    // Detect what chunk the player is in
    Vec3 playerPos = player.camera.position;
    iVec2 playerChunkPos{
        static_cast<int>(floor(playerPos[0]) / Chunk::SIZE),
        static_cast<int>(floor(playerPos[2]) / Chunk::SIZE)
    };

    // Unload chunks out of distance
    std::vector<std::unordered_map<iVec2, ChunkStack>::iterator> iteratorsToRemove;
    for (auto it = mChunkStacks.begin(); it != mChunkStacks.end();) {
        ChunkStack& stack = it->second;
        iVec2 stackPos = stack.GetPosition();
        iVec2 distFromPlayer = stackPos - playerChunkPos;
        int dist = static_cast<int>(std::roundf(distFromPlayer.length()));
        if (dist > totalRenderDistance && !stack.is_in_task) {
            mUnloadPool.push_task([&stack, this] {
                stack.Unload(worldDirectory);
                stack.is_in_task = false;
            });
            iteratorsToRemove.push_back(it);
        } 
        ++it;
    }
    mUnloadPool.wait_for_tasks();
    for (auto it : iteratorsToRemove) {
        mChunkStacks.erase(it);
    }
    
    // Iterate over our chunk circle
    for (int x = -totalRenderDistance; x <= totalRenderDistance; x++) {
        for (int z = -totalRenderDistance; z <= totalRenderDistance; z++) {
            int radius = static_cast<int>(std::roundf(sqrtf(x * x + z * z)));
            // If stack is outside our max radius - skip
            if (radius > totalRenderDistance) {
                continue;
            }

            iVec2 pos = playerChunkPos + iVec2{ x, z };
            auto find = mChunkStacks.find(pos);

            // Inner radius (chunk loading)
            if (radius < mChunkLoadDistance) {
                // If no chunk found, create chunk and fully load
                if (find == mChunkStacks.end()) {
                    auto emplace = mChunkStacks.emplace(pos, pos);
                    ChunkStack& stack = emplace.first->second;
                    stack.is_in_task = true;
                    mTaskPool.push_task([this, &stack] {
                        stack.FullyLoad(worldDirectory, seed, mPerlin);
                        stack.is_in_task = false;
                        });
                }
                else {
                    // If chunk is found, upgrade from partially loaded to fully loaded
                    ChunkStack& stack = find->second;
                    if (stack.state == ChunkStackState::PARTIALLY_LOADED && !stack.is_in_task) {
                        stack.is_in_task = true;
                        mTaskPool.push_task([this, &stack] {
                            stack.FullyLoad(worldDirectory, seed, mPerlin);
                            stack.is_in_task = false;
                            });
                    }

                    // Buffer any chunks in it if necessary
                    for (auto& chunk : stack) {
                        if (chunk->needsBuffering) {
                            chunk->BufferData();
                        }
                    }
                }
            }
            // Outer radius (partial chunk loading)
            else {
                // If no chunk found, create chunk and partially load
                if (find == mChunkStacks.end()) {
                    auto emplace = mChunkStacks.emplace(pos, pos);
                    ChunkStack& stack = emplace.first->second;
                    stack.is_in_task = true;
                    mTaskPool.push_task([this, &stack] {
                        stack.PartiallyLoad(worldDirectory, seed, mPerlin);
                        stack.is_in_task = false;
                        });
                }
                else {
                    // If chunk is found, downgrade from loaded loaded to partially loaded
                    ChunkStack& stack = find->second;
                    if (stack.state == ChunkStackState::LOADED) {
                        if (!stack.is_in_task) {
                            stack.is_in_task = true;
                            mTaskPool.push_task([this, &stack] {
                                stack.PartiallyLoad(worldDirectory, seed, mPerlin);
                                stack.is_in_task = false;
                                });
                        }
                    }

                    // Buffer any chunks in it if necessary
                    for (auto& chunk : stack) {
                        if (chunk->needsBuffering) {
                            chunk->BufferData();
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
    if (chunkStack == nullptr || chunkStack->state != ChunkStackState::LOADED) {
        return nullptr;
    }
    return chunkStack->GetChunk(pos[1]);
}

BlockID World::GetBlock(iVec3 pos) const
{
    iVec3 chunkPos = GetChunkPosFromGlobalBlockPos(pos);
    std::shared_ptr<Chunk> chunk = GetChunk(chunkPos);
    if (chunk != nullptr) {
        iVec3 blockPos = GetChunkBlockPosFromGlobalBlockPos(pos);
        return chunk->GetBlock(blockPos);
    }
    return AIR;
}

void World::SetBlock(iVec3 pos, BlockID block)
{
    iVec3 chunkPos = GetChunkPosFromGlobalBlockPos(pos);
    std::shared_ptr<Chunk> chunk = GetChunk(chunkPos);

    if (chunk != nullptr) {
        iVec3 blockPos = GetChunkBlockPosFromGlobalBlockPos(pos);
        chunk->SetBlock(blockPos, block);
    }
}

void World::SetBlockAndRemesh(iVec3 pos, BlockID block)
{
    iVec3 chunkPos = GetChunkPosFromGlobalBlockPos(pos);
    std::shared_ptr<Chunk> chunk = GetChunk(chunkPos);

    if (chunk != nullptr) {
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
