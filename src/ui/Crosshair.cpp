/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include <ui/Crosshair.hpp>
#include <glad/glad.h>

Crosshair::Crosshair(int initialWindowWidth, int initialWindowHeight, unsigned int size) {
    float bottom_corner_x = (initialWindowWidth / 2.0f) - size / 2.0f;
    float bottom_corner_y = (initialWindowHeight / 2.0f) - size / 2.0f;
    float crosshairVerts[24] = {
        bottom_corner_x, bottom_corner_y, 0.0f, 0.0f, // bottom left
        bottom_corner_x + size, bottom_corner_y, 1.0f, 0.0f, // bottom right
        bottom_corner_x + size,  bottom_corner_y + size, 1.0f, 1.0f, // top right
        bottom_corner_x, bottom_corner_y, 0.0f, 0.0f, // bottom left
        bottom_corner_x + size,  bottom_corner_y + size, 1.0f, 1.0f, // top right
        bottom_corner_x, bottom_corner_y + size, 0.0f, 1.0f,  // top left
    };
    mVBO.BufferData((const void*)crosshairVerts, sizeof(crosshairVerts), GL_STATIC_DRAW);
    VertexBufferLayout bufferLayout;
    bufferLayout.AddAttribute<float>(4);
    mVAO.AddBuffer(mVBO, bufferLayout);
}

void Crosshair::Draw(const Mat4& ortho) {
	glActiveTexture(GL_TEXTURE2);
	mShader.Bind();
	mVAO.Bind();
	mShader.SetInt("screenTexture", 0);
	mShader.SetInt("crosshair", 2);
	mShader.SetMat4("projection", ortho);
	glDrawArrays(GL_TRIANGLES, 0, 6);
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
