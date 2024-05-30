/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#ifndef CHUNK_H
#define CHUNK_H

#include <cstdint>
#include <opengl/VertexArray.hpp>
#include <opengl/BufferObject.hpp>
#include <opengl/Shader.hpp>
#include <world/chunk/ChunkMesher.hpp>
#include <world/Block.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <math/Frustum.hpp>
#include <PerlinNoise.hpp>
#include <vector>
#include <atomic>

std::size_t VoxelIndex(glm::ivec3 pos);

class Chunk {
private:
    VertexArray mVAO;
    VertexBuffer mVBO;
    std::vector<ChunkMesher::ChunkVertex> mVertices;
    std::size_t mVertexCount = 0;

    VertexArray mWaterVAO;
    VertexBuffer mWaterVBO;
    std::vector<ChunkMesher::ChunkVertex> mWaterVertices;
    std::size_t mWaterVertexCount = 0;

    VertexArray mCustomModelVAO;
    VertexBuffer mCustomModelVBO;
    std::vector<ChunkMesher::ChunkVertex> mCustomModelVertices;
    std::size_t mCustomModelVertexCount = 0;

    std::vector<Block> mBlocks;
    glm::ivec3 mPos{};
    glm::mat4 mModel = glm::mat4(1.0f);
    Sphere sphere;
public:
    static constexpr int SIZE_PADDED_LOG_2 = 6;
    static constexpr int SIZE_PADDED_SQUARED_LOG_2 = SIZE_PADDED_LOG_2 * 2;
    static constexpr int SIZE_PADDED = 1 << SIZE_PADDED_LOG_2;
    static constexpr int SIZE_PADDED_SQUARED = 1 << SIZE_PADDED_SQUARED_LOG_2;
    static constexpr int SIZE_PADDED_CUBED = SIZE_PADDED_SQUARED * SIZE_PADDED;
    static constexpr int SIZE_PADDED_SUB_1 = SIZE_PADDED - 1;
    static constexpr int SIZE = SIZE_PADDED - 2;
    static constexpr int HALF_SIZE = SIZE / 2;
    Chunk(glm::ivec3 pos);
    glm::ivec3 GetPosition() const;
    void AllocateMemory();
    void ReleaseMemory();
    void CreateMesh();
    void BufferData();
    void UpdateVisiblity(const Frustum& frustum);
    Block* GetBlockDataPointer();
    void Draw(Shader& shader, int* potentialDrawCalls, int* totalDrawCalls);
    void DrawWater(Shader& shader, int* potentialDrawCalls, int* totalDrawCalls);
    void DrawCustomModel(Shader& shader, int* potentialDrawCalls, int* totalDrawCalls);
    // Get block in chunk - does not perform boundary checks or check whether the chunk is allocated/loaded. Dangerous!
    Block RawGetBlock(glm::ivec3 pos) const;
    // Set block in chunk - does not perform boundary checks or check whether the chunk is allocated/loaded. Dangerous!
    void RawSetBlock(glm::ivec3 pos, Block block);
    // Get block in chunk with boundary checks and allocation check
    Block GetBlock(glm::ivec3 pos) const;
    // Set block in chunk with boundary checks and allocation check
    void SetBlock(glm::ivec3 pos, Block block);
    std::atomic<bool> needsBuffering = false;
    std::atomic<bool> needsSaving = false;
    std::atomic<bool> allocated = false;
    bool visible = true;
};

#endif // !CHUNK_H

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
