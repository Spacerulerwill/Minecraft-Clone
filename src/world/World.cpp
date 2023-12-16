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

    for (auto& [pos, region] : mChunkRegionMap) {
        region.Draw(mCamera.GetPosition(), chunkShader);
    }
}

void World::GenerateChunkRegions()
{
    Vec3 playerPos = mCamera.GetPosition();

    // Unload out of render distance regions
    for (auto it = mChunkRegionMap.begin(); it != mChunkRegionMap.end();)
    {
        ChunkRegion* region = &(it->second);
        iVec2 regionPos = region->GetPosition();

        // Current region position for player
        Vec3 p = playerPos / CHUNK_REGION_BLOCK_SIZE;
        iVec2 playerRegionPos = iVec2{
                static_cast<int>(floor(p[0])),
                static_cast<int>(floor(p[2]))
        };

        // Check if any of the regions are out of bounds
        int minRegionX = static_cast<int>(floor(((playerPos[0] - CHUNK_RENDER_DISTANCE * CS) / CHUNK_REGION_BLOCK_SIZE)));
        int maxRegionX = static_cast<int>(floor(((playerPos[0] + (CHUNK_RENDER_DISTANCE - 1) * CS) / CHUNK_REGION_BLOCK_SIZE)));
        int minRegionZ = static_cast<int>(floor(((playerPos[2] - CHUNK_RENDER_DISTANCE * CS) / CHUNK_REGION_BLOCK_SIZE)));
        int maxRegionZ = static_cast<int>(floor(((playerPos[2] + (CHUNK_RENDER_DISTANCE - 1) * CS) / CHUNK_REGION_BLOCK_SIZE)));

        if (regionPos[0] < minRegionX || regionPos[0] > maxRegionX || regionPos[1] < minRegionZ || regionPos[1] > maxRegionZ) {
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
    }

    for (int x = -CHUNK_RENDER_DISTANCE; x < CHUNK_RENDER_DISTANCE; x++) {
        for (int z = -CHUNK_RENDER_DISTANCE; z < CHUNK_RENDER_DISTANCE; z++) {
            Vec3 newPlayerPos = playerPos + Vec3{ x * static_cast<float>(CS), 0.0f, z * static_cast<float>(CS) };
            Vec3 newPlayerChunkRegionPosFloat = newPlayerPos / CHUNK_REGION_BLOCK_SIZE;
            iVec2 newRegionPos = iVec2{
                static_cast<int>(floor(newPlayerChunkRegionPosFloat[0])),
                static_cast<int>(floor(newPlayerChunkRegionPosFloat[2]))
            };

            auto findResult = mChunkRegionMap.find(newRegionPos);
            ChunkRegion* region = nullptr;
            if (findResult == mChunkRegionMap.end()) {
                auto emplaceResult = mChunkRegionMap.emplace(newRegionPos, newRegionPos);
                region = &emplaceResult.first->second;
            }
            else {
                region = &findResult->second;
            }

            if (!region->FinishedGenerating()) {
                region->GenerateChunks(mPerlin);
                region->BufferChunks();
                return;
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