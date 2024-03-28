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
                raycast.chunk->SetBlock(raycast.blockPos, Block(BlockType::AIR, 0, false));
                raycast.chunk->CreateMesh();
                raycast.chunk->BufferData();
                BlockSoundStruct soundData = BlockSounds[blockData.breakSoundID];
                SoundEngine::GetEngine()->play3D(soundData.sounds[rand() % soundData.sounds.size()].c_str(), irrklang::vec3df(camera.position));

                // If the block was on an chunk edge and it was a cube, re-mesh that one too
                if (blockData.modelID == static_cast<ModelID>(Model::CUBE)) {
                    for (int i = 0; i < 3; i++) {
                        Vec3 chunkPos = raycast.chunk->GetPosition();

                        if (raycast.blockPos[i] == 1) {
                            chunkPos[i]--;
                            auto chunk = world.GetChunk(chunkPos);
                            if (chunk != nullptr) {
                                Vec3 blockPos = raycast.blockPos;
                                blockPos[i] = Chunk::SIZE_PADDED_SUB_1;
                                chunk->SetBlock(blockPos, Block(BlockType::AIR, 0, false));
                                chunk->CreateMesh();
                                chunk->BufferData();
                            }
                        }
                        else if (raycast.blockPos[i] == Chunk::SIZE) {
                            chunkPos[i]++;
                            auto chunk = world.GetChunk(chunkPos);
                            if (chunk != nullptr) {
                                Vec3 blockPos = raycast.blockPos;
                                blockPos[i] = 0;
                                chunk->SetBlock(blockPos, Block(BlockType::AIR, 0, false));
                                chunk->CreateMesh();
                                chunk->BufferData();
                            }
                        }
                    }
                }
            }
        }
        break;
    }
    case GLFW_MOUSE_BUTTON_RIGHT: {
        if (action == GLFW_PRESS) {
            // Perform raycast and get info about the block hit
            const Raycaster::BlockRaycastResult raycast = Raycaster::BlockRaycast(world, camera.position, camera.front, 10.0f);
            BlockType type = raycast.blockHit.GetType();
            const BlockDataStruct& hitBlockData = GetBlockData(type);

            // Info about our selected block
            const BlockDataStruct& selectedBlockData = GetBlockData(selectedBlockType);

            if (raycast.chunk != nullptr && !hitBlockData.canInteractThrough) {
                std::shared_ptr<Chunk> chunkToPlaceIn = nullptr;
                iVec3 blockPlacePosition = raycast.blockPos + raycast.normal;

                for (int i = 0; i < 3; i++) {
                    if (blockPlacePosition[i] == Chunk::SIZE_PADDED_SUB_1) {
                        Vec3 chunkPos = raycast.chunk->GetPosition();
                        chunkPos[i]++;
                        chunkToPlaceIn = world.GetChunk(chunkPos);
                        if (chunkToPlaceIn != nullptr) {
                            raycast.chunk->SetBlock(blockPlacePosition, Block(selectedBlockType, 0, false));
                            // If they are both opaque or both not opaque
                            if (selectedBlockData.opaque == hitBlockData.opaque) {
                                raycast.chunk->CreateMesh();
                                raycast.chunk->BufferData();
                            }
                            blockPlacePosition[i] = 1;
                            goto place_block;
                        }
                    }
                    else if (blockPlacePosition[i] == 0) {
                        Vec3 chunkPos = raycast.chunk->GetPosition();
                        chunkPos[i]--;
                        chunkToPlaceIn = world.GetChunk(chunkPos);
                        if (chunkToPlaceIn != nullptr) {
                            raycast.chunk->SetBlock(blockPlacePosition, Block(selectedBlockType, 0, false));
                            // If they are both opaque or both not opaque
                            if (selectedBlockData.opaque == hitBlockData.opaque) {
                                raycast.chunk->CreateMesh();
                                raycast.chunk->BufferData();
                            }
                            blockPlacePosition[i] = Chunk::SIZE;
                            goto place_block;
                        }
                    }
                }

                // otherwise our chunk is just the raycast chunk hit
                chunkToPlaceIn = raycast.chunk;

            place_block:
                if (chunkToPlaceIn != nullptr) {
                    Block blockBeforePlace = chunkToPlaceIn->GetBlock(blockPlacePosition);
                    chunkToPlaceIn->SetBlock(blockPlacePosition, Block(selectedBlockType, 0, false));

                    if (boundingBox.IsColliding(world, camera.position)) {
                        chunkToPlaceIn->SetBlock(blockPlacePosition, blockBeforePlace);
                        return;
                    }

                    for (int i = 0; i < 3; i++) {
                        if (blockPlacePosition[i] == Chunk::SIZE) {
                            Vec3 chunkPos = chunkToPlaceIn->GetPosition();
                            chunkPos[i]++;
                            auto adjacentChunk = world.GetChunk(chunkPos);
                            if (adjacentChunk != nullptr) {
                                Vec3 blockPos = blockPlacePosition;
                                blockPos[i] = 0;
                                adjacentChunk->SetBlock(blockPos, Block(selectedBlockType, 0, false));
                                blockPos[i] = 1;
                                if (selectedBlockData.opaque == GetBlockData(adjacentChunk->GetBlock(blockPos).GetType()).opaque) {
                                    adjacentChunk->CreateMesh();
                                    adjacentChunk->BufferData();
                                }
                            }
                        }
                        else if (blockPlacePosition[i] == 1) {
                            Vec3 chunkPos = chunkToPlaceIn->GetPosition();
                            chunkPos[i]--;
                            auto adjacentChunk = world.GetChunk(chunkPos);
                            if (adjacentChunk != nullptr) {
                                Vec3 blockPos = blockPlacePosition;
                                blockPos[i] = Chunk::SIZE_PADDED_SUB_1;
                                adjacentChunk->SetBlock(blockPos, Block(selectedBlockType, 0, false));
                                blockPos[i] = Chunk::SIZE;
                                if (selectedBlockData.opaque == GetBlockData(adjacentChunk->GetBlock(blockPos).GetType()).opaque) {
                                    adjacentChunk->CreateMesh();
                                    adjacentChunk->BufferData();
                                }
                            }
                        }
                    }

                    chunkToPlaceIn->CreateMesh();
                    chunkToPlaceIn->BufferData();
                    const BlockDataStruct& blockData = GetBlockData(selectedBlockType);
                    BlockSoundStruct soundData = BlockSounds[blockData.placeSoundID];
                    SoundEngine::GetEngine()->play3D(soundData.sounds[rand() % soundData.sounds.size()].c_str(), irrklang::vec3df(camera.position));
                }
            }
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
