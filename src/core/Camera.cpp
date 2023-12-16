/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include <algorithm>
#include <core/Camera.hpp>
#include <glad/gl.h>
#include <util/Log.hpp>

const float Camera::MAX_FOV = 90.0f;
const float Camera::MIN_FOV = 1.0f;

Camera::Camera() {
    UpdateCameraVectors();
}

Camera::Camera(Vec3 pos) : mPosition(pos) {
    UpdateCameraVectors();
}

Camera::Camera(Vec3 pos, float pitch, float yaw) : mPosition(pos), mYaw(yaw), mPitch(pitch) {
    UpdateCameraVectors();
}

Mat4 Camera::GetViewMatrix() const {
    return lookAt(mPosition, mPosition + mFront, mUp);
}

Mat4 Camera::GetPerspectiveMatrix() const {
    return mPerspectiveMatrix;
}

void Camera::SetFOV(float FOV) {
    if (mFOV != FOV) {
        FOV = std::clamp(FOV, MIN_FOV, MAX_FOV);
        mPerspectiveMatrix = perspective(radians(FOV), mAspect, mNear, mFar);
    }
    mFOV = FOV;
}

float Camera::GetFOV() const {
    return mFOV;
}

void Camera::SetPitch(float pitch) {
    assert(pitch > -180.0f && pitch < 180.0f);
    mPitch = pitch;
}

float Camera::GetPitch() const {
    return mPitch;
}

void Camera::SetYaw(float yaw) {
    assert(yaw > -180.0f && yaw < 180.0f);
    mYaw = yaw;
}

float Camera::GetYaw() const {
    return mYaw;
}

void Camera::SetMovementSpeed(float speed) {
    mMovementSpeed = speed;
}

Vec3 Camera::GetPosition() const {
    return mPosition;
}

Vec3 Camera::GetDirection() const {
    return mFront;
}

void Camera::ProcessKeyboard(CameraMovement direction, float deltaTime)
{
    float velocity = mMovementSpeed * deltaTime;
    Vec3 directionMultiplier;

    if (direction == FORWARD) {
        directionMultiplier = mFront;
    }
    if (direction == BACKWARD) {
        directionMultiplier = mFront * -1;
    }
    if (direction == LEFT) {
        directionMultiplier = mRight * -1;
    }
    if (direction == RIGHT) {
        directionMultiplier = mRight;
    }

    mPosition += directionMultiplier * velocity;
}

void Camera::ProcessMouseMovement(float xpos, float ypos)
{
    if (mIsFirstMouse) // initially set to true
    {
        mLastMouseX = xpos;
        mLastMouseY = ypos;
        mIsFirstMouse = false;
    }

    float xoffset = xpos - mLastMouseX;
    float yoffset = mLastMouseY - ypos; // reversed since y-coordinates go from bottom to top

    mLastMouseX = xpos;
    mLastMouseY = ypos;

    xoffset *= mMouseSensitivity;
    yoffset *= mMouseSensitivity;

    mPitch = std::clamp(mPitch + yoffset, -89.0f, 89.0f);
    mYaw = static_cast<float>(fmod((mYaw + xoffset), 360));

    UpdateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
    SetFOV(mFOV - (yoffset) * 2);
}

void Camera::UpdateCameraVectors()
{
    // Calculate new front vector
    Vec3 front;
    front[0] = static_cast<float>(cos(radians(mYaw)) * cos(radians(mPitch)));
    front[1] = static_cast<float>(sin(radians(mPitch)));
    front[2] = static_cast<float>(sin(radians(mYaw)) * cos(radians(mPitch)));
    mFront = front.normalized();

    // Calculate up and right 
    mRight = (mFront.cross(mWorldUp)).normalized();
    mUp = (mRight.cross(mFront)).normalized();
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