/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include <world/Player.hpp>
#include <world/World.hpp>
#include <math/Raycast.hpp>
#include <core/SoundEngine.hpp>
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
        movementSpeed = 10.0f;
    }
    else {
        movementSpeed = 5.0f;
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

}

void Player::MouseCallback(const World& world, int button, int action, int mods)
{
    switch (button) {
    case GLFW_MOUSE_BUTTON_1: {
        if (action == GLFW_PRESS) {
            const Raycaster::BlockRaycastResult raycast = Raycaster::BlockRaycast(world, camera.position, camera.front, 10.0f);

            if (raycast.chunk != nullptr && raycast.blockHit != AIR) {
                // Update chunk block was broken in
                raycast.chunk->SetBlock(raycast.blockPos, AIR);
                raycast.chunk->CreateMesh();
                raycast.chunk->BufferData();
                BlockDataStruct blockData = BlockData[raycast.blockHit];
                BlockSoundStruct soundData = BlockSounds[blockData.breakSoundID];
                SoundEngine::GetEngine()->play3D(soundData.sounds[rand() % soundData.sounds.size()].c_str(), irrklang::vec3df(camera.position));

                // If the block was on an chunk edge and it was a cube, remesh that one too
                if (BlockData[raycast.blockHit].modelID == static_cast<ModelID>(Model::CUBE)) {
                    for (int i = 0; i < 3; i++) {
                        Vec3 chunkPos = raycast.chunk->GetPosition();

                        if (raycast.blockPos[i] == 1) {
                            chunkPos[i]--;
                            auto chunk = world.GetChunk(chunkPos);
                            if (chunk != nullptr) {
                                Vec3 blockPos = raycast.blockPos;
                                blockPos[i] = CS_P_MINUS_ONE;
                                chunk->SetBlock(blockPos, AIR);
                                chunk->CreateMesh();
                                chunk->BufferData();
                            }
                        }
                        else if (raycast.blockPos[i] == CS) {
                            chunkPos[i]++;
                            auto chunk = world.GetChunk(chunkPos);
                            if (chunk != nullptr) {
                                Vec3 blockPos = raycast.blockPos;
                                blockPos[i] = 0;
                                chunk->SetBlock(blockPos, AIR);
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
            const Raycaster::BlockRaycastResult raycast = Raycaster::BlockRaycast(world, camera.position, camera.front, 10.0f);
            if (raycast.chunk != nullptr && raycast.blockHit != AIR) {
                std::shared_ptr<Chunk> chunkToPlaceIn = nullptr;
                iVec3 blockPlacePosition = raycast.blockPos + raycast.normal;

                for (int i = 0; i < 3; i++) {
                    if (blockPlacePosition[i] == CS_P_MINUS_ONE) {
                        Vec3 chunkPos = raycast.chunk->GetPosition();
                        chunkPos[i]++;
                        chunkToPlaceIn = world.GetChunk(chunkPos);
                        if (chunkToPlaceIn != nullptr) {
                            raycast.chunk->SetBlock(blockPlacePosition, selectedBlock);
                            // If they are both opaque or both not opaque
                            if ((BlockData[selectedBlock].opaque) == (BlockData[raycast.blockHit].opaque)) {
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
                            raycast.chunk->SetBlock(blockPlacePosition, selectedBlock);
                            // If they are both opaque or both not opaque
                            if ((BlockData[selectedBlock].opaque) == (BlockData[raycast.blockHit].opaque)) {
                                raycast.chunk->CreateMesh();
                                raycast.chunk->BufferData();
                            }
                            blockPlacePosition[i] = CS;
                            goto place_block;
                        }
                    }
                }

                // otherwise our chunk is just the raycast chunk hit
                chunkToPlaceIn = raycast.chunk;

            place_block:
                if (chunkToPlaceIn != nullptr) {
                    chunkToPlaceIn->SetBlock(blockPlacePosition, selectedBlock);

                    if (boundingBox.IsColliding(world, camera.position - Vec3{ 0.0f, 0.75f, 0.0f })) {
                        chunkToPlaceIn->SetBlock(blockPlacePosition, AIR);
                        return;
                    }

                    chunkToPlaceIn->CreateMesh();
                    chunkToPlaceIn->BufferData();
                    BlockDataStruct blockData = BlockData[selectedBlock];
                    BlockSoundStruct soundData = BlockSounds[blockData.placeSoundID];
                    SoundEngine::GetEngine()->play3D(soundData.sounds[rand() % soundData.sounds.size()].c_str(), irrklang::vec3df(camera.position));

                    for (int i = 0; i < 3; i++) {
                        if (blockPlacePosition[i] == CS) {
                            Vec3 chunkPos = chunkToPlaceIn->GetPosition();
                            chunkPos[i]++;
                            auto adjacentChunk = world.GetChunk(chunkPos);
                            if (adjacentChunk != nullptr) {
                                Vec3 blockPos = blockPlacePosition;
                                blockPos[i] = 0;
                                adjacentChunk->SetBlock(blockPos, selectedBlock);
                                blockPos[i] = 1;
                                if ((BlockData[selectedBlock].opaque) == (BlockData[adjacentChunk->GetBlock(blockPos)].opaque)) {
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
                                blockPos[i] = CS_P_MINUS_ONE;
                                adjacentChunk->SetBlock(blockPos, selectedBlock);
                                blockPos[i] = CS;
                                if ((BlockData[selectedBlock].opaque) == (BlockData[adjacentChunk->GetBlock(blockPos)].opaque)) {
                                    adjacentChunk->CreateMesh();
                                    adjacentChunk->BufferData();
                                }
                            }
                        }
                    }
                }
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
        directionMultiplier = Vec3{ camera.front[0], 0.0f, camera.front[2] }.normalized();
        break;
    }
    case PlayerMovement::BACKWARD: {
        velocity = deltaTime * movementSpeed;
        directionMultiplier = Vec3{ camera.front[0], 0.0f, camera.front[2] }.normalized() * -1.0f;
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

    Vec3 distanceToMove = directionMultiplier * velocity;
    Vec3 newPosition = camera.position;
    for (int axis = 0; axis < 3; axis++) {
        newPosition[axis] += distanceToMove[axis];
        if (boundingBox.IsColliding(world, newPosition - Vec3{ 0.0f, 0.75f, 0.0f })) {
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
