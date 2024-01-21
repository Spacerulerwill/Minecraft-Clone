/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include <opengl/Shader.hpp>
#include <math/Matrix.hpp>
#include <math/Vector.hpp>
#include <glad/glad.h>
#include <util/Log.hpp>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <fmt/format.h>

Shader::Shader(std::string filepath) : mFilepath(filepath) {
    ShaderSources shaders = ParseShader(filepath);
    uID = CreateShader(shaders.Vertex, shaders.Fragment);
    GetShaderUniformLocations();
}

Shader::Shader(Shader&& other) noexcept : uID(other.uID), mFilepath(std::move(other.mFilepath)), mUniformLocations(std::move(other.mUniformLocations))
{
    other.uID = 0;
    other.mFilepath = {};
    other.mUniformLocations = {};
}

Shader& Shader::operator=(Shader&& other) noexcept
{
    if (this != &other) {
        glDeleteShader(uID);
        uID = std::exchange(other.uID, 0);
        mFilepath = std::exchange(other.mFilepath, {});
        mUniformLocations = std::exchange(other.mUniformLocations, {});
    }
    return *this;
}
ShaderSources Shader::ParseShader(const std::string& filepath) {

    std::ifstream stream(filepath);

    if (stream.fail()) {
        LOG_ERROR("Shader file at location {} not found", filepath);
    }

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos) {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos) {
                type = ShaderType::FRAGMENT;
            }
        }
        else if (type != ShaderType::NONE) {
            ss[static_cast<int>(type)] << line << "\n";
        }
    }

    return {
        ss[static_cast<int>(ShaderType::VERTEX)].str(),
        ss[static_cast<int>(ShaderType::FRAGMENT)].str()
    };
}

GLuint Shader::CompileShader(GLenum type, std::string& source) {
    GLuint id = glCreateShader(type);
    const char* sourceStr = source.c_str();
    glShaderSource(id, 1, &sourceStr, nullptr);
    glCompileShader(id);

    GLint result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        GLsizei length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        std::vector<char> infoLog(length);
        glGetShaderInfoLog(id, length, &length, infoLog.data());
        glDeleteShader(id);
        throw std::runtime_error(fmt::format("Failed to compile {} shader at path: {}\n{}", type == GL_VERTEX_SHADER ? "vertex" : "fragment", mFilepath, infoLog.data()));
    }
    return id;
}

GLuint Shader::CreateShader(std::string& vertex_source, std::string& fragmement_source) {

    GLuint program = glCreateProgram();
    GLuint vs = CompileShader(GL_VERTEX_SHADER, vertex_source);
    GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fragmement_source);

    glAttachShader(program, vs);
    glAttachShader(program, fs);

    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

void Shader::GetShaderUniformLocations() {
    GLint i;
    GLint count;

    GLint size;
    GLenum type;

    const GLsizei bufSize = 100;
    GLchar name[bufSize];
    GLsizei length;

    glGetProgramiv(uID, GL_ACTIVE_UNIFORMS, &count);
    for (i = 0; i < count; i++)
    {
        glGetActiveUniform(uID, static_cast<GLuint>(i), bufSize, &length, &size, &type, name);
        mUniformLocations[name] = i;
    }
}

void Shader::SetMat4(const std::string& name, const Mat4& mat) {
    glUniformMatrix4fv(GetLocation(name), 1, GL_TRUE, mat.GetPointer());
}

void Shader::SetMat3(const std::string& name, const Mat3& mat) {
    glUniformMatrix3fv(GetLocation(name), 1, GL_TRUE, mat.GetPointer());
}

void Shader::SetMat2(const std::string& name, const Mat2& mat) {
    glUniformMatrix2fv(GetLocation(name), 1, GL_TRUE, mat.GetPointer());
}

void Shader::SetiVec4(const std::string& name, const iVec4& vec) {
    glUniform4iv(GetLocation(name), 1, vec.GetPointer());
}

void Shader::SetiVec3(const std::string& name, const iVec3& vec) {
    glUniform3iv(GetLocation(name), 1, vec.GetPointer());
}

void Shader::SetiVec2(const std::string& name, const iVec2& vec) {
    glUniform2iv(GetLocation(name), 1, vec.GetPointer());
}

void Shader::SetVec4(const std::string& name, const Vec4& vec) {
    glUniform4fv(GetLocation(name), 1, vec.GetPointer());
}

void Shader::SetVec3(const std::string& name, const Vec3& vec) {
    glUniform3fv(GetLocation(name), 1, vec.GetPointer());
}


void Shader::SetVec2(const std::string& name, const Vec2& vec) {
    glUniform2fv(GetLocation(name), 1, vec.GetPointer());
}

void Shader::SetFloat(const std::string& name, GLfloat x) {
    glUniform1f(GetLocation(name), x);
}

void Shader::SetInt(const std::string& name, GLint val)
{
    glUniform1i(GetLocation(name), val);
}

void Shader::Bind() const {
    glUseProgram(uID);
}

void Shader::Unbind() const {
    glUseProgram(0);
}

GLint Shader::GetLocation(std::string name) const {
    try {
        return mUniformLocations.at(name);
    }
    catch (const std::out_of_range&) {
        LOG_WARNING("Location of {} not found!", name);
        return -1;
    }
}


Shader::~Shader() {
    glDeleteProgram(uID);
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
