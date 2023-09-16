/*
Copyright (C) 2023 William Redding - All Rights Reserved
LICENSE: MIT
*/

#include <core/Camera.hpp>
#include <util/Log.hpp>
#include <GLFW/glfw3.h>
#include <core/Window.hpp>

engine::Camera::Camera()
{
    UpdateCameraVectors();
}

engine::Mat4 engine::Camera::GetViewMatrix() const
{
    return engine::lookAt(m_Position, m_Position + m_Front, m_Up);
}

void engine::Camera::ProcessKeyboard(CameraMovement direction, float deltaTime, Chunk* chunk)
{
    float velocity = m_MovementSpeed * deltaTime;
    Vec3 directionMultiplier;
       
    if (direction == FORWARD)
        directionMultiplier = m_Front;
    if (direction == BACKWARD)
        directionMultiplier = m_Front * -1;
    if (direction == LEFT)
        directionMultiplier = m_Right * -1;
    if (direction == RIGHT)
        directionMultiplier = m_Right;

    m_Position += directionMultiplier * velocity;
}

void engine::Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    xoffset *= m_MouseSensitivity;
    yoffset *= m_MouseSensitivity;

    m_Yaw += xoffset;
    m_Pitch += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (constrainPitch)
    {
        if (m_Pitch > 89.0f)
            m_Pitch = 89.0f;
        if (m_Pitch < -89.0f)
            m_Pitch = -89.0f;
    }

    // update Front, Right and Up Vectors using the updated Euler angles
    UpdateCameraVectors();
}

void engine::Camera::ProcessMouseScroll(float yoffset)
{
    m_FOV -= yoffset;
    if (m_FOV < 1.0f)
        m_FOV = 1.0f;
    if (m_FOV > 90.0f)
        m_FOV = 90.0f;
}

void engine::Camera::UpdateCameraVectors()
{
    // Calculate the new front vector
    engine::Vec3 front;
    front.x = cos(radians(m_Yaw)) * cos(radians(m_Pitch));
    front.y = sin(radians(m_Pitch));
    front.z = sin(radians(m_Yaw)) * cos(radians(m_Pitch));
    m_Front = Vec3::normalised(front);

    // also recalculate right and up vector
    m_Right = Vec3::normalised(Vec3::cross(m_Front, m_WorldUp));
    m_Up = Vec3::normalised(Vec3::cross(m_Right, m_Front));
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