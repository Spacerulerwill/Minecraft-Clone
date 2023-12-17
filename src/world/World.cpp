/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include <world/World.hpp>
#include <util/Log.hpp>

void World::Draw()
{
    Mat4 perspective = mCamera.GetPerspectiveMatrix();
    Mat4 view = mCamera.GetViewMatrix();

    glDepthFunc(GL_LEQUAL);
    mSkybox.Draw(perspective, translationRemoved(view));
    glDepthFunc(GL_LESS);

    chunkShader.Bind();
    chunkShader.SetMat4("projection", perspective);
    chunkShader.SetMat4("view", view);
    chunkShader.SetInt("LOD", LOD);

    for (auto& [pos, stack] : mChunkStacks) {
        stack.Draw(chunkShader);
    }
}

void World::GenerateChunks()
{
    Vec3 playerPos = mCamera.GetPosition();
    iVec2 playerChunkPos{
        static_cast<int>(floor(playerPos[0]) / CS),
        static_cast<int>(floor(playerPos[2]) / CS)
    };

    // unload chunks out of distance
    for (auto it = mChunkStacks.begin(); it != mChunkStacks.end();) {
        ChunkStack& stack = it->second;

        ++++++++++++++ + iVec2 stackPos = stack.GetPosition();
        if ((stackPos[0] < playerChunkPos[0] - CHUNK_RENDER_DISTANCE ||
            stackPos[0] > playerChunkPos[0] + CHUNK_RENDER_DISTANCE ||
            stackPos[1] < playerChunkPos[1] - CHUNK_RENDER_DISTANCE ||
            stackPos[1] > playerChunkPos[1] + CHUNK_RENDER_DISTANCE) && !stack.isBeingmMeshed) {
            it = mChunkStacks.erase(it);
        }
        else {
            ++it;
        }
    }

    std::size_t chunksBuffered = 0;
    std::size_t chunksCreated = 0;
    for (int x = -CHUNK_RENDER_DISTANCE; x <= CHUNK_RENDER_DISTANCE; x++) {
        for (int z = -CHUNK_RENDER_DISTANCE; z <= CHUNK_RENDER_DISTANCE; z++) {
            iVec2 pos = playerChunkPos + iVec2{ x,z };
            auto find = mChunkStacks.find(pos);
            if (find == mChunkStacks.end()) {
                if (chunksCreated < CHUNK_BUFFER_PER_FRAME) {
                    auto emplace = mChunkStacks.emplace(pos, pos);
                    ChunkStack& chunkStack = emplace.first->second;
                    chunkStack.isBeingmMeshed = true;
                    mTaskPool.push_task([&chunkStack, this] {
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
                    if (chunksBuffered < CHUNK_BUFFER_PER_FRAME) {
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