/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include <world/Player.hpp>
#include <world/World.hpp>
#include <math/Raycast.hpp>
#include <util/Log.hpp>

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
        movementSpeed = 25.0f;
    }
    else {
        movementSpeed = 5.0f;
    }
}

void Player::KeyCallback(int key, int scancode, int action, int mods)
{
    switch (key) {
    case GLFW_KEY_SPACE: {
        if (action == GLFW_PRESS) {
            yVelocity = -25.0f;
            break;
        }
    }
    }
}

void Player::MouseCallback(const World& world, int button, int action, int mods)
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
                raycast.chunk->SetBlock(blockPlacePosition, selectedBlock);
                raycast.chunk->CreateMesh();
                raycast.chunk->BufferData();
            }
        }
        break;
    }
    case GLFW_MOUSE_BUTTON_3: {
        if (action == GLFW_PRESS) {
            const Raycaster::BlockRaycastResult raycast = Raycaster::BlockRaycast(world, camera.position, camera.front, 10.0f);

            if (raycast.chunk != nullptr && raycast.blockHit != AIR) {
                selectedBlock = raycast.blockHit;
            }
        }
        break;
    }
    }
}

void Player::Move(const World& world, PlayerMovement direction, float deltaTime)
{
    float velocity{};
    Vec3 directionMultiplier{};

    switch (direction) {
    case PlayerMovement::FORWARD: {
        velocity = deltaTime * movementSpeed;
        directionMultiplier = Vec3{ camera.front[0], 0.0f, camera.front[2] };
        break;
    }
    case PlayerMovement::BACKWARD: {
        velocity = deltaTime * movementSpeed;
        directionMultiplier = Vec3{ camera.front[0], 0.0f, camera.front[2] } *-1.0f;
        break;
    }
    case PlayerMovement::LEFT: {
        velocity = deltaTime * movementSpeed;
        directionMultiplier = camera.right * -1.0f;
        break;
    }
    case PlayerMovement::RIGHT: {
        velocity = deltaTime * movementSpeed;
        directionMultiplier = camera.right;
        break;
    }
    case PlayerMovement::DOWN: {
        velocity = deltaTime * movementSpeed;
        directionMultiplier = camera.worldUp * -1.0f;
        break;
    }
    case PlayerMovement::UP: {
        velocity = deltaTime * movementSpeed;
        directionMultiplier = camera.worldUp;
        break;
    }
    case PlayerMovement::GRAVITY: {
        velocity = deltaTime * yVelocity;
        directionMultiplier = camera.worldUp * -1.0f;
        break;
    }
    }

    Vec3 newPosition = camera.position;
    for (int i = 0; i < 3; i++) {
        newPosition[i] += directionMultiplier[i] * velocity;
        if (boundingBox.IsColliding(world, newPosition - Vec3{ 0.0f, 1.0f, 0.0f })) {
            newPosition[i] = camera.position[i];
            if (i == 1) {
                yVelocity = 0.0f;
            }
        }
    }
    camera.position = newPosition;
}

void Player::ApplyGravity(const World& world, float deltaTime)
{
    Move(world, PlayerMovement::GRAVITY, deltaTime);
    yVelocity += GRAVITY;
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
