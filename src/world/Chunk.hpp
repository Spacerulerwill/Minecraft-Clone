/*
Copyright (C) 2023 William Redding - All Rights Reserved
LICENSE: MIT
*/

#ifndef CHUNK_H
#define CHUNK_H

#include <cstdint>
#include <vector>
#include <opengl/VertexBuffer.hpp>
#include <opengl/VertexArray.hpp>
#include <core/Shader.hpp>
#include <math/Mat4.hpp>
#include <math/Math.hpp>
#include <math/Vec4.hpp>
#include <world/Block.hpp>
#include <world/ChunkMesher.hpp>
#include <PerlinNoise.hpp>
#include <util/Constants.hpp>

#define VOXEL_INDEX(x,y,z) (z) + ((x) << CHUNK_SIZE_EXP) + ((y) << CHUNK_SIZE_EXP_X2)

namespace engine {

	constexpr float BLOCK_SCALE = 0.2f;
    constexpr float CHUNK_SCALE = CS * BLOCK_SCALE;
	constexpr float INV_BLOCK_SCALE = 1 / BLOCK_SCALE;

    /*
    Chunks are 62^3 and are stored in vertical columns for memory efficency when
    setting blocks. Voxels are stored in a 64^3 array of integers, utilizing a layer
    of padding on each side to remove the need for any bounds checking. Neighbor chunks
    edge data are copied into the padding layers to allow for cross chunk meshing.

    Each chunk has 3 vertex buffers:
    * A regular vertex buffer
    * A water vertex buffer
    * A custom block model vertex buffer
    
    The first 2 buffers used vertexs packed to 8 bytes, however the vertices in the custom
    block model buffer cannot be packed and are each 6 floats, totalling 24 bytes
    */
	class Chunk {
	private:
		int chunkX = 0;
		int chunkY = 0;
		int chunkZ = 0;

		BlockInt* m_Voxels = new BlockInt[CS_P3];

		VertexBuffer m_VBO;
		VertexArray m_VAO;
		std::vector<ChunkVertex> m_Vertices;
		size_t m_VertexCount = 0;

		VertexBuffer m_WaterVBO;
		VertexArray m_WaterVAO;
		std::vector<ChunkVertex> m_WaterVertices;
		size_t m_WaterVertexCount = 0;

        VertexBuffer m_CustomModelVBO;
		VertexArray m_CustomModelVAO;
		std::vector<float> m_CustomModelVertices;
		size_t m_CustomModelVertexCount = 0;

		Mat4 m_Model = scale(Vec3(BLOCK_SCALE));
	public:
		Chunk(int chunkX, int chunkY, int chunkZ);
		~Chunk();
        
		void TerrainGen(const siv::PerlinNoise& perlin);
		void CreateMesh();

		void BufferData();
		void Draw(Shader& shader);
		void DrawWater(Shader& shader);
        void DrawCustomModelBlocks(Shader& shader);

		[[nodiscard]] inline BlockInt GetBlock(unsigned int x, unsigned int y, unsigned int z) const {
			return m_Voxels[VOXEL_INDEX(x,y,z)];
		}

		inline void SetBlock(BlockInt block, unsigned int x, unsigned int y, unsigned int z) {
			m_Voxels[VOXEL_INDEX(x,y,z)] = block;
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