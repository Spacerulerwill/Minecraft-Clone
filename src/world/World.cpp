/*
Copyright (C) 2023 William Redding - All Rights Reserved
LICENSE: MIT
*/

#include <world/World.hpp>
#include <GLFW/glfw3.h>
#include <util/Log.hpp>
#include <util/IO.hpp>
#include <fmt/format.h>
#include <util/Exceptions.hpp>
#include <world/Player.hpp>
#include <util/Log.hpp>

engine::World::World(const char* worldName) : m_WorldName(worldName) {
    bool success;

    WorldSave worldSave;
    success = ReadStructFromDisk(fmt::format("worlds/{}/world.dat", worldName), worldSave);
    if (!success)
        throw WorldCorruptException(fmt::format("Failed to read world data. File worlds/{}/world.dat is corrupted or missing!", worldName));

    PlayerSave playerSave;
    success = ReadStructFromDisk(fmt::format("worlds/{}/player.dat", worldName), playerSave);
    if (!success)
        throw WorldCorruptException(fmt::format("Failed to read world data. File worlds/{}/player.data is corrupted or missing!", worldName));
    m_Player = Player(playerSave.position, playerSave.pitch, playerSave.yaw);

    m_Noise.reseed(worldSave.seed);
}

engine::Player& engine::World::GetPlayer() {
    return m_Player;
}

const char* engine::World::GetName() {
    return m_WorldName;
}

void engine::World::Draw(Shader& chunkShader, Shader& waterShader, Shader& customModelShader) {
    Mat4<float> perspective = m_Player.GetCamera().GetPerspectiveMatrix();
    Mat4<float> view = m_Player.GetCamera().GetViewMatrix();
    const VoxelRaycastResult& raycastResult = m_Player.GetBlockRaycastResult();

    chunkShader.Bind();
    chunkShader.setMat4("projection", perspective);
    chunkShader.setMat4("view", view);
    chunkShader.SetInt("tex_array", 0);
    chunkShader.SetInt("grass_mask", 2);
    m_ChunkRegion.DrawOpaque(chunkShader);
    
    //glDisable(GL_CULL_FACE);
    customModelShader.Bind();
    customModelShader.setMat4("projection", perspective);
    customModelShader.setMat4("view", view);
    customModelShader.SetInt("tex_array", 0);
    m_ChunkRegion.DrawCustomModel(customModelShader);
    //glEnable(GL_CULL_FACE);

    glDepthFunc(GL_LEQUAL);
    m_Skybox.Draw(perspective, translationRemoved(view));
    glDepthFunc(GL_LESS);

    glEnable(GL_BLEND);
    waterShader.Bind();
    waterShader.setMat4("projection", perspective);
    waterShader.setMat4("view", view);
    waterShader.SetInt("tex_array", 0);
    m_ChunkRegion.DrawWater(waterShader);
    glDisable(GL_BLEND);   

    m_ChunkRegion.BufferChunksPerFrame(20);
}

void engine::World::GenerateChunks() {
    m_ChunkRegion.GenerateChunks(m_Noise, gen, distrib);
}

engine::World::~World() {
    // Save player position and view direction
    PlayerSave playerSave {
        .position = m_Player.GetCamera().GetPosition(),
        .pitch = m_Player.GetCamera().GetPitch(),
        .yaw = m_Player.GetCamera().GetYaw()
    };
    WriteStructToDisk(fmt::format("worlds/{}/player.dat", m_WorldName), playerSave);
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