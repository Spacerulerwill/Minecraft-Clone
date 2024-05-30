/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/
#include <math/AABB.hpp>
#include <world/World.hpp>
#include <world/Block.hpp>

bool BoundingBox::IsColliding(const World& world, glm::vec3 center)
{
    glm::ivec3 corner1 = GetWorldBlockPosFromGlobalPos(center + localPos - size);
    glm::ivec3 corner2 = GetWorldBlockPosFromGlobalPos(center + localPos + size);

    for (int x = corner1.x; x <= corner2.x; x++) {
        for (int y = corner1.y; y <= corner2.y; y++) {
            for (int z = corner1.z; z <= corner2.z; z++) {
                Block block = world.GetBlock(glm::ivec3(x,y,z));
                BlockType type = block.GetType();
                if (type != BlockType::AIR && GetBlockData(type).collision) {
                    return true;
                }
            }
        }
    }
    return false;
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