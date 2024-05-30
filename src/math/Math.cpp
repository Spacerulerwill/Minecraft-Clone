/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include <math/Math.hpp>
#include <irrKlang/irrKlang.h>
#include <glm/vec3.hpp>
#include <fmt/format.h>

float radians(float theta)
{
    return theta * PI_OVER_180;
}

float degrees(float theta)
{
    return theta * ONE_EIGHTY_OVER_PI;
}

glm::vec3 irrklang_vec3df_to_glm_vec3(irrklang::vec3df vec)
{
    return glm::vec3(vec.X, vec.Y, vec.Z);
}

std::string vec3_to_string(glm::vec3 vec)
{
    return fmt::format("({}, {}, {})", vec.x, vec.y, vec.z);
}

irrklang::vec3df glm_vec3_to_irrklang_vec3df(glm::vec3 vec)
{
    return irrklang::vec3df(vec.x, vec.y, vec.z);
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
