/*
Copyright (C) 2023 William Redding - All Rights Reserved
LICENSE: MIT
*/

#ifndef SKYBOX_H
#define SKYBOX_H

#include <glad/gl.h>
#include <opengl/BufferObject.hpp>
#include <opengl/VertexArray.hpp>
#include <opengl/Texture.hpp>
#include <core/Shader.hpp>
#include <math/Mat4.hpp>

namespace engine {
	class Skybox {
	private:
        static float m_SkyboxVertices[108];
        static const char* m_SkyboxFaces[6];

        BufferObject<GL_ARRAY_BUFFER> m_VBO;
        VertexArray m_VAO;
        Shader m_Shader = Shader("res/shaders/skybox.shader");
        Texture<GL_TEXTURE_CUBE_MAP> m_Cubemap = Texture<GL_TEXTURE_CUBE_MAP>(m_SkyboxFaces);
	public:
        Skybox();
		void Draw(const Mat4<float>& projection, const Mat4<float>& view);
        Shader& GetShader();
	};
}

#endif // !SKYBOX_H

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