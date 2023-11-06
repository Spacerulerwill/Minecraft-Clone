/*
Copyright (C) 2023 William Redding - All Rights Reserved
LICENSE: MIT
*/

#ifndef PLAYER_H
#define PLAYER_H

#include <core/Camera.hpp>
#include <math/Vec3.hpp>
#include <math/VoxelRaycast.hpp>

namespace engine {
    class World;

    class Player {
    private:
        Camera m_Camera;
        VoxelRaycastResult m_VoxelRaycastResult {};
    public:
        Player();
        Player(Vec3<float> position, float pitch, float yaw);
        void BlockRaycast(World* world);
        const VoxelRaycastResult& GetBlockRaycastResult() const;
        Camera& GetCamera();
        Vec3<int> GetChunkPosition() const;
    };
}

#endif // !PLAYER_H
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