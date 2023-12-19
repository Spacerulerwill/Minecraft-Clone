/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#ifndef BUFFER_OBJECT_H
#define BUFFER_OBJECT_H

#include <glad/glad.h>

template <GLenum type>
class BufferObject {
private:
    GLuint uID = 0;
    GLsizeiptr mCount = 0;
public:
    BufferObject();
    BufferObject(const GLvoid* data, GLsizeiptr size, GLenum usage);
    ~BufferObject();
    BufferObject(const BufferObject&) = delete;
    BufferObject& operator=(const BufferObject&) = delete;
    BufferObject(BufferObject&&) noexcept;
    BufferObject& operator=(BufferObject&&) noexcept;
    void BufferData(const GLvoid* data, GLsizeiptr size, GLenum usage);
    void BufferSubData(const GLvoid* data, GLintptr offset, GLsizeiptr size);
    void Bind() const;
    void Unbind() const;
    GLsizeiptr GetCount() const;
};

using VertexBuffer = BufferObject<GL_ARRAY_BUFFER>;
using ElementBuffer = BufferObject<GL_ELEMENT_ARRAY_BUFFER>;

#endif // !BUFFER_OBJECT_H

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