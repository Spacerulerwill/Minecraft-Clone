/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include <util/Log.hpp>
#include <math/VoxelRaycast.hpp>
#include <math.h>

int signum(float x) {
  return x > 0 ? 1 : x < 0 ? -1 : 0;
}

float ceil2(float s) { if (s == 0.0f) return 1.0f; else return ceil(s); }

float intbound(float s, float ds)
{
    bool sIsInteger = round(s) == s;
    if (ds < 0 && sIsInteger)
        return 0;

    return (ds > 0 ? ceil2(s) - s : s - floor(s)) / abs(ds);
}

engine::VoxelRaycastResult engine::VoxelRaycast(World* world, const Vec3<float>& start, const Vec3<float>& direction, float distance) {
    // Cube containing origin point.
    Vec3<int> blockPos(floor(start.x), floor(start.y), floor(start.z));

    if (start.x < 0) {
        blockPos.x += 1;
    }

    if (start.z < 0) {
        blockPos.z += 1;
    }

    // Direction to increment x,y,z when stepping.
    Vec3<int> steps(signum(direction.x), signum(direction.y), signum(direction.z));

    // See description above. The initial values depend on the fractional
    // part of the origin.
    Vec3<float> tMax(intbound(start.x, direction.x), intbound(start.y, direction.y), intbound(start.z, direction.z));

    // The change in t when taking a step (always positive).
    Vec3<float> tDelta(steps.x / direction.x, steps.y / direction.y, steps.z / direction.z);

    if (direction.x == 0 && direction.y == 0 && direction.z == 0) {
        throw std::runtime_error("Cannot perform voxel raycast with 0 direction!");
    }

    distance /= sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
    int face[3] = {0,0,0};

    while (true)
    {
        if (tMax.x < tMax.y)
        {
            if (tMax.x < tMax.z)
            {
                if (tMax.x > distance) {
                    Vec3<int> chunkPos(blockPos.x / CS, blockPos.y / CS, blockPos.z / CS);
                    Vec3<int> blockHitPos = Vec3<int>(1) + (blockPos.abs() % CS);

                    if (blockPos.x < 0) { 
                        blockHitPos.x = CS_P - blockHitPos.x - 1;
                        chunkPos.x -= 1;
                    }
                    
                    if (blockPos.z < 0)  {
                        blockHitPos.z = CS_P - blockHitPos.z - 1;
                        chunkPos.z -= 1;
                    }

                    Chunk* chunk = world->GetChunk(chunkPos.x, chunkPos.y, chunkPos.z);
                    return VoxelRaycastResult {
                        chunk,
                        blockHitPos,
                        Vec3<int>(0),
                        AIR
                    };
                }
                // Update which cube we are now in.
                blockPos.x += steps.x;
                // Adjust tMaxX to the next X-oriented boundary crossing.
                tMax.x += tDelta.x;
                // Record the normal vector of the cube face we entered.
                face[0] = -steps.x;
                face[1] = 0;
                face[2] = 0;
            }
            else
            {
                if (tMax.z > distance) {
                    Vec3<int> chunkPos(blockPos.x / CS, blockPos.y / CS, blockPos.z / CS);
                    Vec3<int> blockHitPos = Vec3<int>(1) + (blockPos.abs() % CS);

                    if (blockPos.x < 0) { 
                        blockHitPos.x = CS_P - blockHitPos.x - 1;
                        chunkPos.x -= 1;
                    }
                    
                    if (blockPos.z < 0)  {
                        blockHitPos.z = CS_P - blockHitPos.z - 1;
                        chunkPos.z -= 1;
                    }
                    Chunk* chunk = world->GetChunk(chunkPos.x, chunkPos.y, chunkPos.z);
                    return VoxelRaycastResult {
                        chunk,
                        blockHitPos,
                        Vec3<int>(0),
                        AIR
                    };
                }
                blockPos.z += steps.z;
                tMax.z += tDelta.z;
                face[0] = 0;
                face[1] = 0;
                face[2] = -steps.z;
            }
        }
        else
        {
            if (tMax.y < tMax.z)
            {
                if (tMax.y > distance) {
                    Vec3<int> chunkPos(blockPos.x / CS, blockPos.y / CS, blockPos.z / CS);
                    Vec3<int> blockHitPos = Vec3<int>(1) + (blockPos.abs() % CS);

                    if (blockPos.x < 0) { 
                        blockHitPos.x = CS_P - blockHitPos.x - 1;
                        chunkPos.x -= 1;
                    }
                    
                    if (blockPos.z < 0)  {
                        blockHitPos.z = CS_P - blockHitPos.z - 1;
                        chunkPos.z -= 1;
                    }
                }
                blockPos.y += steps.y;
                tMax.y += tDelta.y;
                face[0] = 0;
                face[1] = -steps.y;
                face[2] = 0;
            }
            else
            {
                if (tMax.z > distance) {
                    Vec3<int> chunkPos(blockPos.x / CS, blockPos.y / CS, blockPos.z / CS);
                    Vec3<int> blockHitPos = Vec3<int>(1) + (blockPos.abs() % CS);

                    if (blockPos.x < 0) { 
                        blockHitPos.x = CS_P - blockHitPos.x - 1;
                        chunkPos.x -= 1;
                    }
                    
                    if (blockPos.z < 0)  {
                        blockHitPos.z = CS_P - blockHitPos.z - 1;
                        chunkPos.z -= 1;
                    }

                    Chunk* chunk = world->GetChunk(chunkPos.x, chunkPos.y, chunkPos.z);
                    return VoxelRaycastResult {
                        chunk,
                        blockHitPos,
                        Vec3<int>(0),
                        AIR
                    };
                }
                blockPos.z += steps.z;
                tMax.z += tDelta.z;
                face[0] = 0;
                face[1] = 0;
                face[2] = -steps.z;
            }
        }

        Vec3<int> chunkPos(blockPos.x / CS, blockPos.y / CS, blockPos.z / CS);
        Vec3<int> blockHitPos = Vec3<int>(1) + (blockPos.abs() % CS);

        if (blockPos.x < 0) { 
            blockHitPos.x = CS_P - blockHitPos.x - 1;
            chunkPos.x -= 1;
        }
        
        if (blockPos.z < 0)  {
            blockHitPos.z = CS_P - blockHitPos.z - 1;
            chunkPos.z -= 1;
        }

        Chunk* chunk = world->GetChunk(chunkPos.x, chunkPos.y, chunkPos.z);
        if (chunk == nullptr) {
            return VoxelRaycastResult {
                chunk,
                blockHitPos,
                Vec3<int>(0),
                AIR
            };
        } else {
            BlockInt block = chunk->GetBlock(blockHitPos);

            if (block != AIR) {
                return VoxelRaycastResult {
                        chunk,
                        blockHitPos,
                        Vec3<int>(face[0], face[1], face[2]),
                        block
                };
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