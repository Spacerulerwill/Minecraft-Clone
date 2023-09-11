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
#include <PerlinNoise.hpp>

namespace engine {

	// Chunks are stored as 
	// Chunks are stored in Columns going down X and then Z
	// This is to make setting columns of blocks more memory efficient
	// When generating terrain

	// Chunk size exponents (used when calculating array index to find block at x,y,z)
	constexpr int CHUNK_SIZE_EXP = 6;
	constexpr int CHUNK_SIZE_EXP_X2 = CHUNK_SIZE_EXP * 2;

	// Chunk sizes with padding
	constexpr int CS_P = 1 << CHUNK_SIZE_EXP;
	constexpr int CS_P2 = 1 << CHUNK_SIZE_EXP_X2;
	constexpr int CS_P3 = CS_P2 * CS_P;

	constexpr int CS_P_MINUS_ONE  = CS_P -1;

	// Chunk size without paddings
	constexpr int CS = CS_P - 2;
	constexpr int CS_MINUS_ONE = CS - 1;

	constexpr float BLOCK_SCALE = 0.2f;
	constexpr float INV_BLOCK_SCALE = 1 / BLOCK_SCALE;

	struct ChunkVertex {
		uint32_t dat1; // Contains 6 bits for x, 6 bits for y, 6 bits for z, 3 bits for normal direction. Total 21 bits
		uint32_t dat2; // Contains 6 bits for texX, 6 bits for texY, 8 bits for texZ. Total 20 bits
	};

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

		Mat4 m_Model = scale(Vec3(BLOCK_SCALE));
	public:
		Chunk(int chunkX, int chunkY, int chunkZ);
		~Chunk();
		void TerrainGen(const siv::PerlinNoise& perlin);
		void GreedyTranslucent();
		void GreedyOpaque();
		void CreateMesh();

		void BufferData();
		void Draw(Shader& shader);
		void DrawWater(Shader& shader);

		inline const bool SolidCheck(BlockInt voxel) {
			return voxel > 0;
		}

		inline const int GetAxisI(const int &axis, const int &a, const int &b, const int &c) {
			if (axis == 0) return b + (a * CS_P) + (c * CS_P2);
			else if (axis == 1) return a + (c * CS_P) + (b* CS_P2);
			else return c + (b * CS_P) + (a * CS_P2);
		}

		inline const bool CompareForward(int axis, int forward, int right, int bit_pos) {
			return m_Voxels[GetAxisI(axis, right, forward, bit_pos)] == m_Voxels[GetAxisI(axis, right, forward + 1, bit_pos)];
		}

		inline const bool CompareRight(int axis, int forward, int right, int bit_pos) {
			return m_Voxels[GetAxisI(axis, right, forward, bit_pos)] == m_Voxels[GetAxisI(axis, right + 1, forward, bit_pos)];
		}

		inline const ChunkVertex GetVertex(uint32_t x, uint32_t y, uint32_t z, uint32_t texX, uint32_t texY, uint32_t type, uint32_t norm) {
  			return {
				(norm << 18) | ((z - 1) << 12) | ((y - 1) << 6) | (x - 1),
				(type << 12) | ((texY) << 6) | (texX)
			};
  		}

		inline const void InsertQuad(bool water, ChunkVertex v1, ChunkVertex v2, ChunkVertex v3, ChunkVertex v4) {
			if (water) {
				m_WaterVertices.insert(m_WaterVertices.end(), { v1, v2, v4, v2, v3, v4 });
			} else {
				m_Vertices.insert(m_Vertices.end(), { v1, v2, v4, v2, v3, v4 });
			}
		}

		[[nodiscard]] inline BlockInt GetBlock(unsigned int x, unsigned int y, unsigned int z) const {
			return m_Voxels[z + (x << CHUNK_SIZE_EXP) + (y << CHUNK_SIZE_EXP_X2)];
		}

		inline void SetBlock(BlockInt block, unsigned int x, unsigned int y, unsigned int z) {
			m_Voxels[z + (x << CHUNK_SIZE_EXP) + (y << CHUNK_SIZE_EXP_X2)] = block;
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