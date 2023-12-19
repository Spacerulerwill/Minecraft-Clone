/*
Copyright (C) 2023 William Redding - All Rights Reserved
LICENSE: MIT
*/

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <glad/glad.h>
#include <opengl/Shader.hpp>
#include <opengl/BufferObject.hpp>
#include <opengl/VertexArray.hpp>

/*
MSAARenderer is an interface for rendering the screen to a texture with MSAA,
allowing for a post processing shader on the resultant texture.
*/
class MSAARenderer {
public:
    MSAARenderer(GLsizei width, GLsizei height);
    ~MSAARenderer();

    // Bind the MSAA framebuffer object for rendering
    void BindMSAAFBO() const;

    // Blit the MSAA FBO to the intermediate FBO, and draw that intermediate FBO onto a quad with a post process shader
    void Draw(const Shader& postprocessShader) const;

    static const unsigned int MSAA_SAMPLES = 16;
private:
    VertexBuffer m_VBO;
    VertexArray m_VAO;
    GLsizei width{};
    GLsizei height{};
    GLuint msaaFBO{};
    GLuint textureColorBufferMultiSampled{};
    GLuint rbo{};
    GLuint intermediateFBO{};
    GLuint screenTexture{};
};

#endif // !FRAMEBUFFER_H

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