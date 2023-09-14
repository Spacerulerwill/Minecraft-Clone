/*
Copyright (C) 2023 William Redding - All Rights Reserved
LICENSE: MIT
*/

#include <world/Chunk.hpp>
#include <opengl/VertexBufferLayout.hpp>
#include <cstring>
#include <util/Log.hpp>
#include <math/Math.hpp>
#include <algorithm>

engine::Chunk::Chunk(int chunkX, int chunkY, int chunkZ): chunkX(chunkX), chunkY(chunkY), chunkZ(chunkZ)
{
    m_Model *= translate(Vec3(chunkX * CS, chunkY * CS, chunkZ * CS));
    memset(m_Voxels, AIR, sizeof(BlockInt) * CS_P3);

    VertexBufferLayout bufLayout;
	bufLayout.AddAttribute<unsigned int>(2);
    m_VAO.AddBuffer(m_VBO, bufLayout);
    m_WaterVAO.AddBuffer(m_WaterVBO, bufLayout);

    VertexBufferLayout customModelBufLayout;
    customModelBufLayout.AddAttribute<float>(3);
    customModelBufLayout.AddAttribute<float>(3);
    m_CustomModelVAO.AddBuffer(m_CustomModelVBO, customModelBufLayout);
}

engine::Chunk::~Chunk()
{
    delete[] m_Voxels;
}

void engine::Chunk::TerrainGen(const siv::PerlinNoise& perlin)
{
  memset(m_Voxels, AIR, CS_P3);
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distrib(1, 100);
  const unsigned int water_level = 32;
  
  for (int x = 1; x < CS_P_MINUS_ONE; x++) {
      for (int z = 1; z < CS_P_MINUS_ONE; z++){
          float heightMultiplayer = perlin.octave2D_01((chunkX * CS + x) * 0.0125 , (chunkZ * CS + z) * 0.0125, 4, 0.5);
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
            SetBlock(DIRT, x, height, z);
            for (int y = height + 1; y < water_level; y++){
              SetBlock(WATER,x,y,z);
            }
          } else {
            SetBlock(GRASS, x, height, z);

            int randInt = distrib(gen);
            if (height < CS_P_MINUS_ONE) {
                if (randInt < 20) {
                    SetBlock(TALL_GRASS, x, height+1, z);
                }
                else if (randInt < 30) {
                    SetBlock(ROSE, x, height+1, z);
                }
            }
          }
      }
    }
}


void engine::Chunk::CreateMesh()
{
    m_Vertices.clear();
    //m_Vertices.reserve(CS_P3 * 3);
    m_VertexCount = 0;

    m_WaterVertices.clear();
    //m_WaterVertices.reserve(CS_P3 * 3);
    m_WaterVertexCount = 0;

    m_CustomModelVertices.clear();
    //m_CustomModelVertices.reserve(CS_P3 * 3);
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
        m_VBO.BufferData(m_Vertices.data(), m_VertexCount * sizeof(ChunkVertex));
        m_VBO.Unbind();
        m_Vertices.clear();
    }

    if (m_WaterVertexCount > 0){
        m_WaterVBO.BufferData(m_WaterVertices.data(), m_WaterVertexCount * sizeof(ChunkVertex));
        m_WaterVBO.Unbind();
        m_WaterVertices.clear();
    }

    if (m_CustomModelVertexCount > 0) {
        m_CustomModelVBO.BufferData(m_CustomModelVertices.data(), m_CustomModelVertexCount * sizeof(float));
        m_CustomModelVBO.Unbind();
        m_CustomModelVertices.clear();
    }
}

void engine::Chunk::Draw(Shader& shader)
{
    m_VAO.Bind();
    shader.setMat4("model", m_Model);

    if (m_VertexCount > 0) {
        glDrawArrays(GL_TRIANGLES, 0, m_VertexCount);
    }
}

void engine::Chunk::DrawWater(Shader& shader)
{
    m_WaterVAO.Bind();
    shader.setMat4("model", m_Model);

    if (m_WaterVertexCount > 0) {
        glDrawArrays(GL_TRIANGLES, 0, m_WaterVertexCount);
    }
}

void engine::Chunk::DrawCustomModelBlocks(Shader& shader)
{
    m_CustomModelVAO.Bind();
    shader.setMat4("model", m_Model);

    if (m_CustomModelVertexCount > 0) {
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