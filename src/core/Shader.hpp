/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#ifndef SHADER_H
#define SHADER_H

#include <glad/gl.h>
#include <string>
#include <fstream>
#include <sstream>
#include <util/Log.hpp>
#include <unordered_map>
#include <math/Mat4.hpp>
#include <math/Mat3.hpp>
#include <math/Mat2.hpp>
#include <math/Vec4.hpp>
#include <math/Vec3.hpp>
#include <math/Vec2.hpp>
#include <util/Concepts.hpp>

namespace engine {
    struct ShaderSources {
        std::string Vertex;
        std::string Fragment;
    };

    enum ShaderType {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    class Shader {
    private:
        GLuint u_ID;
        std::string m_Filepath;
        std::unordered_map<std::string, int> m_UnformLocations;

        ShaderSources ParseShader(const std::string& filepath);
        GLuint CompileShader(GLenum type, std::string& source);
        GLuint CreateShader(std::string& vertex_source, std::string& fragmement_source);
        void GetShaderUniformLocations();
        GLint GetLocation(std::string name) const;
    public:
        Shader(std::string filepath);
        ~Shader();

        void Bind() const;
        void Unbind() const;

        void SetMat4(const std::string& name, const Mat4<float>& mat) const;
        void SetMat3(const std::string& name, const Mat3<float>& mat) const;
        void SetMat2(const std::string& name, const Mat2<float>& mat) const;
        void SetVec4(const std::string& name, const Vec4<float>& vec) const;
        void SetIVec4(const std::string& name, const Vec4<int>& vec) const;
        void SetIVec3(const std::string& name, const Vec3<int>& vec) const;
        void SetVec3(const std::string& name, const Vec3<float>& vec) const;
        void SetIVec2(const std::string& name, const Vec2<int>& vec) const;
        void SetVec2(const std::string& name, const Vec2<float>& vec) const;
        void SetFloat(const std::string& name, float x) const;
        void SetInt(const std::string& name, int val) const;

    };
}

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