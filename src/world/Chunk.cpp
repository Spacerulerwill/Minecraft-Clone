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

    
    VertexBufferLayout bufLayout;
	bufLayout.AddAttribute<float>(3);
    bufLayout.AddAttribute<float>(3);
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

    /*
    * This mask will contain the groups of matching voxel faces
    * as we proceed through the chunk in 6 directions. One for each side of ac ube
    */
    VoxelFace* mask = new VoxelFace[CHUNK_SIZE_SQUARED] {};
    VoxelFace voxel, voxel1 = {}; // keeping track of voxels
    VoxelFace null {};
    int* texCoords = new int[12]{};

    /*
    * An unccomon application of a boolean for loop, on the first iteration backFace will be true
    * and the second it will be false. Allows us to keep track of voxel side direction.
    * 
    * The boolean for loop runs twice and the inner 3 for each dimension, so iterates 6 times - 
    * one for each face
    */
    for (bool backFace = true, b = false; b != backFace; backFace = backFace && b, b = !b) {
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

            if (d == 0) { side = backFace ? WEST : EAST; }
            else if (d == 1) { side = backFace ? BOTTOM : TOP; }
            else if (d == 2) { side = backFace ? NORTH : SOUTH; }

            // Move through the current dimension from front to back
            for (x[d] = -1; x[d] < CHUNK_SIZE;) {
                // Compute mask
                n = 0;
                for (x[v] = 0; x[v] < CHUNK_SIZE; x[v]++) {
                    for (x[u] = 0; x[u] < CHUNK_SIZE; x[u]++) {
                        voxel = (x[d] >= 0) ? GetVoxelFace(side, x[0], x[1], x[2]) : null;
                        voxel1 = (x[d] < CHUNK_SIZE_MINUS_ONE) ? GetVoxelFace(side, x[0] + q[0], x[1] + q[1], x[2] + q[2]) : null;

                        mask[n++] = ((voxel != null && voxel1 != null && voxel == voxel1))
                            ? null // TODO check
                            : backFace ? voxel1 : voxel;
                    }
                }

                x[d]++;
                n = 0;

                for (j = 0; j < CHUNK_SIZE; j++) {
                    for (i = 0; i < CHUNK_SIZE;) {
                        if (mask[n] != null) {
                            BlockDataStruct blockData = BlockHandler::BlockData[mask[n].type];

                            // Compute the width of quad
                            for (w = 1; i + w < CHUNK_SIZE && mask[n + w] != null && mask[n + w] == mask[n]; w++) {}

                            // Compute the height of quad
                            bool done = false;

                            for (h = 1; j + h < CHUNK_SIZE; h++) {
                                for (k = 0; k < w; k++) {
                                    if (mask[n + k + h * CHUNK_SIZE] == null || mask[n + k + h * CHUNK_SIZE] != mask[n]) { done = true; break; }
                                }
                                if (done) { break; }
                            }

                            // Check if face should be culled or not
                            if (!mask[n].transparent) {
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

                                // Add vertices for a quad
                                m_Vertices.emplace_back(
                                    x[0] + du[0] + dv[0],
                                    x[1] + du[1] + dv[1],
                                    x[2] + du[2] + dv[2],
                                    texCoords[0],
                                    texCoords[1],
                                    face
                                );

                                m_Vertices.emplace_back(
                                    x[0] + dv[0],
                                    x[1] + dv[1],
                                    x[2] + dv[2],
                                    texCoords[2],
                                    texCoords[3],
                                    face
                                );

                                m_Vertices.emplace_back(
                                    x[0] + du[0],
                                    x[1] + du[1],
                                    x[2] + du[2],
                                    texCoords[4],
                                    texCoords[5],
                                    face
                                );

                                m_Vertices.emplace_back(
                                    x[0] + dv[0],
                                    x[1] + dv[1],
                                    x[2] + dv[2],
                                    texCoords[6],
                                    texCoords[7],
                                    face
                                );

                                m_Vertices.emplace_back(
                                    x[0],
                                    x[1],
                                    x[2],
                                    texCoords[8],
                                    texCoords[9],
                                    face
                                );

                                m_Vertices.emplace_back(
                                    x[0] + du[0],
                                    x[1] + du[1],
                                    x[2] + du[2],
                                    texCoords[10],
                                    texCoords[11],
                                    face
                                );
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

engine::VoxelFace engine::Chunk::GetVoxelFace(Face side, unsigned int x, unsigned int y, unsigned int z)
{
    VoxelFace face{};
    face.type = GetBlock(x, y, z);

    switch (side) {
    case TOP: {
        face.transparent = y < CHUNK_SIZE_MINUS_ONE ? BlockHandler::BlockData[GetBlock(x, y + 1, z)].opaque :false;
        break;
    }
    case BOTTOM: {
        face.transparent = y > 0 ? BlockHandler::BlockData[GetBlock(x, y - 1, z)].opaque : false;
        break;
    }
    case NORTH: {
        face.transparent = z > 0  ? BlockHandler::BlockData[GetBlock(x, y, z-1)].opaque : false;
        break;
    }
    case SOUTH: {
        face.transparent = z < CHUNK_SIZE_MINUS_ONE ? BlockHandler::BlockData[GetBlock(x, y, z + 1)].opaque : false;
        break;
    }
    case EAST: {
        face.transparent = x < CHUNK_SIZE_MINUS_ONE ? BlockHandler::BlockData[GetBlock(x + 1, y, z)].opaque: false;
        break;
    }
    case WEST: {
        face.transparent = x > 0 ? BlockHandler::BlockData[GetBlock(x - 1, y, z)].opaque : false;
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

void engine::Chunk::SetBlock(BlockInt block, unsigned int x, unsigned int y, unsigned int z)
{
    m_Blocks[x + (y << CHUNK_EXP) + (z << CHUNK_EXP_TIMES_TWO)] = block;
}

engine::BlockInt engine::Chunk::GetBlock(unsigned int x, unsigned int y, unsigned int z) const
{
    return m_Blocks[x + (y << CHUNK_EXP) + (z << CHUNK_EXP_TIMES_TWO)];
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