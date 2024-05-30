/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#ifndef WORLD_H
#define WORLD_H

#include <world/Skybox.hpp>
#include <opengl/Shader.hpp>
#include <opengl/Texture.hpp>
#include <world/chunk/ChunkStack.hpp>
#include <world/Block.hpp>
#include <world/Player.hpp>
#include <math/Frustum.hpp>
#include <core/Camera.hpp>
#include <PerlinNoise.hpp>
#include <unordered_map>
#include <BS_thread_pool.hpp>
#include <array>
#include <stdexcept>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/hash.hpp"

glm::ivec3 GetWorldBlockPosFromGlobalPos(glm::vec3 globalPosition);
glm::ivec3 GetChunkPosFromGlobalBlockPos(glm::ivec3 globalBlockPos);
glm::ivec3 GetChunkBlockPosFromGlobalBlockPos(glm::ivec3 pos);

constexpr float GRAVITY = 0.5f;

struct WorldSave {
    siv::PerlinNoise::seed_type seed;
    double elapsedTime;
};

// Thrown when a world fails to load
class WorldCorruptionException : public std::runtime_error
{
public:
    WorldCorruptionException(const std::string& what) : std::runtime_error(what) {}
};

class World {
private:
    std::unordered_map<glm::ivec2, ChunkStack> mChunkStacks;
    Skybox mSkybox;
    Shader mChunkShader = Shader("shaders/chunk.shader");
    Shader mWaterShader = Shader("shaders/water.shader");
    Shader mCustomModelShader = Shader("shaders/custom_model.shader");
    siv::PerlinNoise mPerlin;
    BS::thread_pool mTaskPool;
    BS::thread_pool mUnloadPool;
    std::array<TexArray2D, MAX_ANIMATION_FRAMES> mTextureAtlases;
    Tex2D mGrassSideMask = Tex2D("textures/block/mask/grass_side_mask.png", GL_TEXTURE1);
    std::size_t mCurrentAtlasID{ 0 };
    double mLastAtlasSwitch = 0.0f;
    siv::PerlinNoise::seed_type mSeed;
    std::string mWorldDirectory;
public:
    World(std::string worldDirectory);
    ~World();
    static constexpr int MAX_GEN_HEIGHT = (ChunkStack::DEFAULT_SIZE * Chunk::SIZE) - 1;
    static constexpr int MIN_GEN_HEIGHT = Chunk::SIZE - 1;
    static constexpr int MAX_SUB_MIN_GEN_HEIGHT = MAX_GEN_HEIGHT - MIN_GEN_HEIGHT;
    static constexpr int WATER_LEVEL = MIN_GEN_HEIGHT + (Chunk::SIZE * 4);    
    static_assert(WATER_LEVEL < MAX_GEN_HEIGHT);
    static constexpr double DAY_DURATION = 120.0;
    glm::vec3 mWaterColor = glm::vec3( 68.0f, 124.0f, 245.0f ) / 255.0f;
    glm::vec3 mFoliageColor = glm::vec3( 145.0f, 189.0f, 89.0f ) / 255.0f;
    glm::vec3 mGrassColor = glm::vec3( 145.0f, 189.0f, 89.0f ) / 255.0f;
    int mChunkLoadDistance = 3;
    int mChunkPartialLoadDistance = 1;
    int mMaxTasksPerFrame = 20;
    Player mPlayer;
    void Draw(const Frustum& frustum, int* totalChunks, int* chunksDrawn);
    void TrySwitchToNextTextureAtlas();
    void GenerateChunks();
    double mWorldLoadedTime; // Time of program when world finishes loading
    double mWorldStartTime; // The last saved world time when the player last closed the game
    double mCurrentTime; // Current world time
    const ChunkStack* GetChunkStack(glm::ivec2 pos) const;
    std::shared_ptr<Chunk> GetChunk(glm::ivec3 pos) const;
    Block GetBlock(glm::ivec3 pos) const;
    void SetBlock(glm::ivec3 pos, Block block);
    void SetBlockAndRemesh(glm::ivec3 pos, Block block);
};

#endif // !WORLD_H

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
