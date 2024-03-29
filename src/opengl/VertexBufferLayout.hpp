/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#ifndef VERTEX_BUFFERLAYOUT_H
#define VERTEX_BUFFERLAYOUT_H

#include <glad/glad.h>
#include <vector>

struct VertexBufferLayoutElement {
    unsigned int type;
    unsigned int count;
    GLboolean normalized;

    static unsigned int GetSize(GLenum type) {
        switch (type)
        {
        case GL_FLOAT:
        case GL_INT:
        case GL_UNSIGNED_INT:
            return 4;
        case GL_UNSIGNED_BYTE:
            return 1;
        }
        return 0;
    }
};

class VertexBufferLayout {
public:
    template <typename T>
    void AddAttribute(unsigned int count);

    std::vector<VertexBufferLayoutElement> GetElements() const;
    unsigned int GetStride() const;
private:
    unsigned int uStride{};
    std::vector<VertexBufferLayoutElement> mElements{};
};

#endif // !VERTEX_BUFFERLAYOUT_H

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