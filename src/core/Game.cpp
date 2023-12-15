/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include <core/Game.hpp>
#include <opengl/Shader.hpp>
#include <world/Skybox.hpp>
#include <math/Matrix.hpp>
#include <glad/gl.h>
#include <util/Log.hpp>
#include <stdexcept>

void Game::Run() {
    if (!gladLoadGL(static_cast<GLADloadfunc>(glfwGetProcAddress)))
    {
        throw std::runtime_error("Failed to load GLAD!");
    }

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    pMSAARenderer = std::make_unique<MSAARenderer>(SCREEN_WIDTH, SCREEN_HEIGHT);
    Shader framebufferShader("res/shaders/framebuffer.shader");

    pWorld = std::make_unique<World>();

    while (!mWindow.ShouldClose()) {
        float currentFrame = static_cast<float>(glfwGetTime());
        mDeltaTime = currentFrame - mLastFrame;
        mLastFrame = currentFrame;

        ProcessKeyInput();

        if (mIsWireFrame) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        pMSAARenderer->BindMSAAFBO();
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        pWorld->Draw();

        if (mIsWireFrame) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        glDisable(GL_DEPTH_TEST);
        pMSAARenderer->Draw(framebufferShader);

        glfwPollEvents();
        mWindow.SwapBuffers();

        pWorld->GenerateChunkRegions();
    }
}

void Game::ProcessKeyInput()
{
    if (mWindow.IsKeyPressed(GLFW_KEY_W)) {
        pWorld->mCamera.ProcessKeyboard(FORWARD, mDeltaTime);
    }
    if (mWindow.IsKeyPressed(GLFW_KEY_S)) {
        pWorld->mCamera.ProcessKeyboard(BACKWARD, mDeltaTime);
    }
    if (mWindow.IsKeyPressed(GLFW_KEY_A)) {
        pWorld->mCamera.ProcessKeyboard(LEFT, mDeltaTime);
    }
    if (mWindow.IsKeyPressed(GLFW_KEY_D)) {
        pWorld->mCamera.ProcessKeyboard(RIGHT, mDeltaTime);
    }
    if (mWindow.IsKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
        pWorld->mCamera.SetMovementSpeed(300.0f);
    }
    else {
        pWorld->mCamera.SetMovementSpeed(50.0f);
    }
}

void Game::GLFWMouseMoveCallback(GLFWwindow* window, float xposIn, float yposIn)
{
    pWorld->mCamera.ProcessMouseMovement(xposIn, yposIn);
}

void Game::GLFWScrollCallback(GLFWwindow* window, float xoffset, float yoffset)
{
    pWorld->mCamera.ProcessMouseScroll(yoffset);
}

void Game::GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    switch (key) {
    case GLFW_KEY_ESCAPE: {
        mWindow.SetShouldClose(GLFW_TRUE);
        break;
    }
    case GLFW_KEY_ENTER: {
        if (action == GLFW_PRESS) {
            mIsWireFrame = !mIsWireFrame;
        }
        break;
    }
    }
}

void Game::GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{

}

void Game::GLFWFramebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    pMSAARenderer.reset();
    pMSAARenderer = std::make_unique<MSAARenderer>(static_cast<GLsizei>(width), static_cast<GLsizei>(height));
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
