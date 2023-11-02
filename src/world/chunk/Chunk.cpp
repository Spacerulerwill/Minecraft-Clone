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

void engine::Chunk::AddVertexBufferAttributes() {
    VertexBufferLayout bufLayout;
	bufLayout.AddAttribute<unsigned int>(2);
    m_VAO.AddBuffer(m_VBO, bufLayout);
    m_WaterVAO.AddBuffer(m_WaterVBO, bufLayout);
    m_CustomModelVAO.AddBuffer(m_CustomModelVBO, bufLayout);
}

engine::Chunk::Chunk(): m_Pos(0,0,0){
    AddVertexBufferAttributes();
}

engine::Chunk::Chunk(Vec3<int> chunkPos): m_Pos(chunkPos.x, chunkPos.y, chunkPos.z)
{
    m_Model *= translate(Vec3<float>(static_cast<float>(chunkPos.x * CS), static_cast<float>(chunkPos.y * CS), static_cast<float>(chunkPos.z * CS)));
    AddVertexBufferAttributes();
}

engine::Chunk::Chunk(int x, int y, int z): m_Pos(x, y, z)
{
    m_Model *= translate(Vec3<float>(static_cast<float>(x * CS), static_cast<float>(y * CS), static_cast<float>(z * CS)));
    AddVertexBufferAttributes();
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


engine::Chunk& engine::Chunk::operator=(Chunk&& other) {
    if(this != &other)
    {
        m_Pos = std::move(other.m_Pos);
        delete[] m_Voxels;
        m_Voxels = std::move(other.m_Voxels);
        m_Model = std::move(other.m_Model);

        m_VBO = std::move(other.m_VBO);
        m_VAO = std::move(other.m_VAO);
        m_Vertices = std::move(other.m_Vertices);
        m_VertexCount = std::move(other.m_VertexCount);

        m_WaterVBO = std::move(other.m_WaterVBO);
        m_WaterVAO = std::move(other.m_WaterVAO);
        m_WaterVertices = std::move(other.m_WaterVertices);
        m_WaterVertexCount = std::move(other.m_WaterVertexCount);

        m_CustomModelVBO = std::move(other.m_CustomModelVBO);
        m_CustomModelVAO = std::move(other.m_CustomModelVAO);
        m_CustomModelVertices = std::move(other.m_CustomModelVertices);
        m_CustomModelVertexCount = std::move(other.m_CustomModelVertexCount);

        other.m_Voxels = nullptr;
    }
    return *this;
}

engine::Chunk::~Chunk()
{
    delete[] m_Voxels;
}

void engine::Chunk::TerrainGen(const siv::PerlinNoise& perlin,std::mt19937& gen, std::uniform_int_distribution<>& distrib)
{
    const unsigned int water_level = 32;
  
    for (int x = 1; x < CS_P_MINUS_ONE; x++) {
        for (int z = 1; z < CS_P_MINUS_ONE; z++){
            float heightMultiplayer = perlin.octave2D_01((m_Pos.x * CS + x) * 0.0125 , (m_Pos.z * CS + z) * 0.0125, 4, 0.5);
            int height = 1 + (heightMultiplayer * CS_MINUS_ONE);

            int dirt_height = height - 1;
            int dirt_to_place = dirt_height < 3 ? dirt_height : 3;
            for (int y = height - dirt_to_place; y < height; y++){
                SetBlock(DIRT, x, y, z);
            }
            for (int y = 1; y <= dirt_height - dirt_to_place; y++){
                SetBlock(STONE, x, y, z);
            }
            
            if (height < water_level - 1) {
                
                float oceanFloorNoise = perlin.octave2D_01((m_Pos.x * CS + x) * 0.125 , (m_Pos.z * CS + z) * 0.125, 8, 0.1);

                if (oceanFloorNoise < 0.33) {
                    SetBlock(CLAY, x, height, z);
                }
                else if (oceanFloorNoise < 0.66) {
                    SetBlock(SAND, x, height, z);
                }
                else if (oceanFloorNoise < 1.0) {
                    SetBlock(GRAVEL, x, height, z);
                }
                for (int y = height + 1; y < water_level; y++){
                SetBlock(WATER,x,y,z);
                }
            } else {
                SetBlock(GRASS, x, height, z);

                int randInt = distrib(gen);
                if (height < CS_P_MINUS_ONE - 1) {
                    if (randInt < 20) {
                        SetBlock(TALL_GRASS, x, height+1, z);
                    }
                    else if (randInt < 28) {
                        SetBlock(ROSE, x, height + 1, z);

                    } 
                    else if (randInt < 30) {
                        SetBlock(PINK_TULIP, x, height+1, z);
                    }
                }
                if (height < CS_P_MINUS_ONE - 4) {
                    if (randInt == 69) {
                        for (int y = 0; y < 3; y++) {
                            SetBlock(OAK_LOG, x, height + y + 1, z);
                        }
                        SetBlock(OAK_LEAVES, x, height + 4, z);
                        SetBlock(OAK_LEAVES, x - 1, height + 3, z);
                        SetBlock(OAK_LEAVES, x + 1, height + 3, z );
                        SetBlock(OAK_LEAVES, x, height + 3, z - 1);
                        SetBlock(OAK_LEAVES, x, height + 3, z + 1);
                    }
                }
            }
        }
    }
}

void engine::Chunk::TerrainGen(BlockInt block)
{
	for (int x = 1; x < CS_P_MINUS_ONE; x++) {
		for (int y = 1; y < CS_P_MINUS_ONE; y++) {
			for (int z = 1; z < CS_P_MINUS_ONE; z++) {
				SetBlock(block, x, y, z);
			}
		}
	}
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
}

void engine::Chunk::Draw(Shader& shader)
{
	if (m_VertexCount > 0) {
		m_VAO.Bind();
		shader.setMat4("model", m_Model);
		glDrawArrays(GL_TRIANGLES, 0, m_VertexCount);
	}
}

void engine::Chunk::DrawWater(Shader& shader)
{
	if (m_WaterVertexCount > 0) {
		m_WaterVAO.Bind();
		shader.setMat4("model", m_Model);
		glDrawArrays(GL_TRIANGLES, 0, m_WaterVertexCount);
    }
}

void engine::Chunk::DrawCustomModelBlocks(Shader& shader)
{
	if (m_CustomModelVertexCount > 0) {
		m_CustomModelVAO.Bind();
		shader.setMat4("model", m_Model);
        glDrawArrays(GL_TRIANGLES, 0, m_CustomModelVertexCount);
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