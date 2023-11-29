/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include <assert.h>
#include <math.h>
#include <math/VoxelRaycast.hpp>
#include <util/Log.hpp>
#include <world/World.hpp>

namespace engine {
    VoxelRaycastResult GenerateVoxelRaycastResult(World* world, Vec3<int> normal, Vec3<int> globalBlockPos) {
        // Chunk region 
        Vec2<int> regionPos;

        if (globalBlockPos.x < 0) {
            regionPos.x = ((globalBlockPos.x + 1) / CHUNK_REGION_BLOCK_SIZE) - 1;
        }
        else {
            regionPos.x = globalBlockPos.x / CHUNK_REGION_BLOCK_SIZE;
        }

        if (globalBlockPos.z < 0) {
            regionPos.y = ((globalBlockPos.z + 1) / CHUNK_REGION_BLOCK_SIZE) - 1;
        }
        else {
            regionPos.y = globalBlockPos.z / CHUNK_REGION_BLOCK_SIZE;
        }

        ChunkRegion* region = world->GetRegion(regionPos);

        if (region == nullptr) {
            return VoxelRaycastResult{};
        }

        // Chunk position
        Vec3<int> chunkPos(
            (globalBlockPos.x / CS) - regionPos.x * CHUNK_REGION_SIZE,
            globalBlockPos.y / CS,
            (globalBlockPos.z / CS) - regionPos.y * CHUNK_REGION_SIZE
        );

        if (globalBlockPos.x < 0) {
            chunkPos.x--;
        }

        if (globalBlockPos.z < 0) {
            chunkPos.z--;
        }

        // Block position in chunk
        Vec3<int> blockHitPos;

        blockHitPos.y = 1 + abs(globalBlockPos.y) % CS;

        if (globalBlockPos.x >= 0) {
            blockHitPos.x = 1 + abs(globalBlockPos.x) % CS;
        }
        else {
            blockHitPos.x = CS_P_MINUS_ONE - (1 + (abs(globalBlockPos.x) - 1) % CS);
        }

        if (globalBlockPos.z >= 0) {
            blockHitPos.z = 1 + abs(globalBlockPos.z) % CS;
        }
        else {
            blockHitPos.z = CS_P_MINUS_ONE - (1 + (abs(globalBlockPos.z) - 1) % CS);
        }

        Chunk* chunk = region->GetChunk(chunkPos);

        if (chunk == nullptr) {
            return VoxelRaycastResult{
                chunk,
                blockHitPos,
                normal,
                AIR
            };
        }
        else {
            BlockInt blockHit = chunk->GetBlock(blockHitPos);
            return VoxelRaycastResult{
                chunk,
                blockHitPos,
                normal,
                blockHit
            };
        }
    }

    float fract0(float x) {
        return x - floorf(x);
    }

    float fract1(float x) {
        return 1 - x + floorf(x);
    }

    VoxelRaycastResult VoxelRaycast(World* world, const Vec3<float>& start, const Vec3<float>& direction, int distance) {
        assert(!(direction.x == 0 && direction.y == 0 && direction.z == 0));

        float tMaxX, tMaxY, tMaxZ, tDeltaX, tDeltaY, tDeltaZ;

        Vec3<float> end = start + direction * distance;

        Vec3<int> step = Vec3<int>(
            direction.x > 0 ? 1 : (direction.x < 0 ? -1 : 0),
            direction.y > 0 ? 1 : (direction.y < 0 ? -1 : 0),
            direction.z > 0 ? 1 : (direction.z < 0 ? -1 : 0)
        );

        Vec3<int> voxel = Vec3<int>(
            std::floor(start.x),
            std::floor(start.y),
            std::floor(start.z)
        );

        Vec3<int> normal(0);

        if (step.x != 0) tDeltaX = fmin(step.x / (end.x - start.x), std::numeric_limits<float>::max()); else tDeltaX = std::numeric_limits<float>::max();
        if (step.x > 0) tMaxX = tDeltaX * fract1(start.x); else tMaxX = tDeltaX * fract0(start.x);

        if (step.y != 0) tDeltaY = fmin(step.y / (end.y - start.y), std::numeric_limits<float>::max()); else tDeltaY = std::numeric_limits<float>::max();
        if (step.y > 0) tMaxY = tDeltaY * fract1(start.y); else tMaxY = tDeltaY * fract0(start.y);

        if (step.z != 0) tDeltaZ = fmin(step.z / (end.z - start.z), std::numeric_limits<float>::max()); else tDeltaZ = std::numeric_limits<float>::max();
        if (step.z > 0) tMaxZ = tDeltaZ * fract1(start.z); else tMaxZ = tDeltaZ * fract0(start.z);

        // process first voxel
        {
            VoxelRaycastResult res = GenerateVoxelRaycastResult(world, normal, voxel);
            if (res.blockHit != AIR) {
                return res;
            }
        }

        while (true) {
            if (tMaxX < tMaxY) {
                if (tMaxX < tMaxZ) {
                    voxel.x += step.x;
                    tMaxX += tDeltaX;
                    normal.x = -step.x;
                    normal.y = 0;
                    normal.z = 0;
                }
                else {
                    voxel.z += step.z;
                    tMaxZ += tDeltaZ;
                    normal.x = 0;
                    normal.y = 0;
                    normal.z = -step.z;
                }
            }
            else {
                if (tMaxY < tMaxZ) {
                    voxel.y += step.y;
                    tMaxY += tDeltaY;
                    normal.x = 0;
                    normal.y = -step.y;
                    normal.z = 0;
                }
                else {
                    voxel.z += step.z;
                    tMaxZ += tDeltaZ;
                    normal.x = 0;
                    normal.y = 0;
                    normal.z = -step.z;
                }
            }
            if (tMaxX > 1 && tMaxY > 1 && tMaxZ > 1) {
                // process last voxel
                VoxelRaycastResult res = GenerateVoxelRaycastResult(world, normal, voxel);
                return res;
            }
            else {
                // process intermediate voxels
                VoxelRaycastResult res = GenerateVoxelRaycastResult(world, normal, voxel);
                if (res.blockHit != AIR) {
                    return res;
                }
            }
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