/*
Copyright (C) 2023 William Redding - All Rights Reserved
LICENSE: MIT
*/

#ifndef SKYBOX_H
#define SKYBOX_H

#include <glad/glad.h>
#include <string>
#include <array>
#include <opengl/BufferObject.hpp>
#include <opengl/VertexArray.hpp>
#include <opengl/Shader.hpp>
#include <glm/mat4x4.hpp>
#include <opengl/Texture.hpp>

class Skybox {
private:
    static float sSkyboxVertices[108];
    static std::array<std::string, 6> sDaySkyboxFaces;
    static std::array<std::string, 6> sNightSkyboxFaces;
    static std::array<std::string, 6> sTransitionSkyboxFaces;

    VertexBuffer mVBO;
    VertexArray mVAO;
    Shader mShader = Shader("shaders/skybox.shader");
    Cubemap mDayCubemap;
    Cubemap mNightCubemap;
    Cubemap mTransitionCubemap;
public:
    Skybox();
    void Draw(const glm::mat4& projection, const glm::mat4& view, const glm::mat4& model, double currentDayProgress);
};

#endif // !SKYBOX_H
