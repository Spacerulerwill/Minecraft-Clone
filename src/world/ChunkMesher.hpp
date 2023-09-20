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
        /*
        dat1 contains:
        * 6 bits for x coordinate (0-63)
        * 6 bits for y coordinate (0-63)
        * 6 bits for z coordinate (0-63)
        * 3 bits for normal direction (0-7 (only 0-5 needed))
        Total 21/32 bits used
        */
		uint32_t dat1;
        /*
        dat2 contains:
        6 bits for x texture coordinate (0-63)
        6 bits for y texture coordinate (0-63)
        8 bits for z texture coordinate (0-255)
        1 bit for if the block type is grass
        1 bit for if the block type is foliage
        Total 22/32 bits used
        */
		uint32_t dat2;
	};

    /*
    GreedyTranslucent greedy meshes the non opaque cubic voxels in the mesh. If the voxel is water the vertices are added to their own
    water vertex vector otherwise they are added to the default vertex vector.
    */
    void GreedyTranslucent(std::vector<ChunkVertex>& vertices, std::vector<ChunkVertex>& waterVertices, const engine::BlockInt* voxels);
    // Greedy opaque greedy meshes all the opaque cubic voxels and adds them to the default vertex vector
    void GreedyOpaque(std::vector<ChunkVertex>& vertices, const engine::BlockInt* voxels);
    // MeshCustomModelBlocks adds the model vertices for non cubic voxels to a custom block vertex vector. It performs face culling and
    // will not add the model if it is surrounded by opaque voxels in all directions
    void MeshCustomModelBlocks(std::vector<float>& vertices, const engine::BlockInt* voxels);

    /*
    Pack all the vertex data into a ChunkVertex struct, to save space
    */
    inline const ChunkVertex GetVertex(uint32_t x, uint32_t y, uint32_t z, uint32_t texX, uint32_t texY, uint32_t type, uint32_t norm, bool isGrass, bool isFoliage) {
        return {
            (norm << 18) | ((z - 1) << 12) | ((y - 1) << 6) | (x - 1),
            (isGrass << 21) | (isFoliage << 20) | (type << 12) | ((texY) << 6) | (texX)
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