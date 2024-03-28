/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include <world/Player.hpp>
#include <world/World.hpp>
#include <math/Raycast.hpp>
#include <core/SoundEngine.hpp>
#include <util/Log.hpp>
#include <util/Util.hpp>

void Player::ProcessKeyInput(const World& world, const Window& window, float deltaTime)
{
    float speed = window.IsKeyPressed(GLFW_KEY_LEFT_SHIFT) ? movementSpeed * 2 : movementSpeed;
    if (window.IsKeyPressed(GLFW_KEY_W)) {
        Move(world, PlayerMovement::FORWARD, speed, deltaTime);
    }
    if (window.IsKeyPressed(GLFW_KEY_S)) {
        Move(world, PlayerMovement::BACKWARD, speed, deltaTime);
    }
    if (window.IsKeyPressed(GLFW_KEY_A)) {
        Move(world, PlayerMovement::LEFT, speed, deltaTime);
    }
    if (window.IsKeyPressed(GLFW_KEY_D)) {
        Move(world, PlayerMovement::RIGHT, speed, deltaTime);
    }

    if (window.IsKeyPressed(GLFW_KEY_SPACE)) {
        if (canJump) {
            yVelocity = -15.0f;
            canJump = false;
        }
    }
}

void Player::KeyCallback(int key, int scancode, int action, int mods)
{
    UNUSED(scancode, mods, action);

    switch (key) {
    case GLFW_KEY_X: {
        auto selectedBlockID = static_cast<BlockTypeID>(selectedBlockType);
        selectedBlockID--;
        if (static_cast<BlockType>(selectedBlockID) == BlockType::AIR) {
            selectedBlockType = static_cast<BlockType>(static_cast<BlockTypeID>(BlockType::NUM_BLOCKS) - 1);
        }
        else {
            selectedBlockType = static_cast<BlockType>(selectedBlockID);
        }
        break;
    }
    case GLFW_KEY_C: {
        auto selectedBlockID = static_cast<BlockTypeID>(selectedBlockType);
        selectedBlockID++;
        if (static_cast<BlockType>(selectedBlockID) == BlockType::NUM_BLOCKS) {
            selectedBlockType = static_cast<BlockType>(static_cast<BlockTypeID>(BlockType::AIR) + 1);
        }
        else {
            selectedBlockType = static_cast<BlockType>(selectedBlockID);
        }
        break;
    }
    }
}

void Player::MouseCallback(const World& world, int button, int action, int mods)
{
    UNUSED(mods);

    switch (button) {
    case GLFW_MOUSE_BUTTON_1: {
        if (action == GLFW_PRESS) {
            const Raycaster::BlockRaycastResult raycast = Raycaster::BlockRaycast(world, camera.position, camera.front, 10.0f);
            BlockType type = raycast.blockHit.GetType();
            const BlockDataStruct& blockData = GetBlockData(type);
            if (raycast.chunk != nullptr && !blockData.canInteractThrough) {
                // Update chunk block was broken in
                if (!blockData.canInteractThrough) {
                    if (raycast.blockHit.IsWaterLogged()) {
                        raycast.chunk->SetBlock(raycast.blockPos, Block(BlockType::WATER, 0, false));
                    }
                    else {
                        raycast.chunk->SetBlock(raycast.blockPos, Block(BlockType::AIR, 0, false));
                    }
                    raycast.chunk->CreateMesh();
                    raycast.chunk->BufferData();
                    BlockSoundStruct soundData = BlockSounds[blockData.breakSoundID];
                    SoundEngine::GetEngine()->play3D(soundData.sounds[rand() % soundData.sounds.size()].c_str(), irrklang::vec3df(camera.position));
                }
            }
        }
        break;
    }
    case GLFW_MOUSE_BUTTON_RIGHT: {
        if (action == GLFW_PRESS) {
            // Perform ray cast and get info about the block hit
            const Raycaster::BlockRaycastResult raycast = Raycaster::BlockRaycast(world, camera.position, camera.front, 10.0f);
            BlockType type = raycast.blockHit.GetType();
            const BlockDataStruct& hitBlockData = GetBlockData(type);

            // Info about our selected block
            const BlockDataStruct& selectedBlockData = GetBlockData(selectedBlockType);

            if (raycast.chunk != nullptr) {
                // if we are placing water in a waterloggable block
                if (selectedBlockType == BlockType::WATER && hitBlockData.waterloggable && !raycast.blockHit.IsWaterLogged()) {
                    raycast.chunk->SetBlock(raycast.blockPos, Block(raycast.blockHit.GetType(), 0, true));
                }
                else if (!hitBlockData.canInteractThrough) {
                    iVec3 blockPlacePosition = raycast.blockPos + raycast.normal;
                    Block blockBeforePlace = raycast.chunk->GetBlock(blockPlacePosition);

                    // If we are placing a waterloggable block in a water block
                    if (selectedBlockData.waterloggable && blockBeforePlace.GetType() == BlockType::WATER) {
                        raycast.chunk->SetBlock(blockPlacePosition, Block(selectedBlockType, 0, true));
                    }
                    else {
                        raycast.chunk->SetBlock(blockPlacePosition, Block(selectedBlockType, 0, false));
                    }

                    if (boundingBox.IsColliding(world, camera.position)) {
                        raycast.chunk->SetBlock(blockPlacePosition, blockBeforePlace);
                        return;
                    }
                }
            }
            raycast.chunk->CreateMesh();
            raycast.chunk->BufferData();
            const BlockDataStruct& blockData = GetBlockData(selectedBlockType);
            BlockSoundStruct soundData = BlockSounds[blockData.placeSoundID];
            SoundEngine::GetEngine()->play3D(soundData.sounds[rand() % soundData.sounds.size()].c_str(), irrklang::vec3df(camera.position));
        }
        break;
    }
    case GLFW_MOUSE_BUTTON_3: {
        if (action == GLFW_PRESS) {
            const Raycaster::BlockRaycastResult raycast = Raycaster::BlockRaycast(world, camera.position, camera.front, 10.0f);
            BlockType type = raycast.blockHit.GetType();
            if (raycast.chunk != nullptr && !GetBlockData(type).canInteractThrough) {
                selectedBlockType = type;
            }
        }
        break;
    }
    }
}

void Player::Move(const World& world, PlayerMovement direction, float speed, float deltaTime)
{
    float velocity{};
    Vec3 directionMultiplier{};

    switch (direction) {
    case PlayerMovement::FORWARD: {
        velocity = deltaTime * speed;
        directionMultiplier = Vec3{ camera.front[0], 0.0f, camera.front[2] }.normalized();
        break;
    }
    case PlayerMovement::BACKWARD: {
        velocity = deltaTime * speed;
        directionMultiplier = Vec3{ camera.front[0], 0.0f, camera.front[2] }.normalized() * -1.0f;
        break;
    }
    case PlayerMovement::LEFT: {
        velocity = deltaTime * speed;
        directionMultiplier = camera.right * -1.0f;
        break;
    }
    case PlayerMovement::RIGHT: {
        velocity = deltaTime * speed;
        directionMultiplier = camera.right;
        break;
    }
    case PlayerMovement::DOWN: {
        velocity = deltaTime * speed;
        directionMultiplier = camera.worldUp * -1.0f;
        break;
    }
    case PlayerMovement::UP: {
        velocity = deltaTime * speed;
        directionMultiplier = camera.worldUp;
        break;
    }
    case PlayerMovement::GRAVITY: {
        velocity = deltaTime * yVelocity;
        directionMultiplier = camera.worldUp * -1.0f;
        break;
    }
    }

    Vec3 distanceToMove = directionMultiplier * velocity;
    Vec3 newPosition = camera.position;
    for (int axis = 0; axis < 3; axis++) {
        newPosition[axis] += distanceToMove[axis];
        if (boundingBox.IsColliding(world, newPosition)) {
            newPosition[axis] = camera.position[axis];
            if (axis == 1) {
                yVelocity = 0.0f;
                canJump = true;
            }
        }
    }
    camera.position = newPosition;
}

void Player::ApplyGravity(const World& world, float deltaTime)
{
    Move(world, PlayerMovement::GRAVITY, 1.0f, deltaTime);
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
