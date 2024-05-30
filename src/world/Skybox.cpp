/*
Copyright (C) 2023 William Redding - All Rights Reserved
LICENSE: MIT
*/

#include <opengl/VertexBufferLayout.hpp>
#include <world/Skybox.hpp>
#include <util/Log.hpp>

float Skybox::sSkyboxVertices[108] = {
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
    1.0f, -1.0f,  1.0f
};

std::array<std::string, 6> Skybox::sDaySkyboxFaces = {
    "textures/skybox/day_right.jpg",
    "textures/skybox/day_left.jpg",
    "textures/skybox/day_top.jpg",
    "textures/skybox/day_bottom.jpg",
    "textures/skybox/day_front.jpg",
    "textures/skybox/day_back.jpg" 
};

std::array<std::string, 6> Skybox::sNightSkyboxFaces = {
    "textures/skybox/night_right.png",
    "textures/skybox/night_left.png",
    "textures/skybox/night_top.png",
    "textures/skybox/night_bottom.png",
    "textures/skybox/night_front.png",
    "textures/skybox/night_back.png"
};

std::array<std::string, 6> Skybox::sTransitionSkyboxFaces = {
    "textures/skybox/transition_right.jpg",
    "textures/skybox/transition_left.jpg",
    "textures/skybox/transition_top.jpg",
    "textures/skybox/transition_bottom.jpg",
    "textures/skybox/transition_front.jpg",
    "textures/skybox/transition_back.jpg"
};

Skybox::Skybox()
{
    mDayCubemap = Cubemap(sDaySkyboxFaces, GL_TEXTURE0);
    mNightCubemap = Cubemap(sNightSkyboxFaces, GL_TEXTURE1);
    mTransitionCubemap = Cubemap(sTransitionSkyboxFaces, GL_TEXTURE2);

    // Setup Buffers
    VertexBufferLayout bufLayout;
    bufLayout.AddAttribute<float>(3);
    mVBO.BufferData(sSkyboxVertices, sizeof(sSkyboxVertices), GL_STATIC_DRAW);
    mVAO.AddBuffer(mVBO, bufLayout);
}

void Skybox::Draw(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model, double currentDayProgress) {
    mShader.Bind();
    mVAO.Bind();
    mShader.SetInt("day_skybox", 0);
    mShader.SetInt("night_skybox", 1);
    mShader.SetInt("transition_skybox", 2);
    mShader.SetMat4("projection", projection);
    mShader.SetMat4("view", view);
    mShader.SetMat4("model", model);
    mShader.SetFloat("current_day_progress", static_cast<float>(currentDayProgress));
    glDrawArrays(GL_TRIANGLES, 0, 36);
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
