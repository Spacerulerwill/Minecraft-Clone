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

namespace engine {
	constexpr int CHUNK_EXP = 6;
	constexpr int CHUNK_EXP_TIMES_TWO = CHUNK_EXP * 2;
	constexpr int CHUNK_SIZE = 1 << CHUNK_EXP;
	constexpr int CHUNK_SIZE_MINUS_ONE = CHUNK_SIZE - 1;
	constexpr int CHUNK_SIZE_SQUARED = CHUNK_SIZE * CHUNK_SIZE;
	constexpr int CHUNK_SIZE_CUBED = CHUNK_SIZE_SQUARED * CHUNK_SIZE;

	enum Face {
		NORTH,
		SOUTH,
		EAST,
		WEST,
		TOP,
		BOTTOM
	};

	constexpr float BLOCK_SCALE = 0.2f;
	constexpr float INV_BLOCK_SCALE = 1 / BLOCK_SCALE;

	struct Vertex {
		float x;
		float y;
		float z;
		float texX;
		float texY; 
		float texZ;
	};

	struct VoxelFace {
		BlockInt type = AIR;
		bool transparent = true;

		bool operator==(const VoxelFace& other) {
			return type == other.type &&
				transparent == other.transparent;
		}

		bool operator!=(const VoxelFace& other) {
			return !(*this == other);
		}
	};

	class Chunk {
	private:
		int chunkX = 0;
		int chunkY = 0;
		int chunkZ = 0;
		BlockInt* m_Blocks = new BlockInt[CHUNK_SIZE_CUBED];
		VertexBuffer m_VBO;
		VertexArray m_VAO;
		Mat4 m_Model = scale(Vec3(BLOCK_SCALE));
		int m_VertexCount = 0;
		std::vector<Vertex> m_Vertices;
		[[nodiscard]] VoxelFace GetVoxelFace(Face side, unsigned int x, unsigned int y, unsigned int z);
		void AddQuadToMesh(int bottomLeft[3], int topLeft[3], int topRight[3], int bottomRight[3], int texCoords[12], BlockInt face, bool backFace);

	public:
		Chunk(int chunkX, int chunkY, int chunkZ);
		~Chunk();
		void TerrainGen();
		void GreedyMesh();
		void CreateMesh();
		void BufferData();
		void Draw(Shader& shader);
		void SetBlock(BlockInt block, unsigned int x, unsigned int y, unsigned int z);
		[[nodiscard]] BlockInt GetBlock(unsigned int x, unsigned int y, unsigned int z) const;
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