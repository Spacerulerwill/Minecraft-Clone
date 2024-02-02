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
#include <math/Matrix.hpp>
#include <opengl/Texture.hpp>

class Skybox {
private:
    static float sSkyboxVertices[108];
    static std::array<std::string, 6> sDaySkyboxFaces;
	static std::array<std::string, 6> sNightSkyboxFaces;

    VertexBuffer mVBO;
    VertexArray mVAO;
    Shader mShader = Shader("shaders/skybox.shader");
    Cubemap mDayCubemap;
	Cubemap mNightCubemap;
public:
    Skybox();
    void Draw(const Mat4& projection, const Mat4& view, const Mat4& model);
};

#endif // !SKYBOX_H
