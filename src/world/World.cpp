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

    for (auto& [pos, region] : mChunkRegionMap) {
        region.Draw(chunkShader);
    }
}

void World::GenerateChunkRegions()
{
    Vec3 playerPos = mCamera.GetPosition();
    Vec3 playerRegionPosFloat = playerPos / CHUNK_REGION_BLOCK_SIZE;
    iVec2 playerRegionPosInt = { static_cast<int>(floor(playerRegionPosFloat[0])), static_cast<int>(floor(playerRegionPosFloat[2])) };

    // Unload out of render distance regions
    /*
    for (auto it = mChunkRegionMap.begin(); it != mChunkRegionMap.end();)
    {
        ChunkRegion* region = &(it->second);
        if (region->GetPosition() != playerRegionPosInt) {
            if (region->readyForDeletion) {
                it = mChunkRegionMap.erase(it);
            }
            else if (!region->startedDeletion) {
                mRegionUnloadPool.push_task([region] {
                    region->PrepareForDeletion();
                    });
                ++it;
            }
            else {
                ++it;
            }
        }
        else
        {
            ++it;
        }
    }*/

    auto findResult = mChunkRegionMap.find(playerRegionPosInt);
    ChunkRegion* region = nullptr;
    if (findResult == mChunkRegionMap.end()) {
        auto emplaceResult = mChunkRegionMap.emplace(playerRegionPosInt, playerRegionPosInt);
        region = &emplaceResult.first->second;
    }
    else {
        region = &findResult->second;
    }
    region->GenerateChunks(mPerlin);
    region->BufferChunks();
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