/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include <world/Player.hpp>
#include <world/World.hpp>
#include <math/Raycast.hpp>

void Player::ProcessKeyInput(const World& world, const Window& window, float deltaTime)
{
    if (window.IsKeyPressed(GLFW_KEY_W)) {
        Move(world, PlayerMovement::FORWARD, deltaTime);
    }
    if (window.IsKeyPressed(GLFW_KEY_S)) {
        Move(world, PlayerMovement::BACKWARD, deltaTime);
    }
    if (window.IsKeyPressed(GLFW_KEY_A)) {
        Move(world, PlayerMovement::LEFT, deltaTime);
    }
    if (window.IsKeyPressed(GLFW_KEY_D)) {
        Move(world, PlayerMovement::RIGHT, deltaTime);
    }
    if (window.IsKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
        movementSpeed = 50.0f;
    }
    else {
        movementSpeed = 10.0f;
    }
}

void Player::ProcessMouseInput(const World& world, int button, int action, int mods)
{
    switch (button) {
    case GLFW_MOUSE_BUTTON_1: {
        if (action == GLFW_PRESS) {
            const Raycaster::BlockRaycastResult raycast = Raycaster::BlockRaycast(world, camera.position, camera.front, 10.0f);

            if (raycast.chunk != nullptr && raycast.blockHit != AIR) {
                raycast.chunk->SetBlock(raycast.blockPos, AIR);
                raycast.chunk->CreateMesh();
                raycast.chunk->BufferData();
            }
        }
        break;
    }
    case GLFW_MOUSE_BUTTON_RIGHT: {
        if (action == GLFW_PRESS) {
            const Raycaster::BlockRaycastResult raycast = Raycaster::BlockRaycast(world, camera.position, camera.front, 10.0f);

            if (raycast.chunk != nullptr && raycast.blockHit != AIR) {
                iVec3 blockPlacePosition = raycast.blockPos + raycast.normal;
                raycast.chunk->SetBlock(blockPlacePosition, STONE);
                raycast.chunk->CreateMesh();
                raycast.chunk->BufferData();
            }
        }
        break;
    }
    }
}

void Player::Move(const World& world, PlayerMovement direction, float deltaTime)
{
    float velocity = movementSpeed * deltaTime;
    Vec3 directionMultiplier{};

    switch (direction) {
    case PlayerMovement::FORWARD: {
        directionMultiplier = camera.front;
        break;
    }
    case PlayerMovement::BACKWARD: {
        directionMultiplier = camera.front * -1.0f;
        break;
    }
    case PlayerMovement::LEFT: {
        directionMultiplier = camera.right * -1.0f;
        break;
    }
    case PlayerMovement::RIGHT: {
        directionMultiplier = camera.right;
    }
    }

    Vec3 newPosition = camera.position;
    for (int i = 0; i < 3; i++) {
        newPosition[i] += directionMultiplier[i] * velocity;
        BlockID block = world.GetBlock(GetWorldBlockPosFromGlobalPos(newPosition));
        if (block != AIR) {
            newPosition[i] = camera.position[i];
        }
    }
    camera.position = newPosition;
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
