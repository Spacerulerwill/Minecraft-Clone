/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include <world/chunk/Chunk.hpp>
#include <world/chunk/Constants.hpp>
#include <world/Block.hpp>
#include <GLFW/glfw3.h>
#include <cmath>
#include <util/Log.hpp>

std::size_t VoxelIndex(iVec3 pos) {
    return pos[2] + (pos[0] << CHUNK_SIZE_EXP) + (pos[1] << CHUNK_SIZE_EXP_X2);
}

Chunk::Chunk(iVec3 pos) : mPos(pos)
{
    // Setup buffers
    VertexBufferLayout bufferLayout;
    bufferLayout.AddAttribute<unsigned int>(2);
    mVAO.AddBuffer(mVBO, bufferLayout);
    mWaterVAO.AddBuffer(mWaterVBO, bufferLayout);
    mCustomModelVAO.AddBuffer(mCustomModelVBO, bufferLayout);

    Vec3 globalPosition = static_cast<Vec3>(pos) * CS;

    // Translate to its chunk position
    mModel *= translate(globalPosition);
    sphere = Sphere{ globalPosition + Vec3{CS_OVER_2, CS_OVER_2, CS_OVER_2}, Vec3{CS_OVER_2, CS_OVER_2, CS_OVER_2}.length() };
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
    // Erase previous data!
    std::vector<ChunkMesher::ChunkVertex>().swap(mVertices);
    mVertices.reserve(CS_P3 * 3);
    mVertexCount = 0;

    std::vector<ChunkMesher::ChunkVertex>().swap(mWaterVertices);
    mWaterVertices.reserve(CS_P3 * 3);
    mWaterVertexCount = 0;

    std::vector<ChunkMesher::ChunkVertex>().swap(mCustomModelVertices);
    mCustomModelVertices.reserve(CS_P3 * 3);
    mCustomModelVertexCount = 0;

    // Mesh
    ChunkMesher::BinaryGreedyMesh(mVertices, mBlocks);
    ChunkMesher::BinaryGreedyMeshTransparentBlock(GLASS, mVertices, mBlocks);
    ChunkMesher::BinaryGreedyMeshTransparentBlock(WATER, mWaterVertices, mBlocks);
    ChunkMesher::MeshCustomModelBlocks(mCustomModelVertices, mBlocks);
    needsBuffering = true;
}

void Chunk::BufferData()
{
    if (mVertices.size() > 0) {
        mVBO.BufferData(mVertices.data(), mVertices.size() * sizeof(ChunkMesher::ChunkVertex), GL_STATIC_DRAW);
        mVertexCount = mVertices.size();
        std::vector<ChunkMesher::ChunkVertex>().swap(mVertices);
    }

    if (mWaterVertices.size() > 0) {
        mWaterVBO.BufferData(mWaterVertices.data(), mWaterVertices.size() * sizeof(ChunkMesher::ChunkVertex), GL_STATIC_DRAW);
        mWaterVertexCount = mWaterVertices.size();
        std::vector<ChunkMesher::ChunkVertex>().swap(mWaterVertices);
    }

    if (mCustomModelVertices.size() > 0) {
        mCustomModelVBO.BufferData(mCustomModelVertices.data(), mCustomModelVertices.size() * sizeof(ChunkMesher::ChunkVertex), GL_STATIC_DRAW);
        mCustomModelVertexCount = mCustomModelVertices.size();
        std::vector<ChunkMesher::ChunkVertex>().swap(mCustomModelVertices);
    }

    needsBuffering = false;

    if (loaded == false) {
        firstBufferTime = static_cast<float>(glfwGetTime());
        loaded = true;
    }
}

void Chunk::Draw(const Frustum& frustum, Shader& shader, int* potentialDrawCalls, int* totalDrawCalls)
{
    if (potentialDrawCalls) (*potentialDrawCalls)++;
    if (mVertexCount > 0) {
        if (sphere.IsOnFrustum(frustum)) {
            if (totalDrawCalls) (*totalDrawCalls)++;
            mVAO.Bind();
            shader.SetMat4("model", mModel);;
            glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mVertexCount));
        }
    }
}

void Chunk::DrawWater(const Frustum& frustum, Shader& shader, int* potentialDrawCalls, int* totalDrawCalls)
{
    if (potentialDrawCalls) (*potentialDrawCalls)++;
    if (mWaterVertexCount > 0) {
        if (sphere.IsOnFrustum(frustum)) {
            if (totalDrawCalls) (*totalDrawCalls)++;
            mWaterVAO.Bind();
            shader.SetMat4("model", mModel);
            glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mWaterVertexCount));
        }
    }
}

void Chunk::DrawCustomModel(const Frustum& frustum, Shader& shader, int* potentialDrawCalls, int* totalDrawCalls)
{
    if (potentialDrawCalls) (*potentialDrawCalls)++;
    if (mCustomModelVertexCount > 0) {
        if (sphere.IsOnFrustum(frustum)) {
            if (totalDrawCalls) (*totalDrawCalls)++;
            mCustomModelVAO.Bind();
            shader.SetMat4("model", mModel);
            glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mCustomModelVertexCount));
        }
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