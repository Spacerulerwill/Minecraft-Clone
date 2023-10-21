/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include "core/Shader.hpp"
#include <glad/gl.h>
#include <util/Log.hpp>
#include <fmt/format.h>

engine::Shader::Shader(std::string filepath) : m_Filepath(filepath) {
    ShaderSources shaders = ParseShader(filepath);
    u_ID = CreateShader(shaders.Vertex, shaders.Fragment);
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

void engine::Shader::setFloat(const std::string& name, float x) {
    glUniform1f(GetLocation(name), x);
}

void engine::Shader::SetInt(const std::string& name, int val)
{
    glUniform1i(GetLocation(name), val);
}

void engine::Shader::Bind() const {
    glUseProgram(u_ID);
}

void engine::Shader::Unbind() const {
    glUseProgram(0);
}

GLint engine::Shader::GetLocation(std::string name) {
    if (m_UnformLocation.find(name) != m_UnformLocation.end()) {
        return m_UnformLocation[name];
    }
    GLint location = glGetUniformLocation(u_ID, name.c_str());
    if (location == -1) {
        LOG_WARNING(fmt::format("Location of {} not found!", name));
    }
    m_UnformLocation[name] = location;
    return location;
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