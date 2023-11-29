/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include "core/Shader.hpp"
#include <fmt/format.h>
#include <glad/gl.h>
#include <util/Log.hpp>

engine::Shader::Shader(std::string filepath) : m_Filepath(filepath) {
    ShaderSources shaders = ParseShader(filepath);
    u_ID = CreateShader(shaders.Vertex, shaders.Fragment);
    GetShaderUniformLocations();
}

engine::ShaderSources engine::Shader::ParseShader(const std::string& filepath) {

    std::ifstream stream(filepath);

    if (stream.fail()) {
        LOG_ERROR(fmt::format("Shader file at location {} not found", filepath));
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
        else {
            ss[(int)type] << line << "\n";
        }
    }

    return {
        ss[(int)ShaderType::VERTEX].str(),
        ss[(int)ShaderType::FRAGMENT].str()
    };
}

GLuint engine::Shader::CompileShader(GLenum type, std::string& source) {
    GLuint id = glCreateShader(type);
    const char* sourceStr = source.c_str();
    glShaderSource(id, 1, &sourceStr, nullptr);
    glCompileShader(id);

    GLint result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        GLsizei length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        GLchar* msg = new GLchar[length];
        glGetShaderInfoLog(id, length, &length, msg);
        LOG_ERROR(fmt::format("Failed to compile {} shader\n{}", (type == GL_VERTEX_SHADER ? "vertex" : "fragment"), msg));
        glDeleteShader(id);
        delete[] msg;
        return 0;
    }
    return id;
}

GLuint engine::Shader::CreateShader(std::string& vertex_source, std::string& fragmement_source) {

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

void engine::Shader::GetShaderUniformLocations() {

    GLint i;
    GLint count;

    GLint size;
    GLenum type;

    const GLsizei bufSize = 100;
    GLchar name[bufSize];
    GLsizei length;

    glGetProgramiv(u_ID, GL_ACTIVE_UNIFORMS, &count);
    for (i = 0; i < count; i++)
    {
        glGetActiveUniform(u_ID, static_cast<GLuint>(i), bufSize, &length, &size, &type, name);
        m_UnformLocations[name] = i;
    }
}

void engine::Shader::SetMat4(const std::string& name, const Mat4<float>& mat) const {
    glUniformMatrix4fv(GetLocation(name), 1, GL_TRUE, mat.GetPointer());
}

void engine::Shader::SetMat3(const std::string& name, const Mat3<float>& mat) const {
    glUniformMatrix3fv(GetLocation(name), 1, GL_TRUE, mat.GetPointer());
}

void engine::Shader::SetMat2(const std::string& name, const Mat2<float>& mat) const {
    glUniformMatrix2fv(GetLocation(name), 1, GL_TRUE, mat.GetPointer());
}

void engine::Shader::SetVec4(const std::string& name, const Vec4<float>& vec) const {
    glUniform4f(GetLocation(name), vec.x, vec.y, vec.z, vec.w);
}

void engine::Shader::SetIVec4(const std::string& name, const Vec4<int>& vec) const {
    glUniform4i(GetLocation(name), vec.x, vec.y, vec.z, vec.w);
}

void engine::Shader::SetIVec3(const std::string& name, const Vec3<int>& vec) const {
    glUniform3i(GetLocation(name), vec.x, vec.y, vec.z);
}

void engine::Shader::SetVec3(const std::string& name, const Vec3<float>& vec) const {
    glUniform3f(GetLocation(name), vec.x, vec.y, vec.z);
}

void engine::Shader::SetIVec2(const std::string& name, const Vec2<int>& vec) const {
    glUniform2i(GetLocation(name), vec.x, vec.y);
}

void engine::Shader::SetVec2(const std::string& name, const Vec2<float>& vec) const {
    glUniform2f(GetLocation(name), vec.x, vec.y);
}

void engine::Shader::SetFloat(const std::string& name, float x)  const {
    glUniform1f(GetLocation(name), x);
}

void engine::Shader::SetInt(const std::string& name, int val) const
{
    glUniform1i(GetLocation(name), val);
}

void engine::Shader::Bind() const {
    glUseProgram(u_ID);
}

void engine::Shader::Unbind() const {
    glUseProgram(0);
}

GLint engine::Shader::GetLocation(std::string name) const {
    try {
        return m_UnformLocations.at(name);
    }
    catch (const std::out_of_range& e) {
        LOG_WARNING(fmt::format("Location of {} not found!", name));
        return -1;
    }
}

engine::Shader::~Shader() {
    glDeleteProgram(u_ID);
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