/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include <world/World.hpp>
#include <cmath>
#include <util/Log.hpp>

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


void World::Draw(int* totalChunks, int* chunksDrawn)
{
    float currentTime = static_cast<float>(glfwGetTime());

    Mat4 perspective = player.camera.perspectiveMatrix;
    Mat4 view = player.camera.GetViewMatrix();

    glDepthFunc(GL_LEQUAL);
    mSkybox.Draw(perspective, translationRemoved(view));
    glDepthFunc(GL_LESS);

    chunkShader.Bind();
    chunkShader.SetMat4("projection", perspective);
    chunkShader.SetMat4("view", view);
    chunkShader.SetInt("LOD", LOD);
    mTextureAtlas.Bind();
    glActiveTexture(GL_TEXTURE0);
    chunkShader.SetInt("tex_array", 0);
    glActiveTexture(GL_TEXTURE1);
    mGrassSideMask.Bind();
    chunkShader.SetInt("grass_mask", 1);

    for (auto& [pos, stack] : mChunkStacks) {
        stack.Draw(chunkShader, currentTime, totalChunks, chunksDrawn);
    }
}

void World::GenerateChunks()
{
    Vec3 playerPos = player.camera.position;
    iVec2 playerChunkPos{
        static_cast<int>(floor(playerPos[0]) / CS),
        static_cast<int>(floor(playerPos[2]) / CS)
    };

    // unload chunks out of distance
    for (auto it = mChunkStacks.begin(); it != mChunkStacks.end();) {
        ChunkStack& stack = it->second;

        iVec2 stackPos = stack.GetPosition();
        if ((stackPos[0] < playerChunkPos[0] - mRenderDistance ||
            stackPos[0] > playerChunkPos[0] + mRenderDistance ||
            stackPos[1] < playerChunkPos[1] - mRenderDistance ||
            stackPos[1] > playerChunkPos[1] + mRenderDistance) && !stack.isBeingmMeshed) {
            it = mChunkStacks.erase(it);
        }
        else {
            ++it;
        }
    }

    std::size_t chunksBuffered = 0;
    std::size_t chunksCreated = 0;
    for (int x = -mRenderDistance; x <= mRenderDistance; x++) {
        for (int z = -mRenderDistance; z <= mRenderDistance; z++) {
            iVec2 pos = playerChunkPos + iVec2{ x,z };
            auto find = mChunkStacks.find(pos);
            if (find == mChunkStacks.end()) {
                if (chunksCreated < mBufferPerFrame) {
                    auto emplace = mChunkStacks.emplace(pos, pos);
                    ChunkStack& chunkStack = emplace.first->second;
                    chunkStack.isBeingmMeshed = true;
                    mLoadPool.push_task([&chunkStack, this] {
                        chunkStack.GenerateTerrain(mPerlin);
                        for (auto it = chunkStack.begin(); it != chunkStack.end(); ++it) {
                            std::shared_ptr<Chunk> chunk = (*it);
                            chunk->CreateMesh();
                        }
                        chunkStack.isBeingmMeshed = false;
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
                            chunksBuffered++;
                            chunk->BufferData();
                        }
                    }
                    else {
                        return;
                    }
                }
            }
        }
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