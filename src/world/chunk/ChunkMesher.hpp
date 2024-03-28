/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#ifndef CHUNK_MESHER_H
#define CHUNK_MESHER_H

#include <vector>
#include <cstdint>
#include <world/Block.hpp>
#include <functional>

namespace ChunkMesher {
    typedef bool (*ChunkMeshFilterCallback)(BlockType);

    struct ChunkVertex {
        uint32_t data1;
        uint32_t data2;
    };

    inline bool IsOpaqueCube(BlockType blockType) {
        BlockDataStruct blockData = GetBlockData(blockType);
        return blockData.opaque && blockData.modelID == static_cast<ModelID>(Model::CUBE);
    }

    void BinaryGreedyMesh(std::vector<ChunkVertex>& vertices, const std::vector<Block>& blocks, ChunkMeshFilterCallback condition);
    void MeshCustomModelBlocks(std::vector<ChunkVertex>& vertices, const std::vector<Block>& blocks);
};

#endif // !CHUNK_MESHER_H

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
