/*
Copyright (C) 2023 William Redding - All Rights Reserved
LICENSE: MIT
*/

#ifndef SKYBOX_H
#define SKYBOX_H

#include <glad/gl.h>
#include <opengl/BufferObject.hpp>
#include <opengl/VertexArray.hpp>
#include <core/Shader.hpp>
#include <math/Mat4.hpp>
#include <opengl/Texture.hpp>

namespace engine {
	class Skybox {
	private:
        static float m_SkyboxVertices[108];
        static const char* m_SkyboxFaces[6];

        VertexBuffer m_VBO;
        VertexArray m_VAO;
        Shader m_Shader = Shader("res/shaders/skybox.shader");
        Cubemap m_Cubemap;

	public:
        Skybox();
		void Draw(const Mat4<float>& projection, const Mat4<float>& view);
        Shader& GetShader();
	};
}

#endif // !SKYBOX_H