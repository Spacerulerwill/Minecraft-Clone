/*
Copyright (C) 2023 William Redding - All Rights Reserved
LICENSE: MIT
*/

#include <util/Constants.hpp>
#include <util/Log.hpp>
#include <world/Player.hpp>
#include <world/World.hpp>

engine::Player::Player() {

}

engine::Player::Player(Vec3<float> pos, float pitch, float yaw) : m_Camera(pos, pitch, yaw) {

}

engine::Camera& engine::Player::GetCamera() {
    return m_Camera;
}

engine::Vec3<int> engine::Player::GetChunkPosition() const {
    Vec3<int> cameraPos = m_Camera.GetPosition();
    Vec3<int> chunkPos = cameraPos / CS;
    if (cameraPos.x < 0)
        chunkPos.x--;
    if (cameraPos.z < 0)
        chunkPos.z--;
    return chunkPos;
}

void engine::Player::BlockRaycast(World* world) {
    m_VoxelRaycastResult = VoxelRaycast(world, m_Camera.GetPosition(), m_Camera.GetDirection(), 15);
}

const engine::VoxelRaycastResult& engine::Player::GetBlockRaycastResult() const {
    return m_VoxelRaycastResult;
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