/*
Copyright (C) 2023 William Redding - All Rights Reserved
LICENSE: MIT
*/

#ifndef CAMERA_H
#define CAMERA_H

#include <math/Vec3.hpp>
#include <math/Mat4.hpp>
#include <math/Math.hpp>
#include <glad/gl.h>
#include <vector>

// Abstraction to stay away from window specific input methods
namespace engine {
    enum CameraMovement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };

    class Camera
    {
    public:
        // camera attributes
        engine::Vec3 m_Position = engine::Vec3(0.0f, 0.0f, 0.0f);
        engine::Vec3 m_Front = engine::Vec3(0.0f, 0.0f, -1.0f);
        engine::Vec3 m_Up = engine::Vec3(0.0f, 1.0f, 0.0f);
        engine::Vec3 m_Right = engine::Vec3(0.0f, 0.0f, 1.0f);
        engine::Vec3 m_WorldUp = engine::Vec3(0.0f, 1.0f, 0.0f);

        // euler angles
        float m_Yaw = -90.0f;
        float m_Pitch = 0.0f;

        // camera options
        float m_MovementSpeed = 2.5f;
        float m_MouseSensitivity = 0.1f;
        float m_FOV = 45.0f;

        Camera();
        Camera(engine::Vec3 pos);

        engine::Mat4 GetViewMatrix() const;
        void ProcessKeyboard(CameraMovement direction, float deltaTime);
        void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch);
        void ProcessMouseScroll(float yoffset);
    private:
        // calculates the front vector from the Camera's (updated) Euler Angles
        void UpdateCameraVectors();
    };
}

#endif // !CAMERA_H

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