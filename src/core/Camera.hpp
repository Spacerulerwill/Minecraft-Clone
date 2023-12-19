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

class World;

enum CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

class Camera {
public:
    Camera();
    Camera(Vec3 pos);
    Camera(Vec3 pos, float pitch, float yaw);

    Mat4 GetViewMatrix() const;
    Mat4 GetPerspectiveMatrix() const;

    void SetFOV(float FOV);
    float GetFOV() const;

    void SetPitch(float pitch);
    float GetPitch() const;

    void SetYaw(float yaw);
    float GetYaw() const;

    void SetMovementSpeed(float speed);

    Vec3 GetPosition() const;
    Vec3 GetDirection() const;

    void ProcessKeyboard(World* world, CameraMovement direction, float deltaTime);
    void ProcessMouseMovement(float xpos, float ypos);
    void ProcessMouseScroll(float yoffset);

    static const float MAX_FOV;
    static const float MIN_FOV;

    float mMouseSensitivity = 0.1f;
    bool mIsFirstMouse = true;
private:
    float mLastMouseX = 0.0f;
    float mLastMouseY = 0.0f;

    Vec3 mPosition{ 0.0f, 0.0f, 0.0f };
    Vec3 mFront;
    Vec3 mUp;
    Vec3 mRight;
    Vec3 mWorldUp{ 0.0f, 1.0f, 0.0f };

    // camera options
    float mMovementSpeed = 2.5f;
    float mAspect = 16.0f / 9.0f;
    float mNear = 0.1f;
    float mFOV = 45.0f;
    float mFar = 5000.0f;
    float mPitch = 0.0f;
    float mYaw = -90.0f;

    // Perspective Matrix
    Mat4 mPerspectiveMatrix = perspective(radians(mFOV), mAspect, mNear, mFar);

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