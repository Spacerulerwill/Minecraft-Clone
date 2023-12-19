/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include <math/Raycast.hpp>
#include <world/World.hpp>
#include <util/Log.hpp>

Raycaster::BlockRaycastResult Raycaster::BlockRaycast(World* world, Vec3 start, Vec3 direction, float distance)
{
    Vec3 end = start + direction * distance;
    Vec3 normal{ 0,0,0 };

    int i = (int)floorf(start[0]);
    int j = (int)floorf(start[1]);
    int k = (int)floorf(start[2]);

    const int iend = (int)floorf(end[0]);
    const int jend = (int)floorf(end[1]);
    const int kend = (int)floorf(end[2]);

    const int di = ((start[0] < end[0]) ? 1 : ((start[0] > end[0]) ? -1 : 0));
    const int dj = ((start[1] < end[1]) ? 1 : ((start[1] > end[1]) ? -1 : 0));
    const int dk = ((start[2] < end[2]) ? 1 : ((start[2] > end[2]) ? -1 : 0));

    const float deltatx = 1.0f / std::abs(end[0] - start[0]);
    const float deltaty = 1.0f / std::abs(end[1] - start[1]);
    const float deltatz = 1.0f / std::abs(end[2] - start[2]);

    const float minx = floorf(start[0]), maxx = minx + 1.0f;
    float tx = ((start[0] > end[0]) ? (start[0] - minx) : (maxx - start[0])) * deltatx;
    const float miny = floorf(start[1]), maxy = miny + 1.0f;
    float ty = ((start[1] > end[1]) ? (start[1] - miny) : (maxy - start[1])) * deltaty;
    const float minz = floorf(start[2]), maxz = minz + 1.0f;
    float tz = ((start[2] > end[2]) ? (start[2] - minz) : (maxz - start[2])) * deltatz;

    while (true)
    {
        iVec3 chunkPos = GetChunkPosFromGlobalBlockPos(iVec3{ i,j,k });
        std::shared_ptr<Chunk> chunk = world->GetChunk(chunkPos);
        iVec3 blockPosInChunk = GetChunkBlockPosFromGlobalBlockPos(iVec3{ i,j,k });
        if (chunk != nullptr) {
            // If we do hit a chunk, check if its air. If it's not then we stop the raycasting otherwise we continue
            BlockID block = chunk->GetBlock(blockPosInChunk);
            if (block != AIR) {
                return BlockRaycastResult{
                    chunk,
                    blockPosInChunk,
                    normal,
                    block
                };
            }
        }
        // Raycasting
        if (tx <= ty && tx <= tz)
        {
            if (i == iend) {
                return BlockRaycastResult{
                    chunk,
                    blockPosInChunk,
                    normal,
                    AIR
                };
            };
            tx += deltatx;
            normal = iVec3{
                -di,0,0
            };
            i += di;
        }
        else if (ty <= tz)
        {
            if (j == jend) {
                return BlockRaycastResult{
                    chunk,
                    blockPosInChunk,
                    iVec3{0,0,0},
                    AIR
                };
            };
            ty += deltaty;
            normal = iVec3{
                0,-dj,0
            };
            j += dj;
        }
        else
        {
            if (k == kend) {
                return BlockRaycastResult{
                    chunk,
                    blockPosInChunk,
                    normal,
                    AIR
                };
            };
            tz += deltatz;
            normal = iVec3{
               0,0,-dk
            };
            k += dk;
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