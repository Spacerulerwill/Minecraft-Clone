/*
Copyright (C) 2023 William Redding - All Rights Reserved
LICENSE: MIT
*/

#ifndef CHUNKMESHER_H
#define CHUNKMESHER_H

#include <world/Block.hpp>
#include <util/Constants.hpp>
#include <vector>

namespace engine {
    struct ChunkVertex {
		uint32_t dat1; // Contains 6 bits for x, 6 bits for y, 6 bits for z, 3 bits for normal direction. Total 21 bits
		uint32_t dat2; // Contains 6 bits for texX, 6 bits for texY, 8 bits for texZ. Total 20 bits
	};

    void GreedyTranslucent(std::vector<ChunkVertex>& vertices, std::vector<ChunkVertex>& waterVertices, const engine::BlockInt* voxels);
    void GreedyOpaque(std::vector<ChunkVertex>& vertices, const engine::BlockInt* voxels);
    void MeshCustomModelBlocks(std::vector<float>& vertices, const engine::BlockInt* voxels);

    inline const ChunkVertex GetVertex(uint32_t x, uint32_t y, uint32_t z, uint32_t texX, uint32_t texY, uint32_t type, uint32_t norm) {
        return {
            (norm << 18) | ((z - 1) << 12) | ((y - 1) << 6) | (x - 1),
            (type << 12) | ((texY) << 6) | (texX)
        };
    }
};

#endif // !CHUNKMESHER_H

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