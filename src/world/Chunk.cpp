/*
Copyright (C) 2023 William Redding - All Rights Reserved
LICENSE: MIT
*/

#include <world/Chunk.hpp>
#include <opengl/VertexBufferLayout.hpp>
#include <cstring>
#include <util/Log.hpp>
#include <math/Math.hpp>
#include <chrono>
#include <algorithm>

using namespace std::chrono;

#ifdef _MSC_VER
inline const int CTZ(uint64_t &x) {
  unsigned long index;
  _BitScanForward64(&index, x);
  return (int)index;
}
#else
inline const int CTZ(uint64_t x) {
  return __builtin_ctzll(x);
}
#endif

engine::Chunk::Chunk(int chunkX, int chunkY, int chunkZ): chunkX(chunkX), chunkY(chunkY), chunkZ(chunkZ)
{
    m_Model *= translate(Vec3(chunkX * CS, chunkY * CS, chunkZ * CS));
    memset(m_Voxels, AIR, sizeof(BlockInt) * CS_P3);

    VertexBufferLayout bufLayout;

	  bufLayout.AddAttribute<unsigned int>(2);
    m_VAO.AddBuffer(m_VBO, bufLayout);
    m_WaterVAO.AddBuffer(m_WaterVBO, bufLayout);
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
            SetBlock(ROSE, x, height+1, z);
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
    
    GreedyTranslucent();
    GreedyOpaque();
    MeshCustomModelBlocks();

    m_VertexCount = m_Vertices.size();
    m_WaterVertexCount = m_WaterVertices.size();
}

void engine::Chunk::GreedyTranslucent() {
  uint64_t axis_cols[CS_P2 * 3] = { 0 };
  uint64_t col_face_masks[CS_P2 * 6];

  // Step 1: Convert to binary representation for each direction
  int index = 0;
  for (int y = 0; y < CS_P; y++) {
    for (int x = 0; x < CS_P; x++) {
      uint64_t zb = 0;
      for (int z = 0; z < CS_P; z++) {
        if (SolidCheck(m_Voxels[index]) && BlockHandler::BlockData[m_Voxels[index]].model == CUBE) {
          axis_cols[x + (z * CS_P)] |= 1ULL << y;
          axis_cols[z + (y * CS_P) + (CS_P2)] |= 1ULL << x;
          zb |= 1ULL << z;
        }
        index++;
      }
      axis_cols[y + (x * CS_P) + (CS_P2 * 2)] = zb;
    }
  }

  // Step 2: Visible face culling
  for (int axis = 0; axis <= 2; axis++) {
    for (int i = 0; i < CS_P2; i++) {
      uint64_t col = axis_cols[(CS_P2 * axis) + i];
      col_face_masks[(CS_P2 * (axis * 2)) + i] = col & ~((col >> 1) | (1ULL << (CS_P - 1)));
      col_face_masks[(CS_P2 * (axis * 2 + 1)) + i] = col & ~((col << 1) | 1ULL);
    }
  }
  
  // Step 3: Greedy meshing
  for (int face = 0; face < 6; face++) {
    int axis = face / 2;

    int merged_forward[CS_P2] = { 0 };
    for (int forward = 1; forward < CS_P - 1; forward++) {
      uint64_t bits_walking_right = 0;
      int merged_right[CS_P] = { 0 };
      for (int right = 1; right < CS_P - 1; right++) {
        uint64_t bits_here = col_face_masks[right + (forward * CS_P) + (face * CS_P2)];
        uint64_t bits_forward = forward >= CS ? 0 : col_face_masks[right + (forward * CS_P) + (face * CS_P2) + CS_P];
        uint64_t bits_right = right >= CS ? 0 : col_face_masks[right + 1 + (forward * CS_P) + (face * CS_P2)];
        uint64_t bits_merging_forward = bits_here & bits_forward & ~bits_walking_right;
        uint64_t bits_merging_right = bits_here & bits_right;

        uint64_t copy_front = bits_merging_forward;

        while (copy_front) {
          int bit_pos = CTZ(copy_front);
          copy_front &= ~(1ULL << bit_pos);

          if (bit_pos == 0 || bit_pos == CS_P - 1) continue;

          if (CompareForward(axis, forward, right, bit_pos)) {
            merged_forward[(right * CS_P) + bit_pos]++;
          }
          else {
            bits_merging_forward &= ~(1ULL << bit_pos);
          }
        }

        uint64_t bits_stopped_forward = bits_here & ~bits_merging_forward;
        while (bits_stopped_forward) {
          bool addFace;
          int bit_pos = CTZ(bits_stopped_forward);
          bits_stopped_forward &= ~(1ULL << bit_pos);

          // Discards faces from neighbor voxels
          if (bit_pos == 0 || bit_pos == CS_P - 1 ) { continue; };

          if (
            (bits_merging_right & (1ULL << bit_pos)) != 0 &&
            merged_forward[(right * CS_P) + bit_pos] == merged_forward[(right + 1) * CS_P + bit_pos] &&
            CompareRight(axis, forward, right, bit_pos))
          {
            bits_walking_right |= 1ULL << bit_pos;
            merged_right[bit_pos]++;
            merged_forward[(right * CS_P) + bit_pos] = 0;
            continue;
          }
          bits_walking_right &= ~(1ULL << bit_pos);

          uint8_t mesh_left = right - merged_right[bit_pos];
          uint8_t mesh_right = right + 1;
          uint8_t mesh_front = forward - merged_forward[(right * CS_P) + bit_pos];
          uint8_t mesh_back = forward + 1; 
          uint8_t mesh_up = bit_pos + (face % 2 == 0 ? 1 : 0);

          uint8_t type = m_Voxels[GetAxisI(axis, right, forward, bit_pos)];  
          BlockDataStruct blockData = BlockHandler::BlockData[type];    
          
          merged_forward[(right * CS_P) + bit_pos] = 0;
          merged_right[bit_pos] = 0;

          if (blockData.opaque || blockData.model != CUBE) {
            continue;
          }

          ChunkVertex v1, v2, v3, v4;

          switch(face) {
            case 0: {
              TextureInt texZ = blockData.top_face;
              v1 = GetVertex(mesh_left, mesh_up, mesh_front, 0, 0, texZ, face);
              v2 = GetVertex(mesh_left, mesh_up, mesh_back, 0, mesh_back - mesh_front, texZ, face);
              v3 = GetVertex(mesh_right, mesh_up, mesh_back, mesh_right-mesh_left, mesh_back-mesh_front, texZ, face);
              v4 = GetVertex(mesh_right, mesh_up, mesh_front, mesh_right-mesh_left, 0, texZ, face);
              break;
            }
            case 1: {
              TextureInt texZ = blockData.bottom_face;
              v1 = GetVertex(mesh_left, mesh_up, mesh_back, 0, mesh_back-mesh_front, texZ, face);
              v2 = GetVertex(mesh_left, mesh_up, mesh_front, 0, 0, texZ, face);
              v3 = GetVertex(mesh_right, mesh_up, mesh_front, mesh_right-mesh_left, 0, texZ, face);
              v4 = GetVertex(mesh_right, mesh_up, mesh_back, mesh_right-mesh_left, mesh_back-mesh_front, texZ, face);
              break;
            }
            case 2: { 
              TextureInt texZ = blockData.right_face;
              v1 = GetVertex(mesh_up, mesh_front, mesh_left, 0, mesh_back - mesh_front, texZ, face);
              v2 = GetVertex(mesh_up, mesh_back, mesh_left, 0, 0, texZ, face);
              v3 = GetVertex(mesh_up, mesh_back, mesh_right, mesh_right - mesh_left, 0, texZ, face);
              v4 = GetVertex(mesh_up, mesh_front, mesh_right,  mesh_right - mesh_left, mesh_back - mesh_front, texZ, face);
              break;
            }
            case 3: {
              TextureInt texZ = blockData.left_face;
              v1 = GetVertex(mesh_up, mesh_back, mesh_left, 0, 0, texZ, face);
              v2 = GetVertex(mesh_up, mesh_front, mesh_left, 0, mesh_back - mesh_front, texZ, face);
              v3 = GetVertex(mesh_up, mesh_front, mesh_right, mesh_right - mesh_left, mesh_back - mesh_front, texZ, face);
              v4 = GetVertex(mesh_up, mesh_back, mesh_right,  mesh_right - mesh_left, 0, texZ, face);
              break;
            }
            case 4: {
              TextureInt texZ = blockData.back_face;
              v1 = GetVertex(mesh_front, mesh_left, mesh_up, 0, mesh_right - mesh_left, texZ, face);
              v2 = GetVertex(mesh_back, mesh_left, mesh_up, mesh_back - mesh_front, mesh_right - mesh_left, texZ, face);
              v3 = GetVertex(mesh_back, mesh_right, mesh_up, mesh_back - mesh_front, 0, texZ, face);
              v4 = GetVertex(mesh_front, mesh_right, mesh_up, 0, 0, texZ, face);
              break;
            }
            case 5: {
              TextureInt texZ = blockData.front_face;
              v1 = GetVertex(mesh_back, mesh_left, mesh_up, 0, mesh_right - mesh_left, texZ, face);
              v2 = GetVertex(mesh_front, mesh_left, mesh_up, mesh_back - mesh_front, mesh_right - mesh_left, texZ, face);
              v3 = GetVertex(mesh_front, mesh_right, mesh_up, mesh_back - mesh_front, 0, texZ, face);
              v4 = GetVertex(mesh_back, mesh_right, mesh_up, 0, 0, texZ, face);
              break;
            }
          }
          InsertQuad(type == WATER, v1, v2, v3, v4);
        }
        
      }
    }
  }
}

void engine::Chunk::GreedyOpaque() {
  uint64_t axis_cols[CS_P2 * 3] = { 0 };
  uint64_t col_face_masks[CS_P2 * 6];

  // Step 1: Convert to binary representation for each direction
  int index = 0;
  for (int y = 0; y < CS_P; y++) {
    for (int x = 0; x < CS_P; x++) {
      uint64_t zb = 0;
      for (int z = 0; z < CS_P; z++) {
        BlockDataStruct blockData = BlockHandler::BlockData[m_Voxels[index]];
        if (blockData.opaque && blockData.model == CUBE) {
          axis_cols[x + (z * CS_P)] |= 1ULL << y;
          axis_cols[z + (y * CS_P) + (CS_P2)] |= 1ULL << x;
          zb |= 1ULL << z;
        }
        index++;
      }
      axis_cols[y + (x * CS_P) + (CS_P2 * 2)] = zb;
    }
  }

  // Step 2: Visible face culling
  for (int axis = 0; axis <= 2; axis++) {
    for (int i = 0; i < CS_P2; i++) {
      uint64_t col = axis_cols[(CS_P2 * axis) + i];
      col_face_masks[(CS_P2 * (axis * 2)) + i] = col & ~((col >> 1) | (1ULL << (CS_P - 1)));
      col_face_masks[(CS_P2 * (axis * 2 + 1)) + i] = col & ~((col << 1) | 1ULL);
    }
  }
  
  // Step 3: Greedy meshing
  for (int face = 0; face < 6; face++) {
    int axis = face / 2;

    int merged_forward[CS_P2] = { 0 };
    for (int forward = 1; forward < CS_P - 1; forward++) {
      uint64_t bits_walking_right = 0;
      int merged_right[CS_P] = { 0 };
      for (int right = 1; right < CS_P - 1; right++) {
        uint64_t bits_here = col_face_masks[right + (forward * CS_P) + (face * CS_P2)];
        uint64_t bits_forward = forward >= CS ? 0 : col_face_masks[right + (forward * CS_P) + (face * CS_P2) + CS_P];
        uint64_t bits_right = right >= CS ? 0 : col_face_masks[right + 1 + (forward * CS_P) + (face * CS_P2)];
        uint64_t bits_merging_forward = bits_here & bits_forward & ~bits_walking_right;
        uint64_t bits_merging_right = bits_here & bits_right;

        uint64_t copy_front = bits_merging_forward;

        while (copy_front) {
          int bit_pos = CTZ(copy_front);
          copy_front &= ~(1ULL << bit_pos);

          if (bit_pos == 0 || bit_pos == CS_P - 1 ) continue;

          if (CompareForward(axis, forward, right, bit_pos)) {
            merged_forward[(right * CS_P) + bit_pos]++;
          }
          else {
            bits_merging_forward &= ~(1ULL << bit_pos);
          }
        }

        uint64_t bits_stopped_forward = bits_here & ~bits_merging_forward;
        while (bits_stopped_forward) {
          bool addFace;
          int bit_pos = CTZ(bits_stopped_forward);
          bits_stopped_forward &= ~(1ULL << bit_pos);

          // Discards faces from neighbor voxels
          if (bit_pos == 0 || bit_pos == CS_P - 1) { continue; };

          if (
            (bits_merging_right & (1ULL << bit_pos)) != 0 &&
            merged_forward[(right * CS_P) + bit_pos] == merged_forward[(right + 1) * CS_P + bit_pos] &&
            CompareRight(axis, forward, right, bit_pos))
          {
            bits_walking_right |= 1ULL << bit_pos;
            merged_right[bit_pos]++;
            merged_forward[(right * CS_P) + bit_pos] = 0;
            continue;
          }
          bits_walking_right &= ~(1ULL << bit_pos);

          uint8_t mesh_left = right - merged_right[bit_pos];
          uint8_t mesh_right = right + 1;
          uint8_t mesh_front = forward - merged_forward[(right * CS_P) + bit_pos];
          uint8_t mesh_back = forward + 1; 
          uint8_t mesh_up = bit_pos + (face % 2 == 0 ? 1 : 0);

          uint8_t type = m_Voxels[GetAxisI(axis, right, forward, bit_pos)];  
          BlockDataStruct blockData = BlockHandler::BlockData[type];    

          merged_forward[(right * CS_P) + bit_pos] = 0;
          merged_right[bit_pos] = 0;

          if (!blockData.opaque) { 
            continue;
          }
          ChunkVertex v1, v2, v3, v4;

          switch(face) {
            case 0: {
              TextureInt texZ = blockData.top_face;
              v1 = GetVertex(mesh_left, mesh_up, mesh_front, 0, 0, texZ, face);
              v2 = GetVertex(mesh_left, mesh_up, mesh_back, 0, mesh_back - mesh_front, texZ, face);
              v3 = GetVertex(mesh_right, mesh_up, mesh_back, mesh_right-mesh_left, mesh_back-mesh_front, texZ, face);
              v4 = GetVertex(mesh_right, mesh_up, mesh_front, mesh_right-mesh_left, 0, texZ, face);
              break;
            }
            case 1: {
              TextureInt texZ = blockData.bottom_face;
              v1 = GetVertex(mesh_left, mesh_up, mesh_back, 0, mesh_back-mesh_front, texZ, face);
              v2 = GetVertex(mesh_left, mesh_up, mesh_front, 0, 0, texZ, face);
              v3 = GetVertex(mesh_right, mesh_up, mesh_front, mesh_right-mesh_left, 0, texZ, face);
              v4 = GetVertex(mesh_right, mesh_up, mesh_back, mesh_right-mesh_left, mesh_back-mesh_front, texZ, face);
              break;
            }
            case 2: { 
              TextureInt texZ = blockData.right_face;
              v1 = GetVertex(mesh_up, mesh_front, mesh_left, 0, mesh_back - mesh_front, texZ, face);
              v2 = GetVertex(mesh_up, mesh_back, mesh_left, 0, 0, texZ, face);
              v3 = GetVertex(mesh_up, mesh_back, mesh_right, mesh_right - mesh_left, 0, texZ, face);
              v4 = GetVertex(mesh_up, mesh_front, mesh_right,  mesh_right - mesh_left, mesh_back - mesh_front, texZ, face);
              break;
            }
            case 3: {
              TextureInt texZ = blockData.left_face;
              v1 = GetVertex(mesh_up, mesh_back, mesh_left, 0, 0, texZ, face);
              v2 = GetVertex(mesh_up, mesh_front, mesh_left, 0, mesh_back - mesh_front, texZ, face);
              v3 = GetVertex(mesh_up, mesh_front, mesh_right, mesh_right - mesh_left, mesh_back - mesh_front, texZ, face);
              v4 = GetVertex(mesh_up, mesh_back, mesh_right,  mesh_right - mesh_left, 0, texZ, face);
              break;
            }
            case 4: {
              TextureInt texZ = blockData.back_face;
              v1 = GetVertex(mesh_front, mesh_left, mesh_up, 0, mesh_right - mesh_left, texZ, face);
              v2 = GetVertex(mesh_back, mesh_left, mesh_up, mesh_back - mesh_front, mesh_right - mesh_left, texZ, face);
              v3 = GetVertex(mesh_back, mesh_right, mesh_up, mesh_back - mesh_front, 0, texZ, face);
              v4 = GetVertex(mesh_front, mesh_right, mesh_up, 0, 0, texZ, face);
              break;
            }
            case 5: {
              TextureInt texZ = blockData.front_face;
              v1 = GetVertex(mesh_back, mesh_left, mesh_up, 0, mesh_right - mesh_left, texZ, face);
              v2 = GetVertex(mesh_front, mesh_left, mesh_up, mesh_back - mesh_front, mesh_right - mesh_left, texZ, face);
              v3 = GetVertex(mesh_front, mesh_right, mesh_up, mesh_back - mesh_front, 0, texZ, face);
              v4 = GetVertex(mesh_back, mesh_right, mesh_up, 0, 0, texZ, face);
              break;
            }
          }
          InsertQuad(false, v1, v2, v3, v4);
        }
      }
    }
  }
}

void engine::Chunk::MeshCustomModelBlocks() {
 
}

void engine::Chunk::BufferData()
{
    if (m_Vertices.size() > 0){
      m_VBO.BufferData(m_Vertices.data(), m_VertexCount * sizeof(ChunkVertex));
      m_VBO.Unbind();
    }
    
    m_Vertices.clear();

    if (m_WaterVertices.size() > 0){
      m_WaterVBO.BufferData(m_WaterVertices.data(), m_WaterVertexCount * sizeof(ChunkVertex));
      m_WaterVBO.Unbind();
    }
    m_WaterVertices.clear();
}

void engine::Chunk::Draw(Shader& shader)
{
    m_VAO.Bind();
    shader.setMat4("model", m_Model);
    glDrawArrays(GL_TRIANGLES, 0, m_VertexCount);
}

void engine::Chunk::DrawWater(Shader& shader)
{
    m_WaterVAO.Bind();
    shader.setMat4("model", m_Model);
    glDrawArrays(GL_TRIANGLES, 0, m_WaterVertexCount);
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