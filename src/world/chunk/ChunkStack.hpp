/*
Copyright (C) 2023 William Redding - All Rights Reserved
LICENSE: MIT
*/

#ifndef CHUNKSTACK_HPP
#define CHUNKSTACK_HPP

#include <util/Log.hpp>
#include <world/chunk/Chunk.hpp>
#include <world/Block.hpp>
#include <math/Vec3.hpp>
#include <math/Vec2.hpp>
#include <PerlinNoise.hpp>
#include <random>
#include <vector>

/*
A ChunkStack represents a vertical column of chunks that can grow and shrink
as the player builds
*/
namespace engine {
    class ChunkStack {
    private:
        std::vector<Chunk> m_Chunks;
        Vec2<int> m_Pos;
    public:
        ChunkStack(Vec2<int> pos);
        Vec2<int> GetPos() const;
        void DrawOpaque(Shader& opaqueShader);
        void DrawWater(Shader& waterShader);
        void DrawCustomModel(Shader& customModelShader);
        void SetBlock(BlockInt block, int x, int y, int z);
        void GenerateTerrain(const siv::PerlinNoise& perlin, std::mt19937& gen, std::uniform_int_distribution<>& distrib);
        void MeshAndBufferChunks();
        BlockInt GetBlock(int x, int y, int z) const; 
    };
}

#endif // !CHUNKSTACK_HPP

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