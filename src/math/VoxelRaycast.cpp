/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include <math/VoxelRaycast.hpp>
#include <math.h>
#include <util/Log.hpp>

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

engine::VoxelRaycastResult engine::VoxelRaycast(const World& world, const Vec3& start, const Vec3& direction, float distance) {
    // Cube containing origin point.
    int x = floor(start.x);
    int y = floor(start.y);
    int z = floor(start.z);

    // Direction to increment x,y,z when stepping.
    int stepX = signum(direction.x);
    int stepY = signum(direction.y);
    int stepZ = signum(direction.z);

    // See description above. The initial values depend on the fractional
    // part of the origin.
    float tMaxX = intbound(start.x, direction.x);
    float tMaxY = intbound(start.y, direction.y);
    float tMaxZ = intbound(start.z, direction.z);

    // The change in t when taking a step (always positive).
    float tDeltaX = stepX / direction.x;
    float tDeltaY = stepY / direction.y;
    float tDeltaZ = stepZ / direction.z;

    if (direction.x == 0 && direction.y == 0 && direction.z == 0) {
        throw std::runtime_error("Cannot perform voxel raycast with 0 direction!");
    }

    distance /= sqrt(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
    int face[3] = {0,0,0};

    while (true)
    {
        if (tMaxX < tMaxY)
        {
            if (tMaxX < tMaxZ)
            {
                if (tMaxX > distance) {
                    int chunkX = x / CS;
                    int chunkY = y / CS;
                    int chunkZ = z / CS;

                    if (start.x < 0) chunkX -= 1;
                    if (start.z < 0) chunkZ -= 1;

                    int blockX = 1 + abs(x) % CS;
                    int blockY = 1 + abs(y) % CS;
                    int blockZ = 1 + abs(z) % CS;

                    if (start.x < 0) blockX = CS_P - blockX;
                    if (start.z < 0) blockZ = CS_P - blockZ;
                    Chunk* chunk = world.GetChunk(chunkX, chunkY, chunkZ);
                    return VoxelRaycastResult {
                        chunk,
                        blockX,
                        blockY,
                        blockZ,
                        0,
                        0,
                        0,
                        AIR
                    };
                }
                // Update which cube we are now in.
                x += stepX;
                // Adjust tMaxX to the next X-oriented boundary crossing.
                tMaxX += tDeltaX;
                // Record the normal vector of the cube face we entered.
                face[0] = -stepX;
                face[1] = 0;
                face[2] = 0;
            }
            else
            {
                if (tMaxZ > distance) {
                    int chunkX = x / CS;
                    int chunkY = y / CS;
                    int chunkZ = z / CS;

                    if (start.x < 0) chunkX -= 1;
                    if (start.z < 0) chunkZ -= 1;

                    int blockX = 1 + abs(x) % CS;
                    int blockY = 1 + abs(y) % CS;
                    int blockZ = 1 + abs(z) % CS;

                    if (start.x < 0) blockX = CS_P - blockX;
                    if (start.z < 0) blockZ = CS_P - blockZ;
                    Chunk* chunk = world.GetChunk(chunkX, chunkY, chunkZ);
                    return VoxelRaycastResult {
                        chunk,
                        blockX,
                        blockY,
                        blockZ,
                        0,
                        0,
                        0,
                        AIR
                    };
                }
                z += stepZ;
                tMaxZ += tDeltaZ;
                face[0] = 0;
                face[1] = 0;
                face[2] = -stepZ;
            }
        }
        else
        {
            if (tMaxY < tMaxZ)
            {
                if (tMaxY > distance) {
                    
                }
                y += stepY;
                tMaxY += tDeltaY;
                face[0] = 0;
                face[1] = -stepY;
                face[2] = 0;
            }
            else
            {
                if (tMaxZ > distance) {
                    int chunkX = x / CS;
                    int chunkY = y / CS;
                    int chunkZ = z / CS;

                    if (start.x < 0) chunkX -= 1;
                    if (start.z < 0) chunkZ -= 1;

                    int blockX = 1 + abs(x) % CS;
                    int blockY = 1 + abs(y) % CS;
                    int blockZ = 1 + abs(z) % CS;

                    if (start.x < 0) blockX = CS_P - blockX;
                    if (start.z < 0) blockZ = CS_P - blockZ;
                    Chunk* chunk = world.GetChunk(chunkX, chunkY, chunkZ);
                    return VoxelRaycastResult {
                        chunk,
                        blockX,
                        blockY,
                        blockZ,
                        0,
                        0,
                        0,
                        AIR
                    };
                }
                z += stepZ;
                tMaxZ += tDeltaZ;
                face[0] = 0;
                face[1] = 0;
                face[2] = -stepZ;
            }
        }

        int chunkX = x / CS;
        int chunkY = y / CS;
        int chunkZ = z / CS;

        if (start.x < 0) chunkX -= 1;
        if (start.z < 0) chunkZ -= 1;

        int blockX = 1 + abs(x) % CS;
        int blockY = 1 + abs(y) % CS;
        int blockZ = 1 + abs(z) % CS;

        if (start.x < 0) blockX = CS_P - blockX;
        if (start.z < 0) blockZ = CS_P - blockZ;

        Chunk* chunk = world.GetChunk(chunkX, chunkY, chunkZ);
        if (chunk == nullptr) {
            return VoxelRaycastResult {
                chunk,
                blockX,
                blockY,
                blockZ,
                0,
                0,
                0,
                AIR
            };
        }

        BlockInt block = chunk->GetBlock(blockX,blockY,blockZ);

        if (block != AIR) {
            return VoxelRaycastResult {
                    chunk,
                    blockX,
                    blockY,
                    blockZ,
                    face[0],
                    face[1],
                    face[2],
                    block
            };
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