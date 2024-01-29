/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <math/Math.hpp>
#include <math/Vector.hpp>
#include <math/Matrix.hpp>
#include <math/Frustum.hpp>

class World;

enum CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

struct Camera {
private:
    float lastMouseX = 0.0f;
    float lastMouseY = 0.0f;

    float near = 0.1f;
    float far = 5000.0f;
    float aspect = 16.0f / 9.0f;
    float FOV = 90.0f;
public:
    Camera();
    Camera(Vec3 pos);
    Camera(Vec3 pos, float pitch, float yaw);

    void ProcessMouseMovement(float xpos, float ypos);
    void ProcessMouseScroll(float yoffset);

    void SetFOV(float newFOV);
    float GetFOV() const;

    static const float MAX_FOV;
    static const float MIN_FOV;

    bool isFirstMouseInput = true;

    // camera options
    float mouseSensitivity = 0.1f;

    Vec3 position{ 0.0f, 0.0f, 0.0f };
    Vec3 front{};
    Vec3 up{};
    Vec3 right{};
    Vec3 worldUp{ 0.0f, 1.0f, 0.0f };

	float pitch = 0.0f;
	float yaw = -90.0f;

    Mat4 perspectiveMatrix = perspective(radians(FOV), aspect, near, far);
    Mat4 GetViewMatrix() const;

    Frustum GetFrustum() const;

    void UpdateCameraVectors();
};

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
