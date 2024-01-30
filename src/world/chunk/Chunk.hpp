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
#include <world/chunk/Constants.hpp>
#include <world/Block.hpp>
#include <math/Vector.hpp>
#include <math/Matrix.hpp>
#include <math/Frustum.hpp>
#include <PerlinNoise.hpp>
#include <vector>
#include <atomic>

std::size_t VoxelIndex(iVec3 pos);

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

    std::vector<BlockID> mBlocks;
    iVec3 mPos{};
    Mat4 mModel = Mat4::identity();
    Sphere sphere;
public:
    Chunk(iVec3 pos);
    iVec3 GetPosition() const;
    void AllocateMemory();
    void ReleaseMemory();
    void CreateMesh();
    void BufferData();
	const BlockID* GetBlockDataPointer() const;
    void Draw(const Frustum& frustum, Shader& shader, int* potentialDrawCalls, int* totalDrawCalls);
    void DrawWater(const Frustum& frustum, Shader& shader, int* potentialDrawCalls, int* totalDrawCalls);
    void DrawCustomModel(const Frustum& frustum, Shader& shader, int* potentialDrawCalls, int* totalDrawCalls);
    BlockID GetBlock(iVec3 pos) const;
    void SetBlock(iVec3 pos, BlockID block);
    std::atomic<bool> needsBuffering = false;
    bool loaded = false;
    float firstBufferTime = 0.0f;
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
