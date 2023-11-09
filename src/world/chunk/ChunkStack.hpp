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
        using iterator = std::vector<Chunk>::iterator;
        using const_iterator = std::vector<Chunk>::const_iterator;
        ChunkStack(Vec2<int> pos);
        
        // Get the world space chunk region position
        Vec2<int> GetPos() const;

        /*
        Drawing functions for the 3 components of a chunk mesh.
        They iterate over the chunks in the chunk stack and call 
        their respective drawing functions of the same name.
        */
        void DrawOpaque(Shader& opaqueShader);
        void DrawWater(Shader& waterShader);
        void DrawCustomModel(Shader& customModelShader);

        // Set a block in the chunk stack using chunk stack local block coordinates
        void SetBlock(BlockInt block, Vec3<int> pos);

        // Get a block from the chunk using chunk stack local block coordinates
        BlockInt GetBlock(Vec3<int> pos) const; 

        // Get a chunk from the chunkstack using its vertical position
        Chunk* GetChunk(int y);
        
        // Generate the terrain for the chunks in this chunkstack using perlin noise
        void GenerateTerrain(const siv::PerlinNoise& perlin, std::mt19937& gen, std::uniform_int_distribution<>& distrib);

        // Functions that mimic certain parts of the std::vector interface
        iterator begin();
        iterator end();
        const_iterator cbegin() const;
        const_iterator cend() const;
        size_t size() const;

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