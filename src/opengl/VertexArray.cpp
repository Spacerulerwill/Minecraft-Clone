/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include <opengl/VertexArray.hpp>
#include "glad/gl.h"
#include <util/Log.hpp>

engine::VertexArray::VertexArray()
{
    glGenVertexArrays(1, &m_ID);
}

engine::VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &m_ID);
}

void engine::VertexArray::AddBuffer(const engine::BufferObject<GL_ARRAY_BUFFER>& vb, const engine::VertexBufferLayout& layout)
{
    Bind();
    vb.Bind();
    std::vector<VertexBufferLayoutElement> elements = layout.GetElements();
    uintptr_t offset = 0;
    for (int i = 0; i < elements.size(); i++) {
        const VertexBufferLayoutElement element = elements[i];
        glEnableVertexAttribArray(i);

        if (element.type == GL_UNSIGNED_INT || element.type == GL_INT) {
            glVertexAttribIPointer(i, element.count, element.type, layout.GetStride(), (const void*)(offset));
        }
        else {
            glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (const void*)(offset));
        }
        offset += element.count * VertexBufferLayoutElement::GetSize(element.type);
    }
}

void engine::VertexArray::Bind() const
{
	glBindVertexArray(m_ID);

}

void engine::VertexArray::Unbind() const
{
	glBindVertexArray(0);
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
