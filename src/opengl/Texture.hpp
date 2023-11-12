/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/gl.h>
#include <util/Log.hpp>
#include <stb_image.h>

namespace engine {
    template<GLenum type>
    class Texture {
        private:
            GLuint m_ID = 0;
        public:
            Texture(): m_ID(0) {}
            Texture(const Texture &) = delete;
            Texture& operator=(const Texture&) = delete;
            
            Texture(Texture &&other) : m_ID(other.m_Id)
            {
                other.m_ID = 0;
            }

            Texture& operator=(Texture&& other)
            {
                if(this != &other)
                {
                    glDeleteTextures(1, &m_ID);
                    std::swap(m_ID, other.m_ID);
                }
                return *this;
            }

            Texture(const char* path) requires (type == GL_TEXTURE_2D) { 
                glGenTextures(1, &m_ID);
                glBindTexture(GL_TEXTURE_2D, m_ID);

                int width, height, nrComponents;
                unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
                if (data)
                {
                    GLenum format;  
                    switch (nrComponents) {
                        case 1: {
                            format = GL_RED;
                            break;
                        }
                        case 3: {
                            format = GL_RGB;
                            break;
                        }
                        case 4: {
                            format = GL_RGBA;
                            break;
                        }
                        default: {
                            format = GL_RGBA;
                            LOG_ERROR(fmt::format("Unsupported byte format for 2D texture array at path: {}. Using GL_RGBA.", path));
                            break;
                        }
                    }
                    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                    glGenerateMipmap(GL_TEXTURE_2D);

                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

                    stbi_image_free(data);
                }
                else
                {
                    LOG_ERROR(fmt::format("Texture failed to load at path: {}", path));
                    stbi_image_free(data);
                }
            }

            Texture(const char* faces[6]) requires (type == GL_TEXTURE_CUBE_MAP) { 
                glGenTextures(1, &m_ID);
                glBindTexture(GL_TEXTURE_CUBE_MAP, m_ID);

                int width, height, nrComponents;
                for (unsigned int i = 0; i < 6; i++)
                {
                    unsigned char* data = stbi_load(faces[i], &width, &height, &nrComponents, 0);
                    if (data)
                    {
                        GLenum format;  
                        switch (nrComponents) {
                            case 1: {
                                format = GL_RED;
                                break;
                            }
                            case 3: {
                                format = GL_RGB;
                                break;
                            }
                            case 4: {
                                format = GL_RGBA;
                                break;
                            }
                            default: {
                                format = GL_RGBA;
                                LOG_ERROR(fmt::format("Unsupported byte format for 2D texture array at path: {}. Using GL_RGBA.", faces[i]));
                                break;
                            }
                        }
                        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                        stbi_image_free(data);
                    }
                    else
                    {
                        LOG_ERROR(fmt::format("Failed to load Cubemap texture at path {}", faces[i]));
                        stbi_image_free(data);
                    }
                }
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
            }
            
            Texture(const char* path, int tilesize) requires (type == GL_TEXTURE_2D_ARRAY) {
                glGenTextures(1, &m_ID);
                glBindTexture(GL_TEXTURE_2D_ARRAY, m_ID);

                int width, height, nrComponents;
                unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
                if (data)
                {
                    GLenum format;  
                    switch (nrComponents) {
                        case 1: {
                            format = GL_RED;
                            break;
                        }
                        case 3: {
                            format = GL_RGB;
                            break;
                        }
                        case 4: {
                            format = GL_RGBA;
                            break;
                        }
                        default: {
                            format = GL_RGBA;
                            LOG_ERROR(fmt::format("Unsupported byte format for 2D texture array at path: {}. Using GL_RGBA.", path));
                            break;
                        }
                    }

                    glActiveTexture(GL_TEXTURE0);
                    glTexImage3D(GL_TEXTURE_2D_ARRAY,
                        0,                 // mipmap level
                        GL_RGBA8,          // gpu texel format
                        tilesize,             // width
                        tilesize,             // height
                        height / tilesize,             // depth
                        0,                 // border
                        format,      // cpu pixel format
                        GL_UNSIGNED_BYTE,  // cpu pixel coord type
                        data);           // pixel data

                    glTexParameteri(GL_TEXTURE_2D_ARRAY,
                        GL_TEXTURE_MIN_FILTER,
                        GL_NEAREST_MIPMAP_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D_ARRAY,
                        GL_TEXTURE_MAG_FILTER,
                        GL_NEAREST);
                    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL, 3);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                                        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
                    stbi_image_free(data);
                }
                else
                {
                    LOG_ERROR(fmt::format("Texture array failed to load at path: {}", path));
                    stbi_image_free(data);
                }
            }

            ~Texture() {
                glDeleteTextures(1, &m_ID);
            }

            void Bind() const {
                glBindTexture(type, m_ID);
            }

            void Unbind() const {
                glBindTexture(type, 0);
            }        
    };

    // Useful synonyms
    using Tex2D = Texture<GL_TEXTURE_2D>;
    using TexArray2D = Texture<GL_TEXTURE_2D_ARRAY>;
    using Cubemap = Texture<GL_TEXTURE_CUBE_MAP>;
};

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