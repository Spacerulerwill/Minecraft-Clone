/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>
#include <unordered_map>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat2x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

struct ShaderSources {
    std::string Vertex;
    std::string Fragment;
};

enum ShaderType {
    NONE = -1, VERTEX = 0, FRAGMENT = 1
};

class Shader {
private:
    GLuint uID{};
    std::string mFilepath{};
    std::unordered_map<std::string, GLint> mUniformLocations{};

    ShaderSources ParseShader(const std::string& filepath);
    GLuint CompileShader(GLenum type, std::string& source);
    GLuint CreateShader(std::string& vertexSource, std::string& fragmementSource);
    void GetShaderUniformLocations();
    GLint GetLocation(std::string name) const;
public:
    Shader(std::string filepath);
    Shader(const Shader& arg) = delete;
    Shader(const Shader&& arg) = delete;
    Shader(Shader&&) noexcept;
    Shader& operator=(Shader&&) noexcept;
    ~Shader();

    void Bind() const;
    void Unbind() const;

    void SetMat4(const std::string& name, const glm::mat4& mat);
    void SetMat3(const std::string& name, const glm::mat3& mat);
    void SetMat2(const std::string& name, const glm::mat2& mat);
    void SetiVec4(const std::string& name, const glm::ivec4& vec);
    void SetiVec3(const std::string& name, const glm::ivec3& vec);
    void SetiVec2(const std::string& name, const glm::ivec2& vec);
    void SetVec4(const std::string& name, const glm::vec4& vec);
    void SetVec3(const std::string& name, const glm::vec3& vec);
    void SetVec2(const std::string& name, const glm::vec2& vec);
    void SetFloat(const std::string& name, GLfloat x);
    void SetInt(const std::string& name, GLint val);
};

#endif // !SHADER_H

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