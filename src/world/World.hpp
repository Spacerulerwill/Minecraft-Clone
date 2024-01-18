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

iVec3 GetWorldBlockPosFromGlobalPos(Vec3 globalPosition);
iVec3 GetChunkPosFromGlobalBlockPos(iVec3 globalBlockPos);
iVec3 GetChunkBlockPosFromGlobalBlockPos(iVec3 pos);

constexpr const float GRAVITY = 0.5f;

class World {
private:
    std::unordered_map<iVec2, ChunkStack> mChunkStacks;
    Skybox mSkybox;
    Shader chunkShader = Shader("shaders/chunk.shader");
    Shader waterShader = Shader("shaders/water.shader");
    siv::PerlinNoise mPerlin = siv::PerlinNoise(0);
    BS::thread_pool mLoadPool;
    TexArray2D mTextureAtlas = TexArray2D("textures/atlas.png", TEXTURE_SIZE, GL_TEXTURE0);
    Tex2D mGrassSideMask = Tex2D("textures/block/mask/grass_side_mask.png", GL_TEXTURE1);
public:
    GLint LOD = 1;
    int mRenderDistance = 5;
    int mBufferPerFrame = 20;
    Player player;
    void Draw(const Frustum& frustum, int* totalChunks, int* chunksDrawn);
    void GenerateChunks();

    const ChunkStack* GetChunkStack(iVec2 pos) const;
    std::shared_ptr<Chunk> GetChunk(iVec3 pos) const;
    BlockID GetBlock(iVec3 pos) const;
    void SetBlock(iVec3 pos, BlockID block);
    void SetBlockAndRemesh(iVec3 pos, BlockID block);
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