/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <string>
#include <array>

template<GLenum type>
class Texture {
private:
    GLuint uID{};
public:
    Texture() = default;
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;
    Texture(std::string path, GLenum textureUnit) requires (type == GL_TEXTURE_2D);
    Texture(std::string path, GLsizei tilesize, GLenum textureUnit) requires (type == GL_TEXTURE_2D_ARRAY);
    Texture(std::array<std::string, 6> faces, GLenum textureUnit) requires (type == GL_TEXTURE_CUBE_MAP);
    ~Texture();
    void Bind() const;
    void Unbind() const;
};

using Tex2D = Texture<GL_TEXTURE_2D>;
using TexArray2D = Texture<GL_TEXTURE_2D_ARRAY>;
using Cubemap = Texture<GL_TEXTURE_CUBE_MAP>;

#endif // !TEXTURE_H

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
