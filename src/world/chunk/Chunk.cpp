/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include <world/chunk/Chunk.hpp>
#include <world/Block.hpp>
#include <GLFW/glfw3.h>
#include <cmath>
#include <util/Log.hpp>
#include <glm/gtc/matrix_transform.hpp>

std::size_t VoxelIndex(glm::ivec3 pos) {
    return pos.y + (pos.x << Chunk::SIZE_PADDED_LOG_2) + (pos.z << Chunk::SIZE_PADDED_SQUARED_LOG_2);
}

Chunk::Chunk(glm::ivec3 pos) : mPos(pos)
{
    // Setup buffers
    VertexBufferLayout bufferLayout;
    bufferLayout.AddAttribute<unsigned int>(2);

    mVAO.AddBuffer(mVBO, bufferLayout);
    mWaterVAO.AddBuffer(mWaterVBO, bufferLayout);
    mCustomModelVAO.AddBuffer(mCustomModelVBO, bufferLayout);

    // Transform it to its global position
    glm::vec3 globalPosition = static_cast<glm::vec3>(pos * Chunk::SIZE);
    mModel *= glm::translate(mModel, globalPosition);

    // Sphere for frustum culling
    sphere = Sphere{ 
        globalPosition + glm::vec3(static_cast<float>(Chunk::HALF_SIZE)), 
        glm::length(glm::vec3(static_cast<float>(Chunk::HALF_SIZE)))
    };
}

glm::ivec3 Chunk::GetPosition() const
{
    return mPos;
}

void Chunk::AllocateMemory()
{
    mBlocks.resize(Chunk::SIZE_PADDED_CUBED, Block(BlockType::AIR, 0, false));
    allocated = true;
}

void Chunk::ReleaseMemory()
{
    decltype(mBlocks)().swap(mBlocks);
    allocated = false;
}

void Chunk::CreateMesh() {
    // Erase previous data!
    std::vector<ChunkMesher::ChunkVertex>().swap(mVertices);
    mVertexCount = 0;

    std::vector<ChunkMesher::ChunkVertex>().swap(mWaterVertices);
    mWaterVertexCount = 0;

    std::vector<ChunkMesher::ChunkVertex>().swap(mCustomModelVertices);
    mCustomModelVertexCount = 0;

    // Mesh
    ChunkMesher::BinaryGreedyMesh(mVertices, mBlocks, ChunkMesher::IsOpaqueCube);
    ChunkMesher::BinaryGreedyMesh(mVertices, mBlocks, [](Block block) { return block.GetType() == BlockType::GLASS; });
    ChunkMesher::BinaryGreedyMesh(mWaterVertices, mBlocks, [](Block block) { return block.GetType() == BlockType::WATER || block.IsWaterLogged(); });
    ChunkMesher::MeshCustomModelBlocks(mCustomModelVertices, mBlocks);
    needsBuffering = true;
}

void Chunk::BufferData()
{
    if (mVertices.size() > 0) {
        mVBO.BufferData(mVertices.data(), mVertices.size() * sizeof(ChunkMesher::ChunkVertex), GL_STATIC_DRAW);
        mVertexCount = mVertices.size();
    }
    std::vector<ChunkMesher::ChunkVertex>().swap(mVertices);

    if (mWaterVertices.size() > 0) {
        mWaterVBO.BufferData(mWaterVertices.data(), mWaterVertices.size() * sizeof(ChunkMesher::ChunkVertex), GL_STATIC_DRAW);
        mWaterVertexCount = mWaterVertices.size();
    }
    std::vector<ChunkMesher::ChunkVertex>().swap(mWaterVertices);

    if (mCustomModelVertices.size() > 0) {
        mCustomModelVBO.BufferData(mCustomModelVertices.data(), mCustomModelVertices.size() * sizeof(ChunkMesher::ChunkVertex), GL_STATIC_DRAW);
        mCustomModelVertexCount = mCustomModelVertices.size();
    }
    std::vector<ChunkMesher::ChunkVertex>().swap(mCustomModelVertices);

    needsBuffering = false;
}

void Chunk::UpdateVisiblity(const Frustum& frustum)
{
    visible = (mVertexCount > 0 && mWaterVertexCount > 0 && mCustomModelVertexCount > 0) || sphere.IsOnFrustum(frustum);
}

void Chunk::Draw(Shader& shader, int* potentialDrawCalls, int* totalDrawCalls)
{
    if (potentialDrawCalls) (*potentialDrawCalls)++;
    if (!visible || mVertexCount == 0) return;
    if (totalDrawCalls) (*totalDrawCalls)++;
    mVAO.Bind();
    shader.SetMat4("model", mModel);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mVertexCount));
}

void Chunk::DrawWater(Shader& shader, int* potentialDrawCalls, int* totalDrawCalls)
{
    if (potentialDrawCalls) (*potentialDrawCalls)++;
    if (!visible || mWaterVertexCount == 0) return;
    if (totalDrawCalls) (*totalDrawCalls)++;
    mWaterVAO.Bind();
    shader.SetMat4("model", mModel);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mWaterVertexCount));
}

void Chunk::DrawCustomModel(Shader& shader, int* potentialDrawCalls, int* totalDrawCalls)
{
    if (potentialDrawCalls) (*potentialDrawCalls)++;
    if (!visible || mCustomModelVertexCount == 0) return;
    if (totalDrawCalls) (*totalDrawCalls)++;
    mCustomModelVAO.Bind();
    shader.SetMat4("model", mModel);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(mCustomModelVertexCount));
}

Block Chunk::RawGetBlock(glm::ivec3 pos) const
{
    return mBlocks[VoxelIndex(pos)];
}

void Chunk::RawSetBlock(glm::ivec3 pos, Block block)
{
    mBlocks[VoxelIndex(pos)] = block;
    needsSaving = true;
}

Block Chunk::GetBlock(glm::ivec3 pos) const
{
    if (!allocated || pos.x < 0 || pos.x >= SIZE_PADDED || pos.y < 0 || pos.y >= SIZE_PADDED || pos.z < 0 || pos.z >= SIZE_PADDED) return Block(BlockType::AIR, 0, false);
    return mBlocks[VoxelIndex(pos)];
}

void Chunk::SetBlock(glm::ivec3 pos, Block block)
{
    if (!allocated || pos.x < 0 || pos.x >= SIZE_PADDED || pos.y < 0 || pos.y >= SIZE_PADDED || pos.z < 0 || pos.z >= SIZE_PADDED) return;
    mBlocks[VoxelIndex(pos)] = block;
    needsSaving = true;
}

Block* Chunk::GetBlockDataPointer() {
    return mBlocks.data();
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
