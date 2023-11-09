/*
Copyright (C) 2023 William Redding - All Rights Reserved
LICENSE: MIT
*/

#include <world/chunk/Chunk.hpp>
#include <opengl/VertexBufferLayout.hpp>
#include <cstring>
#include <fstream>
#include <util/Log.hpp>
#include <math/Math.hpp>
#include <GLFW/glfw3.h>
#include <world/chunk/ChunkRegion.hpp>

void engine::Chunk::AddVertexBufferAttributes() {
    VertexBufferLayout bufLayout;
	bufLayout.AddAttribute<unsigned int>(2);
    m_VAO.AddBuffer(m_VBO, bufLayout);
    m_WaterVAO.AddBuffer(m_WaterVBO, bufLayout);
    m_CustomModelVAO.AddBuffer(m_CustomModelVBO, bufLayout);
}

void engine::Chunk::SetupModelMatrix(Vec3<int> chunkPos) {
    m_Model = m_Model *  translate(Vec3<float>(static_cast<float>(chunkPos.x * CS), static_cast<float>(chunkPos.y * CS), static_cast<float>(chunkPos.z * CS)));
}

engine::Chunk::Chunk(): m_Pos(0,0,0){
    SetupModelMatrix(m_Pos);
    AddVertexBufferAttributes();
}

engine::Chunk::Chunk(Vec3<int> chunkPos): m_Pos(chunkPos.x, chunkPos.y, chunkPos.z)
{
    
    SetupModelMatrix(m_Pos);
    AddVertexBufferAttributes();
}

engine::Chunk::~Chunk() {
    delete[] m_Voxels;
}

engine::Chunk::Chunk(Chunk&& other): m_Pos(std::move(other.m_Pos)), m_Model(std::move(other.m_Model)),
    m_VBO(std::move(other.m_VBO)), m_VAO(std::move(other.m_VAO)), m_Vertices(std::move(other.m_Vertices)), m_VertexCount(std::move(other.m_VertexCount)),
    m_WaterVBO(std::move(other.m_WaterVBO)), m_WaterVAO(std::move(other.m_WaterVAO)), m_WaterVertices(std::move(other.m_WaterVertices)), m_WaterVertexCount(std::move(other.m_WaterVertexCount)),
    m_CustomModelVBO(std::move(other.m_CustomModelVBO)), m_CustomModelVAO(std::move(other.m_CustomModelVAO)), m_CustomModelVertices(std::move(other.m_CustomModelVertices)), m_CustomModelVertexCount(std::move(other.m_CustomModelVertexCount))
{  
    delete[] m_Voxels;
    m_Voxels = std::move(other.m_Voxels);
    other.m_Voxels = nullptr;
}

void engine::Chunk::CreateMesh()
{
    m_Vertices.clear();
    m_Vertices.reserve(CS_P3 * 3);
    m_VertexCount = 0;

    m_WaterVertices.clear();
    m_WaterVertices.reserve(CS_P3 * 3);
    m_WaterVertexCount = 0;

    m_CustomModelVertices.clear();
    m_CustomModelVertices.reserve(CS_P3 * 3);
    m_CustomModelVertexCount = 0;
    
    GreedyTranslucent(m_Vertices, m_WaterVertices, m_Voxels);
    GreedyOpaque(m_Vertices, m_Voxels);
    MeshCustomModelBlocks(m_CustomModelVertices, m_Voxels);

    m_VertexCount = m_Vertices.size();
    m_WaterVertexCount = m_WaterVertices.size();
    m_CustomModelVertexCount = m_CustomModelVertices.size();
}

void engine::Chunk::BufferData()
{
    if (m_VertexCount > 0){
        m_VBO.BufferData(m_Vertices.data(), m_VertexCount * sizeof(CubeChunkVertex), GL_STATIC_DRAW);
        std::vector<CubeChunkVertex>().swap(m_Vertices);
    }

    if (m_WaterVertexCount > 0){
        m_WaterVBO.BufferData(m_WaterVertices.data(), m_WaterVertexCount * sizeof(CubeChunkVertex), GL_STATIC_DRAW);
        std::vector<CubeChunkVertex>().swap(m_WaterVertices);
    }

    if (m_CustomModelVertexCount > 0) {
        m_CustomModelVBO.BufferData(m_CustomModelVertices.data(), m_CustomModelVertexCount * sizeof(CustomModelChunkVertex), GL_STATIC_DRAW);
        std::vector<CustomModelChunkVertex>().swap(m_CustomModelVertices);
    }

    buffered = true;
}

engine::Vec3<int> engine::Chunk::GetPos() const {
    return m_Pos;
}

void engine::Chunk::DrawOpaque(Shader& shader)
{
	if (m_VertexCount > 0 && buffered) {
		m_VAO.Bind();
		shader.setMat4("model", m_Model);
		glDrawArrays(GL_TRIANGLES, 0, m_VertexCount);
	}
}

void engine::Chunk::DrawWater(Shader& shader)
{
	if (m_WaterVertexCount > 0 && buffered) {
		m_WaterVAO.Bind();
		shader.setMat4("model", m_Model);
		glDrawArrays(GL_TRIANGLES, 0, m_WaterVertexCount);
    }
}

void engine::Chunk::DrawCustomModelBlocks(Shader& shader)
{
	if (m_CustomModelVertexCount > 0 && buffered) {
		m_CustomModelVAO.Bind();
		shader.setMat4("model", m_Model);
        glDrawArrays(GL_TRIANGLES, 0, m_CustomModelVertexCount);
    }
}

void engine::Chunk::CopyNeighborData(ChunkRegion* region) {
    Vec3<int> localChunkPos = m_Pos % CHUNK_REGION_SIZE;
    if (localChunkPos.x < 0){
        localChunkPos.x = CHUNK_REGION_SIZE - (localChunkPos.x * -1);
    }
    if (localChunkPos.y < 0){
        localChunkPos.y = CHUNK_REGION_SIZE - (localChunkPos.y * -1);
    }
    if (localChunkPos.z < 0){
        localChunkPos.z = CHUNK_REGION_SIZE - (localChunkPos.z * -1);
    }
    Chunk* belowChunk = region->GetChunk(localChunkPos + Vec3<int>(0, -1, 0));
    Chunk* aboveChunk = region->GetChunk(localChunkPos + Vec3<int>(0, 1, 0));
    Chunk* leftChunk = region->GetChunk(localChunkPos + Vec3<int>(-1, 0, 0));
    Chunk* rightChunk = region->GetChunk(localChunkPos + Vec3<int>(1, 0, 0));
    Chunk* frontChunk = region->GetChunk(localChunkPos + Vec3<int>(0, 0, -1));
    Chunk* backChunk = region->GetChunk(localChunkPos + Vec3<int>(0, 0, 1));

    if (belowChunk != nullptr) {
        for (int x = 1; x < CS_P_MINUS_ONE; x++) {
            for (int z = 1; z < CS_P_MINUS_ONE; z++) {
                SetBlock(belowChunk->GetBlock(Vec3<int>(x, CS, z)), Vec3<int>(x, 0, z));
            }
        }
    }

    if (aboveChunk != nullptr) {
        for (int x = 1; x < CS_P_MINUS_ONE; x++) {
            for (int z = 1; z < CS_P_MINUS_ONE; z++) {
                SetBlock(aboveChunk->GetBlock(Vec3<int>(x, 1, z)), Vec3<int>(x, CS_P_MINUS_ONE, z));
            }
        }
    }

    if (leftChunk != nullptr) {
        for (int y = 1; y < CS_P_MINUS_ONE; y++) {
            for (int z = 1; z < CS_P_MINUS_ONE; z++) {
                SetBlock(leftChunk->GetBlock(Vec3<int>(CS, y, z)), Vec3<int>(0, y, z));
            }
        }
    }

    if (rightChunk != nullptr) {
        for (int y = 1; y < CS_P_MINUS_ONE; y++) {
            for (int z = 1; z < CS_P_MINUS_ONE; z++) {
                SetBlock(rightChunk->GetBlock(Vec3<int>(1, y, z)), Vec3<int>(CS_P_MINUS_ONE, y, z));
            }
        }
    }
    
    if (frontChunk != nullptr) {
        for (int x = 1; x < CS_P_MINUS_ONE; x++) {
            for (int y = 1; y < CS_P_MINUS_ONE; y++) {
                SetBlock(frontChunk->GetBlock(Vec3<int>(x, y, CS)), Vec3<int>(x, y, 0));
            }
        }
    }

    if (backChunk != nullptr) {
        for (int x = 1; x < CS_P_MINUS_ONE; x++) {
            for (int y = 1; y < CS_P_MINUS_ONE; y++) {
                SetBlock(backChunk->GetBlock(Vec3<int>(x, y, 1)), Vec3<int>(x, y, CS_P_MINUS_ONE));
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