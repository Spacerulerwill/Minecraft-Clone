/*
Copyright (C) 2023 William Redding - All Rights Reserved
LICENSE: MIT
*/

#include <world/ChunkMesher.hpp>
#include <world/Chunk.hpp>

#ifdef _MSC_VER
#include <intrin.h>
inline const int CTZ(uint64_t &x) {
  unsigned long index;
  _BitScanForward64(&index, x);
  return static_cast<int>(index);
}
#else
inline const int CTZ(uint64_t x) {
  return __builtin_ctzll(x);
}
#endif

inline const bool SolidCheck(engine::BlockInt voxel) {
    return voxel > 0;
}

inline const int GetAxisI(const int &axis, const int &a, const int &b, const int &c) {
    if (axis == 0) return b + (a * CS_P) + (c * CS_P2);
    else if (axis == 1) return a + (c * CS_P) + (b* CS_P2);
    else return c + (b * CS_P) + (a * CS_P2);
}

inline const bool CompareForward(const engine::BlockInt* voxels, int axis, int forward, int right, int bit_pos) {
    return voxels[GetAxisI(axis, right, forward, bit_pos)] == voxels[GetAxisI(axis, right, forward + 1, bit_pos)];
}

inline const bool CompareRight(const engine::BlockInt* voxels, int axis, int forward, int right, int bit_pos) {
    return voxels[GetAxisI(axis, right, forward, bit_pos)] == voxels[GetAxisI(axis, right + 1, forward, bit_pos)];
}

inline const void InsertQuad(std::vector<engine::ChunkVertex>& vertices, engine::ChunkVertex v1, engine::ChunkVertex v2, engine::ChunkVertex v3, engine::ChunkVertex v4) {
    vertices.insert(vertices.end(), { v1, v2, v4, v2, v3, v4 });
}

void engine::GreedyTranslucent(std::vector<engine::ChunkVertex>& vertices, std::vector<engine::ChunkVertex>& waterVertices, const engine::BlockInt* voxels) {
  uint64_t axis_cols[CS_P2 * 3] = { 0 };
  uint64_t col_face_masks[CS_P2 * 6];

  // Step 1: Convert to binary representation for each direction
  int index = 0;
  for (int y = 0; y < CS_P; y++) {
    for (int x = 0; x < CS_P; x++) {
      uint64_t zb = 0;
      for (int z = 0; z < CS_P; z++) {
        if (SolidCheck(voxels[index]) && BlockData[voxels[index]].model == CUBE) {
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

          if (CompareForward(voxels, axis, forward, right, bit_pos)) {
            merged_forward[(right * CS_P) + bit_pos]++;
          }
          else {
            bits_merging_forward &= ~(1ULL << bit_pos);
          }
        }

        uint64_t bits_stopped_forward = bits_here & ~bits_merging_forward;
        while (bits_stopped_forward) {
          int bit_pos = CTZ(bits_stopped_forward);
          bits_stopped_forward &= ~(1ULL << bit_pos);

          // Discards faces from neighbor voxels
          if (bit_pos == 0 || bit_pos == CS_P - 1 ) { continue; };

          if (
            (bits_merging_right & (1ULL << bit_pos)) != 0 &&
            merged_forward[(right * CS_P) + bit_pos] == merged_forward[(right + 1) * CS_P + bit_pos] &&
            CompareRight(voxels, axis, forward, right, bit_pos))
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

          uint8_t type = voxels[GetAxisI(axis, right, forward, bit_pos)];  
          BlockDataStruct blockData = BlockData[type];    
          
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
          if (type == WATER) {
            InsertQuad(waterVertices, v1, v2, v3, v4);
          }else {
            InsertQuad(vertices, v1, v2, v3, v4);
          }
        }
      }
    }
  }
}

void engine::GreedyOpaque(std::vector<engine::ChunkVertex>& vertices, const engine::BlockInt* voxels) {
  uint64_t axis_cols[CS_P2 * 3] = { 0 };
  uint64_t col_face_masks[CS_P2 * 6];

  // Step 1: Convert to binary representation for each direction
  int index = 0;
  for (int y = 0; y < CS_P; y++) {
    for (int x = 0; x < CS_P; x++) {
      uint64_t zb = 0;
      for (int z = 0; z < CS_P; z++) {
        BlockDataStruct blockData = BlockData[voxels[index]];
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

          if (CompareForward(voxels, axis, forward, right, bit_pos)) {
            merged_forward[(right * CS_P) + bit_pos]++;
          }
          else {
            bits_merging_forward &= ~(1ULL << bit_pos);
          }
        }

        uint64_t bits_stopped_forward = bits_here & ~bits_merging_forward;
        while (bits_stopped_forward) {
          int bit_pos = CTZ(bits_stopped_forward);
          bits_stopped_forward &= ~(1ULL << bit_pos);

          // Discards faces from neighbor voxels
          if (bit_pos == 0 || bit_pos == CS_P - 1) { continue; };

          if (
            (bits_merging_right & (1ULL << bit_pos)) != 0 &&
            merged_forward[(right * CS_P) + bit_pos] == merged_forward[(right + 1) * CS_P + bit_pos] &&
            CompareRight(voxels, axis, forward, right, bit_pos))
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

          uint8_t type = voxels[GetAxisI(axis, right, forward, bit_pos)];  
          BlockDataStruct blockData = BlockData[type];    

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
          InsertQuad(vertices, v1, v2, v3, v4);
        }
      }
    }
  }
}

void engine::MeshCustomModelBlocks(std::vector<float>& vertices, const engine::BlockInt* voxels) {
    for (int x = 1; x < CS_P_MINUS_ONE; x++){
        for (int y = 1; y < CS_P_MINUS_ONE; y++) {
            for (int z =1; z < CS_P_MINUS_ONE; z++) {
                BlockDataStruct blockData = BlockData[voxels[VOXEL_INDEX(x,y,z)]];
                if (blockData.model != CUBE && (
                    !BlockData[voxels[VOXEL_INDEX(x+1,y,z)]].opaque ||
                    !BlockData[voxels[VOXEL_INDEX(x-1,y,z)]].opaque ||
                    !BlockData[voxels[VOXEL_INDEX(x,y+1,z)]].opaque ||
                    !BlockData[voxels[VOXEL_INDEX(x,y-1,z)]].opaque ||
                    !BlockData[voxels[VOXEL_INDEX(x,y,z+1)]].opaque ||
                    !BlockData[voxels[VOXEL_INDEX(x,y,z-1)]].opaque
                )) {
                    BlockModelStruct modelData = BlockModelData[blockData.model];
                    std::vector<float> modelVerts(modelData.begin, modelData.end);
                    for (int i = 0; i < modelVerts.size() - 1; i += 6) {
                        modelVerts[i] += x-1;
                        modelVerts[i+1] += y-1;
                        modelVerts[i+2] += z-1;
                        modelVerts[i+5] = blockData.top_face;
                    }
                    vertices.insert(vertices.end(), modelVerts.begin(), modelVerts.end());
                }
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
