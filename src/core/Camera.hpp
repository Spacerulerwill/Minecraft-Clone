/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#ifndef CAMERA_H
#define CAMERA_H

#include <glad/gl.h>
#include <math/Vec3.hpp>
#include <math/Mat4.hpp>
#include <math/Math.hpp>

namespace engine {
    enum CameraMovement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT
    };
    class Camera {
        public:
            Camera();
            Camera(Vec3<float> pos);

            Mat4<float> GetViewMatrix() const;
            Mat4<float> GetPerspectiveMatrix() const;

            void SetFOV(float FOV);
            float GetFOV() const;

            void SetMovementSpeed(float speed);

            Vec3<float> GetPosition() const;
            Vec3<float> GetDirection() const;

            void ProcessKeyboard(CameraMovement direction, float deltaTime);
            void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch);
            void ProcessMouseScroll(float yoffset);

        private:
            Vec3<float> m_Position = Vec3<float>(0.0f, 0.0f, 0.0f);
            Vec3<float> m_Front;
            Vec3<float> m_Up;
            Vec3<float> m_Right;
            Vec3<float> m_WorldUp = Vec3<float>(0.0f, 1.0f, 0.0f);

            // euler angles
            float m_Yaw = -90.0f;
            float m_Pitch = 0.0f;

            // camera options
            float m_MovementSpeed = 2.5f;
            float m_MouseSensitivity = 0.1f;
            float m_Aspect = 16.0f / 9.0f;
            float m_Near = 0.1f;
            float m_Far = 5000.0f;
            float m_FOV = 45.0f;

            // Perspective Matrix
            Mat4<float> m_PerspectiveMatrix = perspective(radians(m_FOV), m_Aspect, m_Near, m_Far);

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