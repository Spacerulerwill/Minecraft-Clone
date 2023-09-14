/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include <opengl/VertexBufferLayout.hpp>

engine::VertexBufferLayout::VertexBufferLayout() :m_Stride(0)
{
}

template <>
void engine::VertexBufferLayout::AddAttribute<float>(unsigned int count)
{
    m_Elements.push_back({ GL_FLOAT, count, GL_FALSE });
    m_Stride += VertexBufferLayoutElement::GetSize(GL_FLOAT) * count;
}

template <>
void engine::VertexBufferLayout::AddAttribute<unsigned int>(unsigned int count)
{
    m_Elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE});
    m_Stride += VertexBufferLayoutElement::GetSize(GL_UNSIGNED_INT) * count;
}

template <>
void engine::VertexBufferLayout::AddAttribute<int>(unsigned int count)
{
    m_Elements.push_back({ GL_INT, count, GL_FALSE });
    m_Stride += VertexBufferLayoutElement::GetSize(GL_INT) * count;
}


std::vector<engine::VertexBufferLayoutElement> engine::VertexBufferLayout::GetElements() const
{
	return m_Elements;
}

unsigned int engine::VertexBufferLayout::GetStride() const
{
	return m_Stride;
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
