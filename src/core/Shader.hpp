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
#include <math/Vec3.hpp>

namespace engine {
    struct ShaderSources {
        std::string Vertex;
        std::string Fragment;
    };

    class Shader {
    protected:
        unsigned int u_ID;
        std::string m_Filepath;
        std::unordered_map<std::string, int> m_UnformLocation;

    public:
        Shader(std::string filepath);
        ~Shader();

        void Bind() const;
        void Unbind() const;
        int GetLocation(std::string name);

        ShaderSources ParseShader(const std::string& filepath);
        unsigned int CompileShader(unsigned int type, std::string& source);
        unsigned int CreateShader(std::string& vertex_source, std::string& fragmement_source);
        void setMat4(const std::string& name, const Mat4& mat);
        void setIVec3(const std::string& name, int x, int y, int z);
        void setFloat(const std::string& name, float x);
        void setVec3(const std::string& name, const Vec3& vec);
        void SetInt(const std::string& name, int val);

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