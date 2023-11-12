/*
Copyright (C) 2023 William Redding - All Rights Reserved
LICENSE: MIT
*/

#ifndef BUFFEROBJECT_H
#define BUFFEROBJECT_H

#include <glad/gl.h>
#include <util/Log.hpp>

#define BUFFER_REQUIREMENT (buffer == GL_ARRAY_BUFFER || buffer == GL_ELEMENT_ARRAY_BUFFER) 

namespace engine {
    template <GLenum buffer>
    class BufferObject {
        private:
            GLuint m_ID = 0;
            GLsizeiptr m_Count = 0;
        public:
            BufferObject() requires BUFFER_REQUIREMENT {
                glGenBuffers(1, &m_ID);
            }

            BufferObject(const GLvoid* data, GLsizeiptr count, GLenum usage) requires BUFFER_REQUIREMENT {
                glGenBuffers(1, &m_ID);
                BufferData(data, count, usage);
            }

            ~BufferObject() {
                glDeleteBuffers(1, &m_ID);
            }

            BufferObject(const BufferObject &) = delete;

            BufferObject& operator=(const BufferObject&) = delete;

            BufferObject(BufferObject &&other) : m_ID(other.m_ID), m_Count(other.m_Count)
            {
                other.m_ID = 0;
            }

            BufferObject& operator=(BufferObject&& other)
            {
                if(this != &other)
                {
                    glDeleteBuffers(1, &m_ID);
                    std::swap(m_ID, other.m_ID);
                    m_Count = std::move(other.m_Count);
                }
                return *this;
            }

            void BufferData(const GLvoid* data, GLsizeiptr size, GLenum usage) const {
                glBindBuffer(buffer, m_ID);
                glBufferData(buffer, size, data, usage);
            }

            void BufferSubData(const GLvoid* data, GLintptr offset, GLsizeiptr size) const {
                glBindBuffer(buffer, m_ID);
                glBufferSubData(buffer, offset, size, data);
            }

            void Bind() const {
                glBindBuffer(buffer, m_ID);
            }

            void Unbind() const {
                glBindBuffer(buffer, 0);
            }
            
            GLsizeiptr GetCount() const {
                return m_Count;
            }
    };

    using VertexBuffer = BufferObject<GL_ARRAY_BUFFER>;
    using ElementBuffer = BufferObject<GL_ELEMENT_ARRAY_BUFFER>;
}

#endif // !BUFFEROBJECT_H

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
