/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include <world/chunk/ChunkMesher.hpp>
#include <math/Vector.hpp>
#include <world/Block.hpp>

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
    if (axis == 0) return b + (a * CS_P) + (c * CS_P2);
    else if (axis == 1) return a + (c * CS_P) + (b * CS_P2);
    else return c + (b * CS_P) + (a * CS_P2);
}

// Add checks to this function to skip culling against grass for example
inline const bool SolidCheck(int voxel) {
    return voxel > 0;
}

inline constexpr iVec2 AODirections[8] = {
    {0, -1},
    {0, 1},
    {-1, 0 },
    {1, 0},
    {-1, -1 },
    { -1, 1 },
    { 1, -1 },
    { 1, 1 },
};

inline const int VertexAO(int side1, int side2, int corner) {
    if (side1 && side2) {
        return 0;
    }
    return 3 - (side1 + side2 + corner);
}

inline const bool CompareAO(const std::vector<BlockID>& blocks, int axis, int forward, int right, int c, int forward_offset, int right_offset) {
    for (const auto& ao_dir : AODirections) {
        if (SolidCheck(blocks.at(GetAxisIndex(axis, right + ao_dir[0], forward + ao_dir[1], c))) !=
            SolidCheck(blocks.at(GetAxisIndex(axis, right + right_offset + ao_dir[0], forward + forward_offset + ao_dir[1], c)))
            ) {
            return false;
        }
    }
    return true;
}

inline const bool CompareForward(const std::vector<BlockID>& blocks, int axis, int forward, int right, int bit_pos, int light_dir) {
    return
        blocks.at(GetAxisIndex(axis, right, forward, bit_pos)) == blocks.at(GetAxisIndex(axis, right, forward + 1, bit_pos)) &&
        CompareAO(blocks, axis, forward, right, bit_pos + light_dir, 1, 0);
}

inline const bool CompareRight(const std::vector<BlockID>& blocks, int axis, int forward, int right, int bit_pos, int light_dir) {
    return
        blocks.at(GetAxisIndex(axis, right, forward, bit_pos)) == blocks.at(GetAxisIndex(axis, right + 1, forward, bit_pos)) &&
        CompareAO(blocks, axis, forward, right, bit_pos + light_dir, 0, 1);
}

inline const void InsertQuad(std::vector<ChunkMesher::ChunkVertex>& vertices, ChunkMesher::ChunkVertex v1, ChunkMesher::ChunkVertex v2, ChunkMesher::ChunkVertex v3, ChunkMesher::ChunkVertex v4, bool flipped) {
    if (flipped) {
        vertices.insert(vertices.end(), { v1, v2, v4, v4, v2, v3 });
    }
    else {
        vertices.insert(vertices.end(), { v1, v2, v3, v3, v4, v1 });
    }
}

inline ChunkMesher::ChunkVertex GetChunkVertex(uint32_t x, uint32_t y, uint32_t z, uint32_t ao, uint32_t texX, uint32_t texY, uint32_t type) {
    return ChunkMesher::ChunkVertex{
        (ao << 18) | ((z - 1) << 12) | ((y - 1) << 6) | (x - 1),
        (type << 12) | ((texY) << 6) | (texX),
    };
}

std::vector<ChunkMesher::ChunkVertex> ChunkMesher::BinaryGreedyMesh(const std::vector<BlockID>& blocks) {
    std::vector<uint64_t> axis_cols(CS_P2 * 3);
    std::vector<uint64_t> col_face_masks(CS_P2 * 6);
    std::vector<ChunkVertex> vertices;

    // Step 1: Convert to binary representation for each direction
    int index = 0;
    for (int y = 0; y < CS_P; y++) {
        for (int x = 0; x < CS_P; x++) {
            uint64_t zb = 0;
            for (int z = 0; z < CS_P; z++) {
                if (SolidCheck(blocks[index])) {
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
        int light_dir = face % 2 == 0 ? 1 : -1;

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

                    if (CompareForward(blocks, axis, forward, right, bit_pos, light_dir)) {
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

                    // Discards faces from neighbor blocks
                    if (bit_pos == 0 || bit_pos == CS_P - 1) { continue; };

                    if (
                        (bits_merging_right & (1ULL << bit_pos)) != 0 &&
                        merged_forward[(right * CS_P) + bit_pos] == merged_forward[(right + 1) * CS_P + bit_pos] &&
                        CompareRight(blocks, axis, forward, right, bit_pos, light_dir))
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

                    BlockID type = blocks[GetAxisIndex(axis, right, forward, bit_pos)];
                    BlockDataStruct blockData = BlockData[type];

                    int c = bit_pos + light_dir;
                    bool ao_F = SolidCheck(blocks[GetAxisIndex(axis, right, forward - 1, c)]);
                    bool ao_B = SolidCheck(blocks[GetAxisIndex(axis, right, forward + 1, c)]);
                    bool ao_L = SolidCheck(blocks[GetAxisIndex(axis, right - 1, forward, c)]);
                    bool ao_R = SolidCheck(blocks[GetAxisIndex(axis, right + 1, forward, c)]);

                    bool ao_LFC = SolidCheck(blocks[GetAxisIndex(axis, right - 1, forward - 1, c)]);
                    bool ao_LBC = SolidCheck(blocks[GetAxisIndex(axis, right - 1, forward + 1, c)]);
                    bool ao_RFC = SolidCheck(blocks[GetAxisIndex(axis, right + 1, forward - 1, c)]);
                    bool ao_RBC = SolidCheck(blocks[GetAxisIndex(axis, right + 1, forward + 1, c)]);

                    uint32_t ao_LB = VertexAO(ao_L, ao_B, ao_LBC);
                    uint32_t ao_LF = VertexAO(ao_L, ao_F, ao_LFC);
                    uint32_t ao_RB = VertexAO(ao_R, ao_B, ao_RBC);
                    uint32_t ao_RF = VertexAO(ao_R, ao_F, ao_RFC);

                    merged_forward[(right * CS_P) + bit_pos] = 0;
                    merged_right[bit_pos] = 0;

                    ChunkVertex v1, v2, v3, v4;
                    switch (face) {
                    case 0: {
                        TextureID texZ = blockData.faces[TOP_FACE];
                        v1 = GetChunkVertex(mesh_left, mesh_up, mesh_front, ao_LF, 0, 0, texZ);
                        v2 = GetChunkVertex(mesh_left, mesh_up, mesh_back, ao_LB, 0, mesh_back - mesh_front, texZ);
                        v3 = GetChunkVertex(mesh_right, mesh_up, mesh_back, ao_RB, mesh_right - mesh_left, mesh_back - mesh_front, texZ);
                        v4 = GetChunkVertex(mesh_right, mesh_up, mesh_front, ao_RF, mesh_right - mesh_left, 0, texZ);
                        break;
                    }
                    case 1: {
                        TextureID texZ = blockData.faces[BOTTOM_FACE];
                        v1 = GetChunkVertex(mesh_left, mesh_up, mesh_back, ao_LB, 0, mesh_back - mesh_front, texZ);
                        v2 = GetChunkVertex(mesh_left, mesh_up, mesh_front, ao_LF, 0, 0, texZ);
                        v3 = GetChunkVertex(mesh_right, mesh_up, mesh_front, ao_RF, mesh_right - mesh_left, 0, texZ);
                        v4 = GetChunkVertex(mesh_right, mesh_up, mesh_back, ao_RB, mesh_right - mesh_left, mesh_back - mesh_front, texZ);
                        break;
                    }
                    case 2: {
                        TextureID texZ = blockData.faces[RIGHT_FACE];
                        v1 = GetChunkVertex(mesh_up, mesh_front, mesh_left, ao_LF, 0, mesh_back - mesh_front, texZ);
                        v2 = GetChunkVertex(mesh_up, mesh_back, mesh_left, ao_LB, 0, 0, texZ);
                        v3 = GetChunkVertex(mesh_up, mesh_back, mesh_right, ao_RB, mesh_right - mesh_left, 0, texZ);
                        v4 = GetChunkVertex(mesh_up, mesh_front, mesh_right, ao_RF, mesh_right - mesh_left, mesh_back - mesh_front, texZ);
                        break;
                    }
                    case 3: {
                        TextureID texZ = blockData.faces[LEFT_FACE];
                        v1 = GetChunkVertex(mesh_up, mesh_back, mesh_left, ao_LB, 0, 0, texZ);
                        v2 = GetChunkVertex(mesh_up, mesh_front, mesh_left, ao_LF, 0, mesh_back - mesh_front, texZ);
                        v3 = GetChunkVertex(mesh_up, mesh_front, mesh_right, ao_RF, mesh_right - mesh_left, mesh_back - mesh_front, texZ);
                        v4 = GetChunkVertex(mesh_up, mesh_back, mesh_right, ao_RB, mesh_right - mesh_left, 0, texZ);
                        break;
                    }
                    case 4: {
                        TextureID texZ = blockData.faces[BACK_FACE];
                        v1 = GetChunkVertex(mesh_front, mesh_left, mesh_up, ao_LF, 0, mesh_right - mesh_left, texZ);
                        v2 = GetChunkVertex(mesh_back, mesh_left, mesh_up, ao_LB, mesh_back - mesh_front, mesh_right - mesh_left, texZ);
                        v3 = GetChunkVertex(mesh_back, mesh_right, mesh_up, ao_RB, mesh_back - mesh_front, 0, texZ);
                        v4 = GetChunkVertex(mesh_front, mesh_right, mesh_up, ao_RF, 0, 0, texZ);
                        break;
                    }
                    case 5: {
                        TextureID texZ = blockData.faces[FRONT_FACE];
                        v1 = GetChunkVertex(mesh_back, mesh_left, mesh_up, ao_LB, 0, mesh_right - mesh_left, texZ);
                        v2 = GetChunkVertex(mesh_front, mesh_left, mesh_up, ao_LF, mesh_back - mesh_front, mesh_right - mesh_left, texZ);
                        v3 = GetChunkVertex(mesh_front, mesh_right, mesh_up, ao_RF, mesh_back - mesh_front, 0, texZ);
                        v4 = GetChunkVertex(mesh_back, mesh_right, mesh_up, ao_RB, 0, 0, texZ);
                        break;
                    }
                    }
                    bool flipped = ao_LB + ao_RF > ao_RB + ao_LF;
                    InsertQuad(vertices, v1, v2, v3, v4, flipped);
                }
            }
        }
    }
    return vertices;
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
