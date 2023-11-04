/*
Copyright (C) 2023 William Redding - All Rights Reserved
LICENSE: MIT
*/

#ifndef CHUNK_H
#define CHUNK_H

#include <cstdint>
#include <cstring>
#include <vector>
#include <opengl/BufferObject.hpp>
#include <opengl/VertexArray.hpp>
#include <core/Shader.hpp>
#include <math/Mat4.hpp>
#include <math/Math.hpp>
#include <math/Vec4.hpp>
#include <math/Vec3.hpp>
#include <world/Block.hpp>
#include <world/chunk/ChunkMesher.hpp>
#include <PerlinNoise.hpp>
#include <util/Constants.hpp>

namespace engine {

	constexpr float BLOCK_SCALE = 1.0f;
    constexpr float CHUNK_SCALE = CS * BLOCK_SCALE;
	constexpr float INV_BLOCK_SCALE = 1 / BLOCK_SCALE;

    inline int voxelIndex(int x, int y, int z) {
        return y + (x << CHUNK_SIZE_EXP) + (z << CHUNK_SIZE_EXP_X2);
    }

    /*
    Chunks are 62^3 and are stored in vertical columns for memory efficency when
    setting blocks. Voxels are stored in a 64^3 array of integers, utilizing a layer
    of padding on each side to remove the need for any bounds checking. Neighbor chunks
    edge data are copied into the padding layers to allow for cross chunk meshing.

    Each chunk has 3 vertex buffers:
    * A regular vertex buffer - 8 byte vertices
    * A water vertex buffer - 8 byte vertices
    * A custom block model vertex buffer -24 byte vertices
    
    Due to data constraints, the vertex data for each cube block can be packed to 8 bytes.
    See engine::CubeChunkVertex for for information on the packing layout.

    Custom model blocks, cannot be packed to 8 bytes
    */
	class Chunk {
	private:        
		BufferObject<GL_ARRAY_BUFFER> m_VBO;
		VertexArray m_VAO;
		std::vector<CubeChunkVertex> m_Vertices;
		size_t m_VertexCount = 0;

		BufferObject<GL_ARRAY_BUFFER> m_WaterVBO;
		VertexArray m_WaterVAO;
		std::vector<CubeChunkVertex> m_WaterVertices;
		size_t m_WaterVertexCount = 0;

        BufferObject<GL_ARRAY_BUFFER> m_CustomModelVBO;
		VertexArray m_CustomModelVAO;
		std::vector<CustomModelChunkVertex> m_CustomModelVertices;
		size_t m_CustomModelVertexCount = 0;

		Mat4<float> m_Model = Mat4<float>(1.0f);

        void AddVertexBufferAttributes();
        void SetupModelMatrix(Vec3<int> chunkPos);
        
	public:
        Vec3<int> m_Pos = Vec3<int>(0);
    	BlockInt* m_Voxels = new BlockInt[CS_P3] {};
        
        Chunk();
        Chunk(Vec3<int> chunkPos);
		~Chunk();
        Chunk(Chunk&& other); // move constructor
        Chunk& operator=(Chunk&& other); // move assignment
        
		void CreateMesh();
		void BufferData();
        
		void DrawOpaque(Shader& shader);
		void DrawWater(Shader& shader);
        void DrawCustomModelBlocks(Shader& shader);

		inline BlockInt GetBlock(int x, int y, int z) const {
			return m_Voxels[voxelIndex(x,y,z)];
		}

        inline BlockInt GetBlock(Vec3<int> pos) {
            return m_Voxels[voxelIndex(pos.x, pos.y, pos.z)];
        }

		inline void SetBlock(BlockInt block, int x, int y, int z) {
			m_Voxels[voxelIndex(x,y,z)] = block;
		}

        inline void SetBlockVerticalColumn(BlockInt block, Vec3<int> start, int length) {
            memset(m_Voxels + voxelIndex(start.x, start.y, start.z), block, length * sizeof(BlockInt));
        }

        inline void SetBlock(BlockInt block, Vec3<int> pos) {
			m_Voxels[voxelIndex(pos.x,pos.y,pos.z)] = block;
		}
    };
}

#endif // !CHUNK_H

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