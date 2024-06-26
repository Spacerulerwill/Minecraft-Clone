/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#ifndef RAYCAST_H
#define RAYCAST_H

#include <world/Block.hpp>
#include <glm/vec3.hpp>
#include <world/chunk/Chunk.hpp>
#include <memory>

class World;

namespace Raycaster {

    struct BlockRaycastResult {
        std::shared_ptr<Chunk> chunk;
        glm::ivec3 blockPos{};
        glm::ivec3 normal{};
        Block blockHit = Block(BlockType::AIR, 0, false);
    };

    BlockRaycastResult BlockRaycast(const World& world, glm::vec3 start, glm::vec3 direction, float distance);
};

#endif // !RAYCAST_H

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