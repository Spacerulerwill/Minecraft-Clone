/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include <opengl/BufferObject.hpp>
#include <glad/gl.h>
#include <utility>

template<GLenum type>
BufferObject<type>::BufferObject() {
    glGenBuffers(1, &uID);
}

template<GLenum type>
BufferObject<type>::BufferObject(const GLvoid* data, GLsizeiptr size, GLenum usage) {
    glGenBuffers(1, &uID);
    BufferData(data, size, usage);
}

template<GLenum type>
BufferObject<type>::~BufferObject() {
    glDeleteBuffers(1, &uID);
}

template<GLenum type>
BufferObject<type>::BufferObject(BufferObject&& other) noexcept : uID(other.uID), mCount(other.mCount)
{
    other.uID = 0;
    other.mCount = 0;
}

template<GLenum type>
BufferObject<type>& BufferObject<type>::operator=(BufferObject&& other) noexcept
{
    if (this != &other)
    {
        glDeleteBuffers(1, &uID);
        uID = std::exchange(other.uID, 0);
        mCount = std::exchange(other.mCount, 0);
    }
    return *this;
}

template<GLenum type>
void BufferObject<type>::BufferData(const GLvoid* data, GLsizeiptr size, GLenum usage)
{
    glBindBuffer(type, uID);
    glBufferData(type, size, data, usage);
}

template<GLenum type>
void BufferObject<type>::BufferSubData(const GLvoid* data, GLintptr offset, GLsizeiptr size)
{
    glBindBuffer(type, uID);
    glBufferSubData(type, offset, size, data);
}

template<GLenum type>
void BufferObject<type>::Bind() const
{
    glBindBuffer(type, uID);
}

template<GLenum type>
void BufferObject<type>::Unbind() const
{
    glBindBuffer(type, 0);
}

template<GLenum type>
GLsizeiptr BufferObject<type>::GetCount() const
{
    return mCount;
}

template class BufferObject<GL_ARRAY_BUFFER>;
template class BufferObject<GL_ELEMENT_ARRAY_BUFFER>;

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