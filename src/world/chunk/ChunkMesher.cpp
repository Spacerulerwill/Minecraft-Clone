/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include <world/chunk/ChunkMesher.hpp>
#include <glm/vec2.hpp>
#include <world/Block.hpp>
#include <world/chunk/Chunk.hpp>

#ifdef _MSC_VER
inline const int CTZ(uint64_t& x) {
    unsigned long index;
    _BitScanForward64(&index, x);
    return (int)index;
}
#else
inline const int CTZ(uint64_t x) {
    return __builtin_ctzll(x);
}
#endif

inline const int GetAxisIndex(const int& axis, const int& a, const int& b, const int& c) {
    if (axis == 0) return b + (a * Chunk::SIZE_PADDED) + (c * Chunk::SIZE_PADDED_SQUARED);
    else if (axis == 1) return a + (c * Chunk::SIZE_PADDED) + (b * Chunk::SIZE_PADDED_SQUARED);
    else return c + (b * Chunk::SIZE_PADDED) + (a * Chunk::SIZE_PADDED_SQUARED);
}

inline const bool SolidCheck(BlockType blockType) {
    return GetBlockData(blockType).opaque;
}

inline constexpr glm::ivec2 AODirections[8] = {
    glm::ivec2(0, -1),
    glm::ivec2(0, 1),
    glm::ivec2( - 1, 0),
    glm::ivec2(1, 0),
    glm::ivec2( - 1, -1 ),
    glm::ivec2( - 1, 1 ),
    glm::ivec2( 1, -1 ),
    glm::ivec2( 1, 1 ),
};

inline const int VertexAO(int side1, int side2, int corner) {
    if (side1 && side2) {
        return 0;
    }
    return 3 - (side1 + side2 + corner);
}

inline const bool CompareAO(const std::vector<Block>& blocks, int axis, int forward, int right, int c, int forward_offset, int right_offset) {
    for (const auto& ao_dir : AODirections) {
        bool firstBlockSolid = SolidCheck(blocks.at(GetAxisIndex(axis, right + ao_dir[0], forward + ao_dir[1], c)).GetType());
        bool secondBlockSolid = SolidCheck(blocks.at(GetAxisIndex(axis, right + right_offset + ao_dir[0], forward + forward_offset + ao_dir[1], c)).GetType());
        if (firstBlockSolid != secondBlockSolid) {
            return false;
        }
    }
    return true;
}

inline const bool CompareForward(const std::vector<Block>& blocks, int axis, int forward, int right, int bit_pos, int light_dir) {
    Block first = blocks.at(GetAxisIndex(axis, right, forward, bit_pos));
    Block second = blocks.at(GetAxisIndex(axis, right, forward + 1, bit_pos));
    bool ao = CompareAO(blocks, axis, forward, right, bit_pos + light_dir, 1, 0);
    return first == second && ao;
}

inline const bool CompareRight(const std::vector<Block>& blocks, int axis, int forward, int right, int bit_pos, int light_dir) {
    return
        blocks.at(GetAxisIndex(axis, right, forward, bit_pos)) == blocks.at(GetAxisIndex(axis, right + 1, forward, bit_pos)) &&
        CompareAO(blocks, axis, forward, right, bit_pos + light_dir, 0, 1);
}

inline const void InsertQuad(std::vector<ChunkMesher::ChunkVertex>& vertices, ChunkMesher::ChunkVertex v1, ChunkMesher::ChunkVertex v2, ChunkMesher::ChunkVertex v3, ChunkMesher::ChunkVertex v4, bool flipped) {
    if (flipped) {
        vertices.insert(vertices.end(), { v3, v2, v4, v4, v2, v1 });
    }
    else {
        vertices.insert(vertices.end(), { v1, v4, v3, v3, v2, v1});
    }
}

inline ChunkMesher::ChunkVertex GetChunkVertex(uint32_t x, uint32_t y, uint32_t z, uint32_t ao, uint32_t texX, uint32_t texY, uint32_t type, uint32_t face, bool isGrass) {
    return ChunkMesher::ChunkVertex{
        (ao << 18) | ((z - 1) << 12) | ((y - 1) << 6) | (x - 1),
        (isGrass << 23) | (face << 20) | (type << 12) | ((texY) << 6) | (texX),
    };
}

void ChunkMesher::BinaryGreedyMesh(std::vector<ChunkVertex>& vertices, const std::vector<Block>& blocks, ChunkMeshFilterCallback condition) {
    // Step 1: Convert to binary column representation for each direction
    std::vector<uint64_t> axis_cols(Chunk::SIZE_PADDED_SQUARED * 3);
    int index = 0;
    for (int y = 0; y < Chunk::SIZE_PADDED; y++) {
        for (int x = 0; x < Chunk::SIZE_PADDED; x++) {
            uint64_t zb = 0;
            for (int z = 0; z < Chunk::SIZE_PADDED; z++) {
                if (condition(blocks[index])) {
                    axis_cols[x + (z * Chunk::SIZE_PADDED)] |= 1ULL << y;
                    axis_cols[z + (y * Chunk::SIZE_PADDED) + (Chunk::SIZE_PADDED_SQUARED)] |= 1ULL << x;
                    zb |= 1ULL << z;
                }
                index++;
            }
            axis_cols[y + (x * Chunk::SIZE_PADDED) + (Chunk::SIZE_PADDED_SQUARED * 2)] = zb;
        }
    }


    // Step 2: Visible face culling
    std::vector<uint64_t> col_face_masks(Chunk::SIZE_PADDED_SQUARED * 6);
    for (int axis = 0; axis <= 2; axis++) {
        for (int i = 0; i < Chunk::SIZE_PADDED_SQUARED; i++) {
            uint64_t col = axis_cols[(Chunk::SIZE_PADDED_SQUARED * axis) + i];
            col_face_masks[(Chunk::SIZE_PADDED_SQUARED * (axis * 2)) + i] = col & ~((col >> 1) | (1ULL << (Chunk::SIZE_PADDED - 1)));
            col_face_masks[(Chunk::SIZE_PADDED_SQUARED * (axis * 2 + 1)) + i] = col & ~((col << 1) | 1ULL);
        }
    }

    // Step 3: Greedy meshing
    for (int face = 0; face < 6; face++) {
        int axis = face / 2;
        int light_dir = face % 2 == 0 ? 1 : -1;

        std::vector<int> merged_forward(Chunk::SIZE_PADDED_SQUARED);
        for (int forward = 1; forward < Chunk::SIZE_PADDED - 1; forward++) {
            uint64_t bits_walking_right = 0;
            int merged_right[Chunk::SIZE_PADDED] = { 0 };
            for (int right = 1; right < Chunk::SIZE_PADDED - 1; right++) {
                uint64_t bits_here = col_face_masks[right + (forward * Chunk::SIZE_PADDED) + (face * Chunk::SIZE_PADDED_SQUARED)];
                uint64_t bits_forward = forward >= Chunk::SIZE ? 0 : col_face_masks[right + (forward * Chunk::SIZE_PADDED) + (face * Chunk::SIZE_PADDED_SQUARED) + Chunk::SIZE_PADDED];
                uint64_t bits_right = right >= Chunk::SIZE ? 0 : col_face_masks[right + 1 + (forward * Chunk::SIZE_PADDED) + (face * Chunk::SIZE_PADDED_SQUARED)];
                uint64_t bits_merging_forward = bits_here & bits_forward & ~bits_walking_right;
                uint64_t bits_merging_right = bits_here & bits_right;

                uint64_t copy_front = bits_merging_forward;

                while (copy_front) {
                    int bit_pos = CTZ(copy_front);
                    copy_front &= ~(1ULL << bit_pos);

                    if (bit_pos == 0 || bit_pos == Chunk::SIZE_PADDED - 1) continue;

                    if (CompareForward(blocks, axis, forward, right, bit_pos, light_dir)) {
                        merged_forward[(right * Chunk::SIZE_PADDED) + bit_pos]++;
                    }
                    else {
                        bits_merging_forward &= ~(1ULL << bit_pos);
                    }
                }

                uint64_t bits_stopped_forward = bits_here & ~bits_merging_forward;
                while (bits_stopped_forward) {
                    int bit_pos = CTZ(bits_stopped_forward);
                    bits_stopped_forward &= ~(1ULL << bit_pos);

                    // Discards faces from neighbor blocks
                    if (bit_pos == 0 || bit_pos == Chunk::SIZE_PADDED - 1) { continue; };

                    if (
                        (bits_merging_right & (1ULL << bit_pos)) != 0 &&
                        merged_forward[(right * Chunk::SIZE_PADDED) + bit_pos] == merged_forward[(right + 1) * Chunk::SIZE_PADDED + bit_pos] &&
                        CompareRight(blocks, axis, forward, right, bit_pos, light_dir))
                    {
                        bits_walking_right |= 1ULL << bit_pos;
                        merged_right[bit_pos]++;
                        merged_forward[(right * Chunk::SIZE_PADDED) + bit_pos] = 0;
                        continue;
                    }
                    bits_walking_right &= ~(1ULL << bit_pos);

                    uint8_t mesh_left = right - merged_right[bit_pos];
                    uint8_t mesh_right = right + 1;
                    uint8_t mesh_front = forward - merged_forward[(right * Chunk::SIZE_PADDED) + bit_pos];
                    uint8_t mesh_back = forward + 1;
                    uint8_t mesh_up = bit_pos + (face % 2 == 0 ? 1 : 0);

                    Block block = blocks[GetAxisIndex(axis, right, forward, bit_pos)];
                    BlockType type = block.IsWaterLogged() ? BlockType::WATER : block.GetType();
                    bool isGrass = type == BlockType::GRASS;
                    const BlockDataStruct& blockData = GetBlockData(type);

                    int c = bit_pos + light_dir;
                    bool ao_F = SolidCheck(blocks[GetAxisIndex(axis, right, forward - 1, c)].GetType());
                    bool ao_B = SolidCheck(blocks[GetAxisIndex(axis, right, forward + 1, c)].GetType());
                    bool ao_L = SolidCheck(blocks[GetAxisIndex(axis, right - 1, forward, c)].GetType());
                    bool ao_R = SolidCheck(blocks[GetAxisIndex(axis, right + 1, forward, c)].GetType());

                    bool ao_LFC = SolidCheck(blocks[GetAxisIndex(axis, right - 1, forward - 1, c)].GetType());
                    bool ao_LBC = SolidCheck(blocks[GetAxisIndex(axis, right - 1, forward + 1, c)].GetType());
                    bool ao_RFC = SolidCheck(blocks[GetAxisIndex(axis, right + 1, forward - 1, c)].GetType());
                    bool ao_RBC = SolidCheck(blocks[GetAxisIndex(axis, right + 1, forward + 1, c)].GetType());

                    uint32_t ao_LB = VertexAO(ao_L, ao_B, ao_LBC);
                    uint32_t ao_LF = VertexAO(ao_L, ao_F, ao_LFC);
                    uint32_t ao_RB = VertexAO(ao_R, ao_B, ao_RBC);
                    uint32_t ao_RF = VertexAO(ao_R, ao_F, ao_RFC);

                    merged_forward[(right * Chunk::SIZE_PADDED) + bit_pos] = 0;
                    merged_right[bit_pos] = 0;

                    ChunkVertex v1{}, v2{}, v3{}, v4{};
                    switch (face) {
                    case 0: {
                        TextureID texZ = blockData.faces[BACK_FACE];
                        v1 = GetChunkVertex(mesh_left, mesh_up, mesh_front, ao_LF, 0, mesh_back - mesh_front, texZ, face, isGrass);
                        v2 = GetChunkVertex(mesh_left, mesh_up, mesh_back, ao_LB, 0, 0, texZ, face, isGrass);
                        v3 = GetChunkVertex(mesh_right, mesh_up, mesh_back, ao_RB, mesh_right - mesh_left, 0, texZ, face, isGrass);
                        v4 = GetChunkVertex(mesh_right, mesh_up, mesh_front, ao_RF, mesh_right - mesh_left, mesh_back - mesh_front, texZ, face, isGrass);
                        break;
                    }
                    case 1: {
                        TextureID texZ = blockData.faces[FRONT_FACE];
                        v1 = GetChunkVertex(mesh_left, mesh_up, mesh_back, ao_LB, 0, 0, texZ, face, isGrass);
                        v2 = GetChunkVertex(mesh_left, mesh_up, mesh_front, ao_LF, 0, mesh_back - mesh_front, texZ, face, isGrass);
                        v3 = GetChunkVertex(mesh_right, mesh_up, mesh_front, ao_RF, mesh_right - mesh_left, mesh_back - mesh_front, texZ, face, isGrass);
                        v4 = GetChunkVertex(mesh_right, mesh_up, mesh_back, ao_RB, mesh_right - mesh_left, 0, texZ, face, isGrass);
                        break;
                    }
                    case 2: {
                        TextureID texZ = blockData.faces[RIGHT_FACE];
                        v1 = GetChunkVertex(mesh_up, mesh_front, mesh_left, ao_LF, mesh_back - mesh_front, mesh_right - mesh_left, texZ, face, isGrass);
                        v2 = GetChunkVertex(mesh_up, mesh_back, mesh_left, ao_LB, 0, mesh_right - mesh_left, texZ, face, isGrass);
                        v3 = GetChunkVertex(mesh_up, mesh_back, mesh_right, ao_RB, 0, 0, texZ, face, isGrass);
                        v4 = GetChunkVertex(mesh_up, mesh_front, mesh_right, ao_RF, mesh_back - mesh_front, 0, texZ, face, isGrass);
                        break;
                    }
                    case 3: {
                        TextureID texZ = blockData.faces[LEFT_FACE];
                        v1 = GetChunkVertex(mesh_up, mesh_back, mesh_left, ao_LB, 0, mesh_right - mesh_left, texZ, face, isGrass);
                        v2 = GetChunkVertex(mesh_up, mesh_front, mesh_left, ao_LF, mesh_back - mesh_front, mesh_right - mesh_left, texZ, face, isGrass);
                        v3 = GetChunkVertex(mesh_up, mesh_front, mesh_right, ao_RF, mesh_back - mesh_front, 0, texZ, face, isGrass);
                        v4 = GetChunkVertex(mesh_up, mesh_back, mesh_right, ao_RB, 0, 0, texZ, face, isGrass);
                        break;
                    }
                    case 4: {
                        TextureID texZ = blockData.faces[TOP_FACE];
                        v1 = GetChunkVertex(mesh_front, mesh_left, mesh_up, ao_LF, 0, mesh_right - mesh_left, texZ, face, isGrass);
                        v2 = GetChunkVertex(mesh_back, mesh_left, mesh_up, ao_LB, mesh_back - mesh_front, mesh_right - mesh_left, texZ, face, isGrass);
                        v3 = GetChunkVertex(mesh_back, mesh_right, mesh_up, ao_RB, mesh_back - mesh_front, 0, texZ, face, isGrass);
                        v4 = GetChunkVertex(mesh_front, mesh_right, mesh_up, ao_RF, 0, 0, texZ, face, isGrass);
                        break;
                    }
                    case 5: {
                        TextureID texZ = blockData.faces[BOTTOM_FACE];
                        v1 = GetChunkVertex(mesh_back, mesh_left, mesh_up, ao_LB, 0, mesh_right - mesh_left, texZ, face, isGrass);
                        v2 = GetChunkVertex(mesh_front, mesh_left, mesh_up, ao_LF, mesh_back - mesh_front, mesh_right - mesh_left, texZ, face, isGrass);
                        v3 = GetChunkVertex(mesh_front, mesh_right, mesh_up, ao_RF, mesh_back - mesh_front, 0, texZ, face, isGrass);
                        v4 = GetChunkVertex(mesh_back, mesh_right, mesh_up, ao_RB, 0, 0, texZ, face, isGrass);
                        break;
                    }
                    }
                    bool flipped = ao_LB + ao_RF > ao_RB + ao_LF;
                    InsertQuad(vertices, v1, v2, v3, v4, !flipped);
                }
            }
        }
    }
}

ChunkMesher::ChunkVertex GetCustomModelBlockVertex(uint32_t x, uint32_t y, uint32_t z, uint32_t texX, uint32_t texY, uint32_t type, bool isFoliage) {
    return {
        ((z) << 20) | ((y) << 10) | (x),
        (isFoliage << 18) | (type << 10) | (texY << 5) | (texX)
    };
}

void ChunkMesher::MeshCustomModelBlocks(std::vector<ChunkVertex>& vertices, const std::vector<Block>& blocks) {
    for (int x = 1; x < Chunk::SIZE_PADDED_SUB_1; x++) {
        for (int y = 1; y < Chunk::SIZE_PADDED_SUB_1; y++) {
            for (int z = 1; z < Chunk::SIZE_PADDED_SUB_1; z++) {
                BlockType type = blocks[VoxelIndex(glm::ivec3( x, y, z ))].GetType();
                BlockDataStruct blockData = GetBlockData(type);
                if (blockData.modelID != static_cast<ModelID>(Model::CUBE)) {
                    if (
                        !GetBlockData(blocks[VoxelIndex(glm::ivec3( x + 1, y, z ))].GetType()).opaque ||
                        !GetBlockData(blocks[VoxelIndex(glm::ivec3( x - 1, y, z ))].GetType()).opaque ||
                        !GetBlockData(blocks[VoxelIndex(glm::ivec3( x, y + 1, z ))].GetType()).opaque ||
                        !GetBlockData(blocks[VoxelIndex(glm::ivec3( x, y - 1, z ))].GetType()).opaque ||
                        !GetBlockData(blocks[VoxelIndex(glm::ivec3( x, y, z + 1 ))].GetType()).opaque ||
                        !GetBlockData(blocks[VoxelIndex(glm::ivec3( x, y, z - 1 ))].GetType()).opaque
                        ) {
                        std::vector<uint32_t>& modelVertices = BlockModels[blockData.modelID];
                        std::vector<ChunkVertex> modelPackedVerts;
                        for (int i = 0; i < modelVertices.size() - 1; i += 7) {
                            modelPackedVerts.push_back(GetCustomModelBlockVertex(
                                modelVertices[i] + ((x - 1) * 16),
                                modelVertices[i + 1] + ((y - 1) * 16),
                                modelVertices[i + 2] + ((z - 1) * 16),
                                modelVertices[i + 3],
                                modelVertices[i + 4],
                                blockData.faces[TOP_FACE],
                                type == BlockType::TALL_GRASS
                            ));
                        }
                        vertices.insert(vertices.end(), modelPackedVerts.begin(), modelPackedVerts.end());
                    }
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
