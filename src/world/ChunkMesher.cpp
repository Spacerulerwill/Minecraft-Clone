/*
Copyright (C) 2023 William Redding - All Rights Reserved
LICENSE: MIT
*/

#include <util/Log.hpp>
#include <world/ChunkMesher.hpp>
#include <world/Chunk.hpp>
#include <world/Block.hpp>

namespace engine {
    #ifdef _MSC_VER
    #include <intrin.h>
    int CTZ(uint64_t &x) {
    unsigned long index;
    _BitScanForward64(&index, x);
    return static_cast<int>(index);
    }
    #else
    int CTZ(uint64_t x) {
    return __builtin_ctzll(x);
    }
    #endif

    bool SolidCheck(BlockInt voxel) {
        return voxel > 0;
    }

    bool OpaqueCheck(BlockInt voxel) {
        BlockDataStruct data = BlockData[voxel];
        return data.opaque && data.model == CUBE;
    }

    int GetAxisI(const int &axis, const int &a, const int &b, const int &c) {
        if (axis == 0) return b + (a * CS_P) + (c * CS_P2);
        else if (axis == 1) return a + (c * CS_P) + (b* CS_P2);
        else return c + (b * CS_P) + (a * CS_P2);
    }
    
    uint32_t CalculateVertexAO(bool side1, bool side2, bool corner) {
        return 3 - (side1 + side2 + corner);
    }
    
    Vec2<int> ao_dirs[8] = {
        {0, -1},
        {0, 1},
        {-1, 0},
        {1, 0},
        {-1, -1},
        {-1, 1},
        {1, -1},
        {1, 1}
    };

    bool CompareAO(const BlockInt* voxels, int axis, int forward, int right, int c, int forward_offset, int right_offset) {
        for (const auto& ao_dir : ao_dirs) {
            if (OpaqueCheck(voxels[GetAxisI(axis, right + ao_dir.x, forward + ao_dir.y, c)]) !=
                OpaqueCheck(voxels[GetAxisI(axis, right + right_offset + ao_dir.x, forward + forward_offset + ao_dir.y, c)])
            ) {
            return false;
            }
        }
        return true;
    }

    bool CompareForward(const BlockInt* voxels, int axis, int forward, int right, int bit_pos, int light_dir) {
        return voxels[GetAxisI(axis, right, forward, bit_pos)] == voxels[GetAxisI(axis, right, forward + 1, bit_pos)]
        && CompareAO(voxels, axis, forward, right, bit_pos + light_dir, 1, 0);
    }

    bool CompareRight(const BlockInt* voxels, int axis, int forward, int right, int bit_pos, int light_dir) {
        return voxels[GetAxisI(axis, right, forward, bit_pos)] == voxels[GetAxisI(axis, right + 1, forward, bit_pos)]
        && CompareAO(voxels, axis, forward, right, bit_pos + light_dir, 0, 1);
    }

    void InsertQuad(std::vector<CubeChunkVertex>& vertices, CubeChunkVertex v1, CubeChunkVertex v2, CubeChunkVertex v3, CubeChunkVertex v4, bool flipped) {
        if (flipped) {
            vertices.insert(vertices.end(), { v1, v2, v4, v4, v2, v3 });
        }
        else {
            vertices.insert(vertices.end(), { v1, v2, v3, v3, v4, v1 });
        }
    }
}

void engine::GreedyTranslucent(std::vector<engine::CubeChunkVertex>& vertices, std::vector<engine::CubeChunkVertex>& waterVertices, const engine::BlockInt* voxels) {
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

                    if (CompareForward(voxels, axis, forward, right, bit_pos, 0)) {
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
                        CompareRight(voxels, axis, forward, right, bit_pos, 0))
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

                    CubeChunkVertex v1, v2, v3, v4;
                    bool isGrass = type  == GRASS;
                    bool isFoliage = type == TALL_GRASS | type == OAK_LEAVES;

                    switch(face) {
                    case 0: {
                        TextureInt texZ = blockData.top_face;
                        v1 = GetCubeBlockVertex(mesh_left, mesh_up, mesh_front, face, 0, 0, texZ, 3, isGrass, isFoliage);
                        v2 = GetCubeBlockVertex(mesh_left, mesh_up, mesh_back, face, 0, mesh_back - mesh_front, texZ, 3, isGrass, isFoliage);
                        v3 = GetCubeBlockVertex(mesh_right, mesh_up, mesh_back, face, mesh_right-mesh_left, mesh_back-mesh_front, texZ, 3, isGrass, isFoliage);
                        v4 = GetCubeBlockVertex(mesh_right, mesh_up, mesh_front, face, mesh_right-mesh_left, 0, texZ, 3, isGrass, isFoliage);
                        break;
                    }
                    case 1: {
                        TextureInt texZ = blockData.bottom_face;
                        v1 = GetCubeBlockVertex(mesh_left, mesh_up, mesh_back, face, 0, mesh_back-mesh_front, texZ, 3, isGrass, isFoliage);
                        v2 = GetCubeBlockVertex(mesh_left, mesh_up, mesh_front, face, 0, 0, texZ, 3, isGrass, isFoliage);
                        v3 = GetCubeBlockVertex(mesh_right, mesh_up, mesh_front, face, mesh_right-mesh_left, 0, texZ, 3, isGrass, isFoliage);
                        v4 = GetCubeBlockVertex(mesh_right, mesh_up, mesh_back, face, mesh_right-mesh_left, mesh_back-mesh_front, texZ, 3, isGrass, isFoliage);
                        break;
                    }
                    case 2: { 
                        TextureInt texZ = blockData.right_face;
                        v1 = GetCubeBlockVertex(mesh_up, mesh_front, mesh_left, face, 0, mesh_back - mesh_front, texZ, 3, isGrass, isFoliage);
                        v2 = GetCubeBlockVertex(mesh_up, mesh_back, mesh_left, face, 0, 0, texZ, 3, isGrass, isFoliage);
                        v3 = GetCubeBlockVertex(mesh_up, mesh_back, mesh_right, face, mesh_right - mesh_left, 0, texZ, 3, isGrass, isFoliage);
                        v4 = GetCubeBlockVertex(mesh_up, mesh_front, mesh_right, face, mesh_right - mesh_left, mesh_back - mesh_front, texZ, 3, isGrass, isFoliage);
                        break;
                    }
                    case 3: {
                        TextureInt texZ = blockData.left_face;
                        v1 = GetCubeBlockVertex(mesh_up, mesh_back, mesh_left, face, 0, 0, texZ, 3, isGrass, isFoliage);
                        v2 = GetCubeBlockVertex(mesh_up, mesh_front, mesh_left, face, 0, mesh_back - mesh_front, texZ, 3, isGrass, isFoliage);
                        v3 = GetCubeBlockVertex(mesh_up, mesh_front, mesh_right, face, mesh_right - mesh_left, mesh_back - mesh_front, texZ, 3, isGrass, isFoliage);
                        v4 = GetCubeBlockVertex(mesh_up, mesh_back, mesh_right, face, mesh_right - mesh_left, 0, texZ, 3, isGrass, isFoliage);
                        break;
                    }
                    case 4: {
                        TextureInt texZ = blockData.back_face;
                        v1 = GetCubeBlockVertex(mesh_front, mesh_left, mesh_up, face, 0, mesh_right - mesh_left, texZ, 3, isGrass, isFoliage);
                        v2 = GetCubeBlockVertex(mesh_back, mesh_left, mesh_up, face, mesh_back - mesh_front, mesh_right - mesh_left, texZ, 3, isGrass, isFoliage);
                        v3 = GetCubeBlockVertex(mesh_back, mesh_right, mesh_up, face, mesh_back - mesh_front, 0, texZ , 3, isGrass, isFoliage);
                        v4 = GetCubeBlockVertex(mesh_front, mesh_right, mesh_up, face, 0, 0, texZ, 3, isGrass, isFoliage);
                        break;
                    }
                    case 5: {
                        TextureInt texZ = blockData.front_face;
                        v1 = GetCubeBlockVertex(mesh_back, mesh_left, mesh_up, face, 0, mesh_right - mesh_left, texZ, 3, isGrass, isFoliage);
                        v2 = GetCubeBlockVertex(mesh_front, mesh_left, mesh_up, face, mesh_back - mesh_front, mesh_right - mesh_left, texZ, 3, isGrass, isFoliage);
                        v3 = GetCubeBlockVertex(mesh_front, mesh_right, mesh_up, face, mesh_back - mesh_front, 0, texZ, 3, isGrass, isFoliage);
                        v4 = GetCubeBlockVertex(mesh_back, mesh_right, mesh_up, face, 0, 0, texZ, 3, isGrass, isFoliage);
                        break;
                    }
                    }
                    if (type == WATER) {
                        InsertQuad(waterVertices, v1, v2, v3, v4, false);
                    }else {
                        InsertQuad(vertices, v1, v2, v3, v4, false);
                    }
                }
            }
        }
    }
}

void engine::GreedyOpaque(std::vector<engine::CubeChunkVertex>& vertices, const engine::BlockInt* voxels) {
    uint64_t axis_cols[CS_P2 * 3] = { 0 };
    uint64_t not_opaque_axis_cols[CS_P2 * 3] = { 0 };
    uint64_t col_face_masks[CS_P2 * 6];

    // Step 1: Convert to binary representation for each direction
    int index = 0;
    for (int y = 0; y < CS_P; y++) {
        for (int x = 0; x < CS_P; x++) {
            uint64_t zb = 0;
            for (int z = 0; z < CS_P; z++) {
                if (OpaqueCheck(voxels[index])) {
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

                    if (bit_pos == 0 || bit_pos == CS_P - 1 ) continue;

                    if (CompareForward(voxels, axis, forward, right, bit_pos, light_dir)) {
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
                        CompareRight(voxels, axis, forward, right, bit_pos, light_dir))
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

                    int c = bit_pos + light_dir;
                    bool ao_F = OpaqueCheck(voxels[GetAxisI(axis, right, forward - 1, c)]);
                    bool ao_B = OpaqueCheck(voxels[GetAxisI(axis, right, forward + 1, c)]);
                    bool ao_L = OpaqueCheck(voxels[GetAxisI(axis, right - 1, forward, c)]);
                    bool ao_R = OpaqueCheck(voxels[GetAxisI(axis, right + 1, forward, c)]);

                    bool ao_LFC = OpaqueCheck(voxels[GetAxisI(axis, right - 1, forward - 1, c)]);
                    bool ao_LBC = OpaqueCheck(voxels[GetAxisI(axis, right - 1, forward + 1, c)]);
                    bool ao_RFC = OpaqueCheck(voxels[GetAxisI(axis, right + 1, forward - 1, c)]);
                    bool ao_RBC = OpaqueCheck(voxels[GetAxisI(axis, right + 1, forward + 1, c)]);

                    uint32_t ao_LB = CalculateVertexAO(ao_L, ao_B, ao_LBC);
                    uint32_t ao_LF = CalculateVertexAO(ao_L, ao_F, ao_LFC);
                    uint32_t ao_RB = CalculateVertexAO(ao_R, ao_B, ao_RBC);
                    uint32_t ao_RF = CalculateVertexAO(ao_R, ao_F, ao_RFC);

                    merged_forward[(right * CS_P) + bit_pos] = 0;
                    merged_right[bit_pos] = 0;

                    CubeChunkVertex v1, v2, v3, v4;
                    bool isGrass = type == GRASS;
                    bool isFoliage = type == type == OAK_LEAVES;

                    switch(face) {
                    case 0: {
                        TextureInt texZ = blockData.top_face;
                        v1 = GetCubeBlockVertex(mesh_left, mesh_up, mesh_front, face, 0, 0, texZ, ao_LF, isGrass, isFoliage);
                        v2 = GetCubeBlockVertex(mesh_left, mesh_up, mesh_back,face, 0, mesh_back - mesh_front, texZ, ao_LB, isGrass, isFoliage);
                        v3 = GetCubeBlockVertex(mesh_right, mesh_up, mesh_back, face, mesh_right-mesh_left, mesh_back-mesh_front, texZ, ao_RB, isGrass, isFoliage);
                        v4 = GetCubeBlockVertex(mesh_right, mesh_up, mesh_front,face, mesh_right-mesh_left, 0, texZ, ao_RF, isGrass, isFoliage);
                        break;
                    }
                    case 1: {
                        TextureInt texZ = blockData.bottom_face;
                        v1 = GetCubeBlockVertex(mesh_left, mesh_up, mesh_back, face, 0, mesh_back-mesh_front, texZ, ao_LB, isGrass, isFoliage);
                        v2 = GetCubeBlockVertex(mesh_left, mesh_up, mesh_front, face, 0, 0, texZ, ao_LF, isGrass, isFoliage);
                        v3 = GetCubeBlockVertex(mesh_right, mesh_up, mesh_front, face, mesh_right-mesh_left, 0, texZ, ao_RF, isGrass, isFoliage);
                        v4 = GetCubeBlockVertex(mesh_right, mesh_up, mesh_back, face, mesh_right-mesh_left, mesh_back-mesh_front, texZ, ao_RB, isGrass, isFoliage);
                        
                        break;
                    }
                    case 2: { 
                        TextureInt texZ = blockData.right_face;
                        v1 = GetCubeBlockVertex(mesh_up, mesh_front, mesh_left, face, 0, mesh_back - mesh_front, texZ, ao_LF, isGrass, isFoliage);
                        v2 = GetCubeBlockVertex(mesh_up, mesh_back, mesh_left, face, 0, 0, texZ, ao_LB, isGrass, isFoliage);
                        v3 = GetCubeBlockVertex(mesh_up, mesh_back, mesh_right, face, mesh_right - mesh_left, 0, texZ, ao_RB, isGrass, isFoliage);
                        v4 = GetCubeBlockVertex(mesh_up, mesh_front, mesh_right, face, mesh_right - mesh_left, mesh_back - mesh_front, texZ, ao_RF, isGrass, isFoliage);
                        break;
                    }
                    case 3: {
                        TextureInt texZ = blockData.left_face;
                        v1 = GetCubeBlockVertex(mesh_up, mesh_back, mesh_left, face, 0, 0, texZ, ao_LB, isGrass, isFoliage);
                        v2 = GetCubeBlockVertex(mesh_up, mesh_front, mesh_left, face, 0, mesh_back - mesh_front, texZ, ao_LF, isGrass, isFoliage);
                        v3 = GetCubeBlockVertex(mesh_up, mesh_front, mesh_right, face, mesh_right - mesh_left, mesh_back - mesh_front, texZ, ao_RF, isGrass, isFoliage);
                        v4 = GetCubeBlockVertex(mesh_up, mesh_back, mesh_right,  face, mesh_right - mesh_left, 0, texZ, ao_RB, isGrass, isFoliage);
                        break;
                    }
                    case 4: {
                        TextureInt texZ = blockData.back_face;
                        v1 = GetCubeBlockVertex(mesh_front, mesh_left, mesh_up, face, 0, mesh_right - mesh_left, texZ, ao_LF, isGrass, isFoliage);
                        v2 = GetCubeBlockVertex(mesh_back, mesh_left, mesh_up, face, mesh_back - mesh_front, mesh_right - mesh_left, texZ, ao_LB, isGrass, isFoliage);
                        v3 = GetCubeBlockVertex(mesh_back, mesh_right, mesh_up, face, mesh_back - mesh_front, 0, texZ, ao_RB, isGrass, isFoliage);
                        v4 = GetCubeBlockVertex(mesh_front, mesh_right, mesh_up, face, 0, 0, texZ, ao_RF, isGrass, isFoliage);
                        break;
                    }
                    case 5: {
                        TextureInt texZ = blockData.front_face;
                        v1 = GetCubeBlockVertex(mesh_back, mesh_left, mesh_up, face, 0, mesh_right - mesh_left, texZ, ao_LB, isGrass, isFoliage);
                        v2 = GetCubeBlockVertex(mesh_front, mesh_left, mesh_up, face, mesh_back - mesh_front, mesh_right - mesh_left, texZ, ao_LF, isGrass, isFoliage);
                        v3 = GetCubeBlockVertex(mesh_front, mesh_right, mesh_up, face, mesh_back - mesh_front, 0, texZ, ao_RF, isGrass, isFoliage);
                        v4 = GetCubeBlockVertex(mesh_back, mesh_right, mesh_up, face, 0, 0, texZ, ao_RB, isGrass, isFoliage);
                        break;
                    }
                    }
                    bool flipped = ao_LB + ao_RF > ao_RB + ao_LF;
                    InsertQuad(vertices, v1, v2, v3, v4, flipped);
                }   
            }
        }
    }
}

void engine::MeshCustomModelBlocks(std::vector<CustomModelChunkVertex>& vertices, const engine::BlockInt* voxels) {
    for (int x = 1; x < CS_P_MINUS_ONE; x++){
        for (int y = 1; y < CS_P_MINUS_ONE; y++) {
            for (int z =1; z < CS_P_MINUS_ONE; z++) {
                BlockInt type = voxels[voxelIndex(x,y,z)];
                BlockDataStruct blockData = BlockData[type];
                if (blockData.model != CUBE && (
                    !BlockData[voxels[voxelIndex(x+1,y,z)]].opaque ||
                    !BlockData[voxels[voxelIndex(x-1,y,z)]].opaque ||
                    !BlockData[voxels[voxelIndex(x,y+1,z)]].opaque ||
                    !BlockData[voxels[voxelIndex(x,y-1,z)]].opaque ||
                    !BlockData[voxels[voxelIndex(x,y,z+1)]].opaque ||
                    !BlockData[voxels[voxelIndex(x,y,z-1)]].opaque
                )) {
                    BlockModelStruct modelData = BlockModelData[blockData.model];
                    std::vector<uint32_t> modelVerts(modelData.begin, modelData.end);
                    std::vector<CustomModelChunkVertex> modelPackedVerts;
                    for (int i = 0; i < modelVerts.size()-1; i += 7) {                  
                        modelPackedVerts.push_back(GetCustomModelBlockVertex(
                                modelVerts[i] + ((x-1)*16), 
                                modelVerts[i+1] + ((y-1)*16),
                                modelVerts[i+2] + ((z-1)*16),
                                modelVerts[i+3],
                                modelVerts[i+4],
                                blockData.top_face,
                                type == TALL_GRASS
                        ));    
                    }
                    vertices.insert(vertices.end(), modelPackedVerts.begin(), modelPackedVerts.end());
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
