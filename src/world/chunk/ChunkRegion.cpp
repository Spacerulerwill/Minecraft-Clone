/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include <world/chunk/ChunkRegion.hpp>

ChunkRegion::ChunkRegion(iVec2 pos) : mPos(pos)
{
    mChunkStacks.reserve(CHUNK_REGION_SIZE_SQUARED);
    for (int x = 0; x < CHUNK_REGION_SIZE; x++) {
        for (int z = 0; z < CHUNK_REGION_SIZE; z++) {
            mChunkStacks.emplace_back(iVec2{ (pos[0] * CHUNK_REGION_SIZE) + x, (pos[1] * CHUNK_REGION_SIZE) + z });
        }
    }
}

iVec2 ChunkRegion::GetPosition() const
{
    return mPos;
}

void ChunkRegion::GenerateChunks(const siv::PerlinNoise& perlin)
{
    if (!mHasStartedTerrainGeneration) {
        for (ChunkStack& chunkStack : mChunkStacks) {
            mTerrainGenerationPool.push_task([&chunkStack, &perlin] {
                chunkStack.GenerateTerrain(perlin);
                });
        }
        mHasStartedTerrainGeneration = true;
        return;
    }
    else if (mTerrainGenerationPool.get_tasks_total() != 0) {
        return;
    }

    if (!mHasStartedChunkMerging) {
        for (int x = 0; x < CHUNK_REGION_SIZE; x++) {
            for (int z = 0; z < CHUNK_REGION_SIZE; z++) {
                ChunkStack* stack = GetChunkStack(iVec2{ x,z });
                for (int y = 0; y < stack->size(); y++) {
                    Chunk* chunk = stack->GetChunk(y);
                    mChunkMergePool.push_task([chunk, this] {
                        chunk->CopyNeighbourChunkEdgeBlocks(this);
                        });
                }
            }
        }
        mHasStartedChunkMerging = true;
        return;
    }
    else if (mChunkMergePool.get_tasks_total() != 0) {
        return;
    }

    if (!mHasStartedChunkMeshing) {
        for (ChunkStack& chunkStack : mChunkStacks) {
            for (auto it = chunkStack.begin(); it != chunkStack.end(); ++it) {
                mChunkMeshPool.push_task([it, this] {
                    (*it).CreateMesh();
                    mChunkBufferQueue.enqueue(&(*it));
                    });
            }
        }
        mHasStartedChunkMeshing = true;
    }
}

void ChunkRegion::Draw(Shader& shader)
{
    for (auto& chunkStack : mChunkStacks) {
        chunkStack.Draw(shader);
    }
}

void ChunkRegion::BufferChunks()
{
    std::size_t dequeued = mChunkBufferQueue.try_dequeue_bulk(mChunkBufferDequeResult.begin(), mChunkBufferDequeResult.size());
    for (size_t i = 0; i < dequeued; i++) {
        mChunkBufferDequeResult[i]->BufferData();
    }
}

Chunk* ChunkRegion::GetChunk(iVec3 pos) {
    ChunkStack* stack = GetChunkStack(iVec2{ pos[0], pos[2]
        });
    if (stack != nullptr) {
        return stack->GetChunk(pos[1]);
    }
    else {
        return nullptr;
    }
}

ChunkStack* ChunkRegion::GetChunkStack(iVec2 pos) {
    if (pos[0] < 0 || pos[0] >= CHUNK_REGION_SIZE || pos[1] < 0 || pos[1] >= CHUNK_REGION_SIZE) {
        return nullptr;
    }
    else {
        int chunkStackIndex = ChunkStackIndex(pos[0], pos[1]);
        return &mChunkStacks.at(chunkStackIndex);
    }
}

void ChunkRegion::PrepareForDeletion()
{
    // Purge any tasks in the queue
    mTerrainGenerationPool.purge();
    mChunkMergePool.purge();
    mChunkMeshPool.wait_for_tasks();

    // Wait for any tasks already processing
    mTerrainGenerationPool.wait_for_tasks();
    mChunkMergePool.wait_for_tasks();
    mChunkMeshPool.wait_for_tasks();

    moodycamel::ConcurrentQueue<Chunk*>().swap(mChunkBufferQueue);

    // Deallocate memory for each chunk
    for (auto& stack : mChunkStacks) {
        for (auto it = stack.begin(); it != stack.end(); ++it) {
            it->ReleaseMemory();
        }
    }
    // Mark as ready for deletion
    readyForDeletion = true;
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
