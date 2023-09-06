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

engine::Chunk::Chunk(int chunkX, int chunkY, int chunkZ): chunkX(chunkX), chunkY(chunkY), chunkZ(chunkZ)
{
    m_Model *= translate(Vec3(chunkX * CHUNK_SIZE, chunkY * CHUNK_SIZE, chunkZ * CHUNK_SIZE));

    memset(m_Blocks, DIRT, sizeof(BlockInt) * CHUNK_SIZE_CUBED);
    memset(m_Blocks, GRASS, CHUNK_SIZE);
    
    VertexBufferLayout bufLayout;
	bufLayout.AddAttribute<float>(3);
    bufLayout.AddAttribute<float>(3);
    bufLayout.AddAttribute<float>(1);
    m_VAO.AddBuffer(m_VBO, bufLayout);
}

engine::Chunk::~Chunk()
{
    delete[] m_Blocks;
}

void engine::Chunk::TerrainGen()
{

}

void engine::Chunk::GreedyMesh()
{

    // Working variables for the algorithm, iterators etc.
    int i, j, k, l, w, h, u, v, n = 0;
    Face side = NORTH;

    int x[3] = { 0,0,0 };
    int q[3] = { 0,0,0 };
    int du[3] = { 0,0,0 };
    int dv[3] = { 0,0,0 };

    VoxelFace voxel, voxel1 = {}; // keeping track of voxels
    bool backFace = false;
    VoxelFace null {};
    int* texCoords = new int[12]{0,0,0,0,0,0,0,0,0,0,0,0};
    VoxelFace* mask = new VoxelFace[CHUNK_SIZE * CHUNK_SIZE] {};

    for (int d = 0; d < 3; d++) {

        u = (d + 1) % 3;
        v = (d + 2) % 3;

        x[0] = 0;
        x[1] = 0;
        x[2] = 0;

        q[0] = 0;
        q[1] = 0;
        q[2] = 0;
        q[d] = 1;

        // Move through the current dimension from front to back
        for (x[d] = -1; x[d] < CHUNK_SIZE;) {
            // Compute mask
            n = 0;
            for (x[v] = 0; x[v] < CHUNK_SIZE; x[v]++) {
                for (x[u] = 0; x[u] < CHUNK_SIZE; x[u]++) {
                        
                    backFace = (x[d] < CHUNK_SIZE_MINUS_ONE ? GetBlock(x[0] + q[0],     x[1] + q[1],        x[2] + q[2]) != AIR : false);

                    if (d == 0) { side = backFace ? WEST : EAST; }
                    else if (d == 1) { side = backFace ? BOTTOM : TOP; }
                    else if (d == 2) { side = backFace ? NORTH : SOUTH; }
                    
                    voxel = (x[d] >= 0) ? GetVoxelFace(side, x[0], x[1], x[2]) : null;
                    voxel1 = (x[d] < CHUNK_SIZE_MINUS_ONE) ? GetVoxelFace(side, x[0] + q[0], x[1] + q[1], x[2] + q[2]) : null;

                    voxel.windingOrder = backFace;
                    voxel1.windingOrder = backFace;

                    mask[n++] = ((voxel != null && voxel1 != null && voxel == voxel1))
                        ? null
                        : backFace ? voxel1 : voxel;
                }
            }

            x[d]++;
            n = 0;

            for (j = 0; j < CHUNK_SIZE; j++) {
                for (i = 0; i < CHUNK_SIZE;) {
                    if (mask[n] != null) {

                        // Compute the width of quad
                        for (w = 1; i + w < CHUNK_SIZE && mask[n + w] != null && mask[n + w] == mask[n]; w++) {}

                        // Compute the height of quad
                        for (h = 1; j + h < CHUNK_SIZE; h++) {
                            for (k = 0; k < w; k++) {
                                if (mask[n + k + h * CHUNK_SIZE] == null || mask[n + k + h * CHUNK_SIZE] != mask[n]) { goto done;}
                            }
                        }
                        done:

                        // Check if face should be culled or not
                        if (!mask[n].transparent) {
                            BlockDataStruct blockData = BlockHandler::BlockData[mask[n].type];

                            x[u] = i;
                            x[v] = j;

                            du[0] = 0;
                            du[1] = 0;
                            du[2] = 0;
                            du[u] = w;

                            dv[0] = 0;
                            dv[1] = 0;
                            dv[2] = 0;
                            dv[v] = h;

                            TextureInt face;
                            switch (d) {
                            case 0: {
                                face = backFace ? blockData.left_face : blockData.right_face;
                                texCoords[0] = h;
                                texCoords[1] = 0;
                                texCoords[2] = h;
                                texCoords[3] = w;
                                texCoords[4] = 0;
                                texCoords[5] = 0;
                                texCoords[6] = h;
                                texCoords[7] = w;
                                texCoords[8] = 0;
                                texCoords[9] = w;
                                texCoords[10] = 0;
                                texCoords[11] = 0;
                                break;
                            }
                            case 1: {
                                face = backFace ? blockData.bottom_face : blockData.top_face;
                                texCoords[0] = h;
                                texCoords[1] = w;
                                texCoords[2] = h;
                                texCoords[3] = 0;
                                texCoords[4] = 0;
                                texCoords[5] = w;
                                texCoords[6] = h;
                                texCoords[7] = 0;
                                texCoords[8] = 0;
                                texCoords[9] = 0;
                                texCoords[10] = 0;
                                texCoords[11] = w;
                                break;
                            }
                            case 2: {
                                face = backFace ? blockData.back_face : blockData.front_face;
                                texCoords[0] = w;
                                texCoords[1] = 0;
                                texCoords[2] = 0;
                                texCoords[3] = 0;
                                texCoords[4] = w;
                                texCoords[5] = h;
                                texCoords[6] = 0;
                                texCoords[7] = 0;
                                texCoords[8] = 0;
                                texCoords[9] = h;
                                texCoords[10] = w;
                                texCoords[11] = h;
                                break;
                            }
                            }   
                            int topLeft[3] = {x[0] + du[0], x[1] + du[1], x[2] + du[2]};
                            int topRight[3] = {topLeft[0] + dv[0], topLeft[1] + dv[1], topLeft[2] + dv[2]};
                            int bottomRight[3] = {x[0] + dv[0], x[1] + dv[1], x[2] + dv[2]};

                            // if backface we add quad with counter clockwise winding order
                            // otherwise we use clockwise
                            AddQuadToMesh(x, topLeft, topRight, bottomRight, texCoords, mask[n].ambientOcclusion00, mask[n].ambientOcclusion01, mask[n].ambientOcclusion10, mask[n].ambientOcclusion11, face,mask[n].windingOrder);                            
                        } 

                        // Zero out mask
                        for (l = 0; l < h; ++l) {
                            for (k = 0; k < w; ++k) { mask[n + k + l * CHUNK_SIZE] = null; }
                        }
                        // Increment counters and continue
                        i += w;
                        n += w;
                    }
                    else {
                        i++;
                        n++;
                    }
                }
            }
        }
    }
    delete[] mask;
    delete[] texCoords;
}

void engine::Chunk::CreateMesh()
{
    m_Vertices.clear();
    m_Vertices.reserve((CHUNK_SIZE_CUBED / 2) * 36);
    m_VertexCount = 0;
    
    GreedyMesh();

    m_VertexCount = m_Vertices.size();
}

void engine::Chunk::AddQuadToMesh(int bottomLeft[3], int topLeft[3], int topRight[3], int bottomRight[3], int texCoords[12], int ao00, int ao01, int ao10, int ao11, BlockInt face, bool ccw) {
    if (ccw) {
        m_Vertices.emplace_back(bottomLeft[0],bottomLeft[1],bottomLeft[2],texCoords[8],texCoords[9],face,ao00);
        m_Vertices.emplace_back(bottomRight[0],bottomRight[1],bottomRight[2],texCoords[2],texCoords[3],face,ao10);
        m_Vertices.emplace_back(topRight[0],topRight[1],topRight[2],texCoords[0],texCoords[1],face,ao11);
        m_Vertices.emplace_back(bottomLeft[0],bottomLeft[1],bottomLeft[2],texCoords[8],texCoords[9],face,ao00);
        m_Vertices.emplace_back(topRight[0],topRight[1],topRight[2],texCoords[0],texCoords[1],face, ao11);
        m_Vertices.emplace_back(topLeft[0],topLeft[1],topLeft[2],texCoords[4],texCoords[5],face,ao01);
    } else {
        m_Vertices.emplace_back(bottomLeft[0],bottomLeft[1],bottomLeft[2],texCoords[8],texCoords[9],face,ao00);
        m_Vertices.emplace_back(topRight[0],topRight[1],topRight[2],texCoords[0],texCoords[1],face,ao11);
        m_Vertices.emplace_back(bottomRight[0],bottomRight[1],bottomRight[2],texCoords[2],texCoords[3],face,ao10);
        m_Vertices.emplace_back(bottomLeft[0],bottomLeft[1],bottomLeft[2],texCoords[8],texCoords[9],face,ao00);
        m_Vertices.emplace_back(topLeft[0],topLeft[1],topLeft[2],texCoords[4],texCoords[5],face,ao01);
        m_Vertices.emplace_back(topRight[0],topRight[1],topRight[2],texCoords[0],texCoords[1],face,ao11);
    }
}

engine::VoxelFace engine::Chunk::GetVoxelFace(Face side, unsigned int x, unsigned int y, unsigned int z)
{
    VoxelFace face{};
    face.type = GetBlock(x, y, z);

    switch (side) {
    case TOP: {
        face.transparent = y < CHUNK_SIZE_MINUS_ONE ? BlockHandler::BlockData[GetBlock(x, y + 1, z)].opaque :false;
        
        bool left = x > 0 && y < CHUNK_SIZE_MINUS_ONE ? GetBlock(x-1, y+1, z) != AIR : false;
        bool right = x < CHUNK_SIZE_MINUS_ONE && y < CHUNK_SIZE_MINUS_ONE ? GetBlock(x+1, y+1, z) != AIR : false;
        bool top = z > 0 && y < CHUNK_SIZE_MINUS_ONE ? GetBlock(x, y+1, z-1) != AIR : false;
        bool bottom = z < CHUNK_SIZE_MINUS_ONE && y < CHUNK_SIZE_MINUS_ONE ? GetBlock(x, y+1, z+1) != AIR : false;

        face.ambientOcclusion00 = CalculateVertexAO(
            left,
            top,
            x > 0 && z > 0 && y < CHUNK_SIZE_MINUS_ONE ? GetBlock(x-1, y+1, z-1) != AIR : false
        );
        face.ambientOcclusion01 = CalculateVertexAO(
            left,
            bottom,
            x > 0 && z < CHUNK_SIZE_MINUS_ONE && y < CHUNK_SIZE_MINUS_ONE ? GetBlock(x-1, y+1, z+1) != AIR : false
        );
        face.ambientOcclusion10 = CalculateVertexAO(
            top,
            right,
            x < CHUNK_SIZE_MINUS_ONE && z > 0 && y < CHUNK_SIZE_MINUS_ONE ? GetBlock(x+1, y+1, z-1) != AIR : false
        );
        face.ambientOcclusion11 = CalculateVertexAO(
            bottom,
            right,
            x < CHUNK_SIZE_MINUS_ONE && z < CHUNK_SIZE_MINUS_ONE && y < CHUNK_SIZE_MINUS_ONE ? GetBlock(x+1, y+1, z+1) != AIR : false
        );
        break;
    }
    case BOTTOM: {
        face.transparent = y > 0 ? BlockHandler::BlockData[GetBlock(x, y - 1, z)].opaque : false;

        bool left = x > 0 && y > 0 ? GetBlock(x-1, y-1, z) != AIR : false;
        bool right = x < CHUNK_SIZE_MINUS_ONE && y > 0 ? GetBlock(x+1, y-1, z) != AIR : false;
        bool top = z > 0 && y > 0 ? GetBlock(x, y-1, z-1) != AIR : false;
        bool bottom = z < CHUNK_SIZE_MINUS_ONE && y > 0 ? GetBlock(x, y-1, z+1) != AIR : false;

        face.ambientOcclusion00 = CalculateVertexAO(
            left,
            top,
            x > 0 && z > 0 && y > 0 ? GetBlock(x-1, y-1, z-1) != AIR : false
        );
        face.ambientOcclusion01 = CalculateVertexAO(
            left,
            bottom,
            x > 0 && z < CHUNK_SIZE_MINUS_ONE && y > 0 ? GetBlock(x-1, y-1, z+1) != AIR : false
        );
        face.ambientOcclusion10 = CalculateVertexAO(
            top,
            right,
            x < CHUNK_SIZE_MINUS_ONE && z > 0 && y > 0 ? GetBlock(x+1, y-1, z-1) != AIR : false
        );
        face.ambientOcclusion11 = CalculateVertexAO(
            bottom,
            right,
            x < CHUNK_SIZE_MINUS_ONE && z < CHUNK_SIZE_MINUS_ONE && y > 0 ? GetBlock(x+1, y-1, z+1) != AIR : false
        );
        break;
    }
    case NORTH: {
        face.transparent = z > 0  ? BlockHandler::BlockData[GetBlock(x, y, z-1)].opaque : false;

        bool top = y < CHUNK_SIZE_MINUS_ONE && z > 0 ? GetBlock(x, y + 1, z - 1) != AIR : false;
        bool bottom = y > 0 && z > 0 ? GetBlock(x, y - 1, z - 1) != AIR : false;
        bool left = x > 0 && z > 0 ? GetBlock(x -1, y, z - 1) != AIR : false;
        bool right = x < CHUNK_SIZE_MINUS_ONE && z > 0 ? GetBlock(x + 1, y, z - 1) != AIR : false;

        face.ambientOcclusion00 = CalculateVertexAO(
            bottom,
            left,
            x > 0 && y > 0 && z > 0 ? GetBlock(x-1, y-1, z-1) != AIR : false
        );
        face.ambientOcclusion10 = CalculateVertexAO(
            top,
            left,
            x > 0 && y < CHUNK_SIZE_MINUS_ONE && z > 0 ? GetBlock(x-1, y+1, z-1) != AIR : false
        );
        face.ambientOcclusion01 = CalculateVertexAO(
            bottom,
            right,
            x < CHUNK_SIZE_MINUS_ONE && y > 0 && z > 0 ? GetBlock(x + 1, y - 1, z -1) != AIR : false
        );
        face.ambientOcclusion11 = CalculateVertexAO(
            top,
            right,
            x < CHUNK_SIZE_MINUS_ONE && y < CHUNK_SIZE_MINUS_ONE && z > 0 ? GetBlock(x+1, y+1, z-1) != AIR : false
        );
        break;
    }
    case SOUTH: {
        face.transparent = z < CHUNK_SIZE_MINUS_ONE ? BlockHandler::BlockData[GetBlock(x, y, z + 1)].opaque : false;
        
        bool top = y < CHUNK_SIZE_MINUS_ONE && z < CHUNK_SIZE_MINUS_ONE ? GetBlock(x, y + 1, z + 1) != AIR : false;
        bool bottom = y > 0 && z < CHUNK_SIZE_MINUS_ONE ? GetBlock(x, y - 1, z + 1) != AIR : false;
        bool left = x > 0 && z < CHUNK_SIZE_MINUS_ONE ? GetBlock(x -1, y, z + 1) != AIR : false;
        bool right = x < CHUNK_SIZE_MINUS_ONE && z < CHUNK_SIZE_MINUS_ONE ? GetBlock(x + 1, y, z + 1) != AIR : false;

        face.ambientOcclusion00 = CalculateVertexAO(
            bottom,
            left,
            x > 0 && y > 0 && z < CHUNK_SIZE_MINUS_ONE ? GetBlock(x-1, y-1, z+1) != AIR : false
        );
        face.ambientOcclusion10 = CalculateVertexAO(
            top,
            left,
            x > 0 && y < CHUNK_SIZE_MINUS_ONE && z < CHUNK_SIZE_MINUS_ONE ? GetBlock(x-1, y+1, z+1) != AIR : false
        );
        face.ambientOcclusion01 = CalculateVertexAO(
            bottom,
            right,
            x < CHUNK_SIZE_MINUS_ONE && y > 0 && z < CHUNK_SIZE_MINUS_ONE ? GetBlock(x + 1, y - 1, z +1) != AIR : false
        );
        face.ambientOcclusion11 = CalculateVertexAO(
            top,
            right,
            x < CHUNK_SIZE_MINUS_ONE && y < CHUNK_SIZE_MINUS_ONE && z < CHUNK_SIZE_MINUS_ONE ? GetBlock(x+1, y+1, z+1) != AIR : false
        );
        break;
    }
    case EAST: {
        face.transparent = x < CHUNK_SIZE_MINUS_ONE ? BlockHandler::BlockData[GetBlock(x + 1, y, z)].opaque: false;

        bool top = x < CHUNK_SIZE_MINUS_ONE && y < CHUNK_SIZE_MINUS_ONE ? GetBlock(x+1, y+1, z) != AIR : false;
        bool bottom = x < CHUNK_SIZE_MINUS_ONE && y > 0 ? GetBlock(x+1, y-1, z) != AIR : false;
        bool left = x < CHUNK_SIZE_MINUS_ONE  && z > 0 ? GetBlock(x+1, y, z - 1) != AIR : false;
        bool right = x < CHUNK_SIZE_MINUS_ONE && z < CHUNK_SIZE_MINUS_ONE ? GetBlock(x+1, y, z + 1) != AIR : false;

        face.ambientOcclusion00 = CalculateVertexAO(
            bottom,
            left,
            x < CHUNK_SIZE_MINUS_ONE && y > 0 && z > 0 ? GetBlock(x+1, y-1, z-1) != AIR : false
        );
        face.ambientOcclusion01 = CalculateVertexAO(
            top,
            left,
            x < CHUNK_SIZE_MINUS_ONE && y < CHUNK_SIZE_MINUS_ONE && z > 0 ? GetBlock(x+1, y+1, z-1) != AIR : false
        );
        face.ambientOcclusion10 = CalculateVertexAO(
            bottom,
            right,
            x < CHUNK_SIZE_MINUS_ONE && y > 0 && z < CHUNK_SIZE_MINUS_ONE ? GetBlock(x+1, y-1, z+1) != AIR : false
        );
        face.ambientOcclusion11 = CalculateVertexAO(
            top,
            right,
            x < CHUNK_SIZE_MINUS_ONE && y < CHUNK_SIZE_MINUS_ONE && z < CHUNK_SIZE_MINUS_ONE ? GetBlock(x+1, y+1, z+1) != AIR : false
        );
        break;
    }
    case WEST: {
        face.transparent = x > 0 ? BlockHandler::BlockData[GetBlock(x - 1, y, z)].opaque : false;

        bool top = x > 0 && y < CHUNK_SIZE_MINUS_ONE ? GetBlock(x-1, y+1, z) != AIR : false;
        bool bottom = x > 0 && y > 0 ? GetBlock(x-1, y-1, z) != AIR : false;
        bool left = x > 0  && z > 0 ? GetBlock(x-1, y, z - 1) != AIR : false;
        bool right = x > 0 && z < CHUNK_SIZE_MINUS_ONE ? GetBlock(x-1, y, z + 1) != AIR : false;

        face.ambientOcclusion00 = CalculateVertexAO(
            bottom,
            left,
            x > 0 && y > 0 && z > 0 ? GetBlock(x-1, y-1, z-1) != AIR : false
        );
        face.ambientOcclusion01 = CalculateVertexAO(
            top,
            left,
            x > 0 && y < CHUNK_SIZE_MINUS_ONE && z > 0 ? GetBlock(x-1, y+1, z-1) != AIR : false
        );
        face.ambientOcclusion10 = CalculateVertexAO(
            bottom,
            right,
            x > 0 && y > 0 && z < CHUNK_SIZE_MINUS_ONE ? GetBlock(x-1, y-1, z+1) != AIR : false
        );
        face.ambientOcclusion11 = CalculateVertexAO(
            top,
            right,
            x > 0 && y < CHUNK_SIZE_MINUS_ONE && z < CHUNK_SIZE_MINUS_ONE ? GetBlock(x-1, y+1, z+1) != AIR : false
        );
        break;
    }
    }
    return face;
}

void engine::Chunk::BufferData()
{
    m_VBO.BufferData(m_Vertices.data(), m_VertexCount * sizeof(Vertex));
    m_VBO.Unbind();
    m_Vertices.clear();
}

void engine::Chunk::Draw(Shader& shader)
{
    m_VAO.Bind();
    shader.setMat4("model", m_Model);
    glDrawArrays(GL_TRIANGLES, 0, m_VertexCount);

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