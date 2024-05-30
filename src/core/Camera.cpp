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

Camera::Camera(glm::vec3 pos) : position(pos) {
    UpdateCameraVectors();
}

Camera::Camera(glm::vec3 pos, float pitch, float yaw) : position(pos), yaw(yaw), pitch(pitch) {
    UpdateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix() const {
    return lookAt(position, position + front, up);
}

Frustum Camera::GetFrustum() const
{
    Frustum frustum{};
    const float halfVSide = far * tanf(radians(FOV * .5f));
    const float halfHSide = halfVSide * aspect;
    const glm::vec3 frontMultFar = far * front;

    frustum[static_cast<std::size_t>(FrustumPlane::NEAR)] = { position + near * front, front };
    frustum[static_cast<std::size_t>(FrustumPlane::FAR)] = { position + frontMultFar, front * -1.0f };
    frustum[static_cast<std::size_t>(FrustumPlane::RIGHT)] = { position, glm::normalize(glm::cross(frontMultFar - right * halfHSide, up)) };
    frustum[static_cast<std::size_t>(FrustumPlane::LEFT)] = { position, glm::normalize(glm::cross(up, frontMultFar + right * halfHSide))};
    frustum[static_cast<std::size_t>(FrustumPlane::TOP)] = { position, glm::normalize(glm::cross(right, frontMultFar - up * halfVSide)) };
    frustum[static_cast<std::size_t>(FrustumPlane::BOTTOM)] = { position, glm::normalize(glm::cross(frontMultFar + up * halfVSide, right))};
    return frustum;
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
    perspectiveMatrix = glm::perspective(radians(FOV), aspect, near, far);
}

float Camera::GetFOV() const
{
    return FOV;
}

void Camera::UpdateCameraVectors()
{
    // Calculate new front vector
    glm::vec3 newFront;
    newFront.x = static_cast<float>(cos(radians(yaw)) * cos(radians(pitch)));
    newFront.y = static_cast<float>(sin(radians(pitch)));
    newFront.z = static_cast<float>(sin(radians(yaw)) * cos(radians(pitch)));
    front = glm::normalize(newFront);

    // Calculate up and right 
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::normalize(glm::cross(right, front));
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
