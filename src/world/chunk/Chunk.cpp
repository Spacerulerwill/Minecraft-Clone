/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include <world/chunk/Chunk.hpp>
#include <world/chunk/Constants.hpp>
#include <world/Block.hpp>
#include <GLFW/glfw3.h>
#include <cmath>

std::size_t VoxelIndex(iVec3 pos) {
    return pos[2] + (pos[0] << CHUNK_SIZE_EXP) + (pos[1] << CHUNK_SIZE_EXP_X2);
}

Chunk::Chunk(iVec3 pos) : mPos(pos)
{
    // Setup buffers
    VertexBufferLayout bufferLayout;
    bufferLayout.AddAttribute<unsigned int>(2);
    mVAO.AddBuffer(mVBO, bufferLayout);

    // Translate to its chunk position
    mModel *= translate(Vec3{
        static_cast<float>(pos[0] * CS),
        static_cast<float>(pos[1] * CS),
        static_cast<float>(pos[2] * CS)
        });
}

iVec3 Chunk::GetPosition() const
{
    return mPos;
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
    needsBuffering = true;
}

void Chunk::BufferData()
{
    if (mVertices.size() > 0) {
        mVBO.BufferData(mVertices.data(), mVertices.size() * sizeof(ChunkMesher::ChunkVertex), GL_STATIC_DRAW);
        mVertexCount = mVertices.size();
        std::vector<ChunkMesher::ChunkVertex>().swap(mVertices);
    }

    needsBuffering = false;

    if (loaded == false) {
        firstBufferTime = static_cast<float>(glfwGetTime());
        loaded = true;
    }
}

void Chunk::Draw(Shader& shader, float currentTime)
{
    if (mVertexCount > 0) {
        mVAO.Bind();
        shader.SetMat4("model", mModel);
        shader.SetFloat("firstBufferTime", firstBufferTime);
        shader.SetFloat("time", currentTime);
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mVertexCount));
    }
}

BlockID Chunk::GetBlock(iVec3 pos) const
{
    return mBlocks[VoxelIndex(pos)];
}

void Chunk::SetBlock(iVec3 pos, BlockID block)
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