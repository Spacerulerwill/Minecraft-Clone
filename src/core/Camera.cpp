/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include <algorithm>
#include <core/Camera.hpp>
#include <glad/glad.h>
#include <assert.h>
#include <world/World.hpp>

const float Camera::MAX_FOV = 135.0f;
const float Camera::MIN_FOV = 1.0f;

Camera::Camera() {
    UpdateCameraVectors();
}

Camera::Camera(Vec3 pos) : position(pos) {
    UpdateCameraVectors();
}

Camera::Camera(Vec3 pos, float pitch, float yaw) : position(pos), yaw(yaw), pitch(pitch) {
    UpdateCameraVectors();
}

Mat4 Camera::GetViewMatrix() const {
    return lookAt(position, position + front, up);
}

void Camera::ProcessMouseMovement(float xpos, float ypos)
{
    if (isFirstMouseInput) // initially set to true
    {
        lastMouseX = xpos;
        lastMouseY = ypos;
        isFirstMouseInput = false;
    }

    float xoffset = xpos - lastMouseX;
    float yoffset = lastMouseY - ypos; // reversed since y-coordinates go from bottom to top

    lastMouseX = xpos;
    lastMouseY = ypos;

    xoffset *= mouseSensitivity;
    yoffset *= mouseSensitivity;

    pitch = std::clamp(pitch + yoffset, -89.0f, 89.0f);
    yaw = static_cast<float>(fmod((yaw + xoffset), 360));

    UpdateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
    SetFOV(FOV - (yoffset) * 2);
}

void Camera::SetFOV(float newFOV)
{
    FOV = std::clamp(newFOV, MIN_FOV, MAX_FOV);
    perspectiveMatrix = perspective(radians(FOV), aspect, near, far);
}

float Camera::GetFOV() const
{
    return FOV;
}

void Camera::UpdateCameraVectors()
{
    // Calculate new front vector
    Vec3 newFront;
    newFront[0] = static_cast<float>(cos(radians(yaw)) * cos(radians(pitch)));
    newFront[1] = static_cast<float>(sin(radians(pitch)));
    newFront[2] = static_cast<float>(sin(radians(yaw)) * cos(radians(pitch)));
    front = newFront.normalized();

    // Calculate up and right 
    right = (front.cross(worldUp)).normalized();
    up = (right.cross(front)).normalized();
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