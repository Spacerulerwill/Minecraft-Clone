/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include <math/Raycast.hpp>
#include <world/World.hpp>
#include <util/Log.hpp>

Raycaster::BlockRaycastResult Raycaster::BlockRaycast(const World& world, glm::vec3 start, glm::vec3 direction, float distance)
{
    glm::vec3 end = start + direction * distance;
    glm::ivec3 normal = glm::ivec3(0);
    
    glm::ivec3 currentBlock = glm::ivec3(
        static_cast<int>(floorf(start[0])),
        static_cast<int>(floorf(start[1])),
        static_cast<int>(floorf(start[2]))
    );

    const glm::ivec3 endBlock = glm::ivec3(
        static_cast<int>(floorf(end[0])),
        static_cast<int>(floorf(end[1])),
        static_cast<int>(floorf(end[2]))
    );

    const glm::ivec3 d = glm::ivec3(
        ((start[0] < end[0]) ? 1 : ((start[0] > end[0]) ? -1 : 0)),
        ((start[1] < end[1]) ? 1 : ((start[1] > end[1]) ? -1 : 0)),
        ((start[2] < end[2]) ? 1 : ((start[2] > end[2]) ? -1 : 0))
    );

    const glm::vec3 deltat = glm::vec3(
        1.0f / std::abs(end[0] - start[0]),
        1.0f / std::abs(end[1] - start[1]),
        1.0f / std::abs(end[2] - start[2])
    );

    const glm::vec3 min  = glm::vec3(
        floorf(start[0]),
        floorf(start[1]),
        floorf(start[2])
    );

    const glm::vec3 max = min + glm::vec3(1.0f);

    glm::vec3 t = glm::vec3(
        ((start[0] > end[0]) ? (start[0] - min[0]) : (max[0] - start[0])) * deltat[0],
        ((start[1] > end[1]) ? (start[1] - min[1]) : (max[1] - start[1])) * deltat[1],
        ((start[2] > end[2]) ? (start[2] - min[2]) : (max[2] - start[2])) * deltat[2]
    );

    while (true)
    {
        glm::ivec3 chunkPos = GetChunkPosFromGlobalBlockPos(currentBlock);
        std::shared_ptr<Chunk> chunk = world.GetChunk(chunkPos);
        glm::ivec3 blockPosInChunk = GetChunkBlockPosFromGlobalBlockPos(currentBlock);
        if (chunk != nullptr) {
            // If we do hit a chunk, check if its air. If it's not then we stop the raycasting otherwise we continue
            Block block = chunk->GetBlock(blockPosInChunk);
            BlockType type = block.GetType();
            if (!GetBlockData(type).canInteractThrough || block.IsWaterLogged()) {
                return BlockRaycastResult{
                    chunk,
                    blockPosInChunk,
                    normal,
                    block
                };
            }
        }
        // Raycasting
        if (t[0] <= t[1] && t[0] <= t[2])
        {
            if (currentBlock[0] == endBlock[0]) {
                Block block = chunk != nullptr ? chunk->GetBlock(blockPosInChunk) : Block(BlockType::AIR, 0, false);
                return BlockRaycastResult{
                    chunk,
                    blockPosInChunk,
                    normal,
                    block
                };
            };
            t[0] += deltat[0];
            normal = glm::ivec3(
                -d[0],0,0
            );
            currentBlock[0] += d[0];
        }
        else if (t[1] <= t[2])
        {
            if (currentBlock[1] == endBlock[1]) {
                Block block = chunk != nullptr ? chunk->GetBlock(blockPosInChunk) : Block(BlockType::AIR, 0, false);
                return BlockRaycastResult{
                    chunk,
                    blockPosInChunk,
                    normal,
                    block
                };
            };
            t[1] += deltat[1];
            normal = glm::ivec3(
                0,-d[1],0
            );
            currentBlock[1] += d[1];
        }
        else
        {
            if (currentBlock[2] == endBlock[2]) {
                Block block = chunk != nullptr ? chunk->GetBlock(blockPosInChunk) : Block(BlockType::AIR, 0, false);
                return BlockRaycastResult{
                    chunk,
                    blockPosInChunk,
                    normal,
                    block
                };
            };
            t[2] += deltat[2];
            normal = glm::ivec3(
               0,0,-d[2]
            );
            currentBlock[2] += d[2];
        }
    }
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
