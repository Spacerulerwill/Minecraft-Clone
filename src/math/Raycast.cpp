/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include <math/Raycast.hpp>
#include <world/World.hpp>
#include <util/Log.hpp>

Raycaster::BlockRaycastResult Raycaster::BlockRaycast(const World& world, Vec3 start, Vec3 direction, float distance)
{
    Vec3 end = start + direction * distance;
    iVec3 normal{ 0,0,0 };
    
    iVec3 currentBlock {
        static_cast<int>(floorf(start[0])),
        static_cast<int>(floorf(start[1])),
        static_cast<int>(floorf(start[2]))
    };

    const iVec3 endBlock{
        static_cast<int>(floorf(end[0])),
        static_cast<int>(floorf(end[1])),
        static_cast<int>(floorf(end[2]))
    };

    const iVec3 d {
        ((start[0] < end[0]) ? 1 : ((start[0] > end[0]) ? -1 : 0)),
        ((start[1] < end[1]) ? 1 : ((start[1] > end[1]) ? -1 : 0)),
        ((start[2] < end[2]) ? 1 : ((start[2] > end[2]) ? -1 : 0))
    };

    const Vec3 deltat{
        1.0f / std::abs(end[0] - start[0]),
        1.0f / std::abs(end[1] - start[1]),
        1.0f / std::abs(end[2] - start[2])
    };

    const Vec3 min {
        floorf(start[0]),
        floorf(start[1]),
        floorf(start[2])
    };

    const Vec3 max = min + Vec3{1.0f, 1.0f, 1.0f};

    Vec3 t {
        ((start[0] > end[0]) ? (start[0] - min[0]) : (max[0] - start[0])) * deltat[0],
        ((start[1] > end[1]) ? (start[1] - min[1]) : (max[1] - start[1])) * deltat[1],
        ((start[2] > end[2]) ? (start[2] - min[2]) : (max[2] - start[2])) * deltat[2]
    };



    while (true)
    {
        iVec3 chunkPos = GetChunkPosFromGlobalBlockPos(currentBlock);
        std::shared_ptr<Chunk> chunk = world.GetChunk(chunkPos);
        iVec3 blockPosInChunk = GetChunkBlockPosFromGlobalBlockPos(currentBlock);
        if (chunk != nullptr) {
            // If we do hit a chunk, check if its air. If it's not then we stop the raycasting otherwise we continue
            BlockID block = chunk->GetBlock(blockPosInChunk);
            if (!BlockData[block].canInteractThrough) {
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
                BlockID block = chunk != nullptr ? chunk->GetBlock(blockPosInChunk) : AIR;
                return BlockRaycastResult{
                    chunk,
                    blockPosInChunk,
                    normal,
                    block
                };
            };
            t[0] += deltat[0];
            normal = iVec3{
                -d[0],0,0
            };
            currentBlock[0] += d[0];
        }
        else if (t[1] <= t[2])
        {
            if (currentBlock[1] == endBlock[1]) {
                BlockID block = chunk != nullptr ? chunk->GetBlock(blockPosInChunk) : AIR;
                return BlockRaycastResult{
                    chunk,
                    blockPosInChunk,
                    normal,
                    block
                };
            };
            t[1] += deltat[1];
            normal = iVec3{
                0,-d[1],0
            };
            currentBlock[1] += d[1];
        }
        else
        {
            if (currentBlock[2] == endBlock[2]) {
                BlockID block = chunk != nullptr ? chunk->GetBlock(blockPosInChunk) : AIR;
                return BlockRaycastResult{
                    chunk,
                    blockPosInChunk,
                    normal,
                    block
                };
            };
            t[2] += deltat[2];
            normal = iVec3{
               0,0,-d[2]
            };
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
