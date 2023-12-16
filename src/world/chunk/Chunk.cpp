/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include <world/chunk/Chunk.hpp>
#include <world/chunk/Constants.hpp>
#include <world/chunk/ChunkRegion.hpp>
#include <util/Log.hpp>

Chunk::Chunk(iVec3 pos) : mPos(pos)
{
    // Setup buffers
    VertexBufferLayout bufferLayout;
    bufferLayout.AddAttribute<unsigned int>(1);
    mVAO.AddBuffer(mVBO, bufferLayout);

    // Translate to its chunk position
    mModel *= translate(Vec3{
        static_cast<float>(pos[0] * CS),
        static_cast<float>(pos[1] * CS),
        static_cast<float>(pos[2] * CS)
        });
}

void Chunk::CopyNeighbourChunkEdgeBlocks(ChunkRegion* region)
{
    iVec3 localChunkPos = mPos % CHUNK_REGION_SIZE;
    if (localChunkPos[0] < 0) {
        localChunkPos[0] = CHUNK_REGION_SIZE - (localChunkPos[0] * -1);
    }
    if (localChunkPos[1] < 0) {
        localChunkPos[1] = CHUNK_REGION_SIZE - (localChunkPos[1] * -1);
    }
    if (localChunkPos[2] < 0) {
        localChunkPos[2] = CHUNK_REGION_SIZE - (localChunkPos[2] * -1);
    }
    Chunk* belowChunk = region->GetChunk(localChunkPos + iVec3{ 0, -1, 0 });
    Chunk* aboveChunk = region->GetChunk(localChunkPos + iVec3{ 0, 1, 0 });
    Chunk* leftChunk = region->GetChunk(localChunkPos + iVec3{ -1, 0, 0 });
    Chunk* rightChunk = region->GetChunk(localChunkPos + iVec3{ 1, 0, 0 });
    Chunk* frontChunk = region->GetChunk(localChunkPos + iVec3{ 0, 0, -1 });
    Chunk* backChunk = region->GetChunk(localChunkPos + iVec3{ 0, 0, 1 });

    if (belowChunk != nullptr) {
        for (int x = 1; x < CS_P_MINUS_ONE; x++) {
            for (int z = 1; z < CS_P_MINUS_ONE; z++) {
                SetBlock(belowChunk->GetBlock(iVec3{ x, CS, z }), iVec3{ x, 0, z });
            }
        }
    }

    if (aboveChunk != nullptr) {
        for (int x = 1; x < CS_P_MINUS_ONE; x++) {
            for (int z = 1; z < CS_P_MINUS_ONE; z++) {
                SetBlock(aboveChunk->GetBlock(iVec3{ x, 1, z }), iVec3{ x, CS_P_MINUS_ONE, z });
            }
        }
    }

    if (leftChunk != nullptr) {
        for (int y = 1; y < CS_P_MINUS_ONE; y++) {
            for (int z = 1; z < CS_P_MINUS_ONE; z++) {
                SetBlock(leftChunk->GetBlock(iVec3{ CS, y, z }), iVec3{ 0, y, z });
            }
        }
    }

    if (rightChunk != nullptr) {
        for (int y = 1; y < CS_P_MINUS_ONE; y++) {
            for (int z = 1; z < CS_P_MINUS_ONE; z++) {
                SetBlock(rightChunk->GetBlock(iVec3{ 1, y, z }), iVec3{ CS_P_MINUS_ONE, y, z });
            }
        }
    }

    if (frontChunk != nullptr) {
        for (int x = 1; x < CS_P_MINUS_ONE; x++) {
            for (int y = 1; y < CS_P_MINUS_ONE; y++) {
                SetBlock(frontChunk->GetBlock(iVec3{ x, y, CS }), iVec3{ x, y, 0 });
            }
        }
    }

    if (backChunk != nullptr) {
        for (int x = 1; x < CS_P_MINUS_ONE; x++) {
            for (int y = 1; y < CS_P_MINUS_ONE; y++) {
                SetBlock(backChunk->GetBlock(iVec3{ x, y, 1 }), iVec3{ x, y, CS_P_MINUS_ONE });
            }
        }
    }
}

void Chunk::AllocateMemory()
{
    mBlocks.resize(CS_P3);
}

void Chunk::ReleaseMemory()
{
    std::vector<BlockID>().swap(mBlocks);
}
void Chunk::CreateMesh() {
    // Erase previous data
    std::vector<ChunkMesher::ChunkVertex>().swap(mVertices);
    mVertices.reserve(CS_P3 * 3);
    mVertexCount = 0;

    // Mesh
    mVertices = ChunkMesher::BinaryGreedyMesh(mBlocks);
}

void Chunk::ReleaseMemory()
{
    std::vector<BlockID>().swap(mBlocks);
}

void Chunk::BufferData()
{
    if (mVertices.size() > 0) {
        mVBO.BufferData(mVertices.data(), mVertices.size() * sizeof(ChunkMesher::ChunkVertex), GL_STATIC_DRAW);
        mVertexCount = mVertices.size();
        std::vector<ChunkMesher::ChunkVertex>().swap(mVertices);
    }
}

void Chunk::Draw(Shader& shader)
{
    mVAO.Bind();
    shader.SetMat4("model", mModel);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mVertexCount));
}

BlockID Chunk::GetBlock(iVec3 pos) const
{
    return mBlocks[VoxelIndex(pos)];
}

void Chunk::SetBlock(BlockID block, iVec3 pos)
{
    mBlocks[VoxelIndex(pos)] = block;
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