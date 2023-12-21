/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include <core/Game.hpp>
#include <opengl/Shader.hpp>
#include <world/Block.hpp>
#include <math/Matrix.hpp>
#include <math/Raycast.hpp>
#include <glad/glad.h>
#include <stdexcept>
#include <format>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_internal.h>
#include <GLFW/glfw3.h>

#include <util/Log.hpp>

#include <irrKlang/irrKlang.h>

void Game::Run() {
    if (!gladLoadGL())
    {
        throw std::runtime_error("Failed to load GLAD!");
    }

    // start the sound engine with default parameters
    irrklang::ISoundEngine* engine = irrklang::createIrrKlangDevice();

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.IniFilename = NULL;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(mWindow.GetWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 330");

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    InitBlocks();

    pMSAARenderer = std::make_unique<MSAARenderer>(SCREEN_WIDTH, SCREEN_HEIGHT);
    Shader framebufferShader("shaders/framebuffer.shader");

    pWorld = std::make_unique<World>();

    while (!mWindow.ShouldClose()) {
        float currentFrame = static_cast<float>(glfwGetTime());
        mDeltaTime = currentFrame - mLastFrame;
        mLastFrame = currentFrame;

        ProcessKeyInput();

        pWorld->player.ApplyGravity(*pWorld, mDeltaTime);

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

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowSize(ImVec2{ 0,0 });
        ImGui::SetNextWindowPos(ImVec2{ 0,0 });
        ImGui::Begin("Settings");
        ImGui::SliderFloat("Sensitivity", &pWorld->player.camera.mouseSensitivity, 0.0f, 1.0f);
        ImGui::SliderInt("Render Distance", &pWorld->mRenderDistance, 5, 30);
        ImGui::SliderInt("Chunk buffers per frame", &pWorld->mBufferPerFrame, 1, 50);

        Vec3 pos = pWorld->player.camera.position;
        iVec3 blockPos = GetWorldBlockPosFromGlobalPos(pos);
        iVec3 chunkPos = GetChunkPosFromGlobalBlockPos(blockPos);
        iVec3 chunkLocalBlockPos = GetChunkBlockPosFromGlobalBlockPos(blockPos);

        ImGui::Text(std::format("Block coordinates: {}", std::string(blockPos)).c_str());
        ImGui::Text(std::format("Chunk coordinates: {}", std::string(chunkPos)).c_str());

        ImGui::Text(std::format("FPS: {}", static_cast<int>(1.0f / mDeltaTime)).c_str());
        ImGui::End();

        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwPollEvents();
        mWindow.SwapBuffers();

        pWorld->GenerateChunks();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Game::ProcessKeyInput()
{
    if (!mIsMouseVisible) {
        pWorld->player.ProcessKeyInput(*pWorld, mWindow, mDeltaTime);
    }
}

void Game::GLFWMouseMoveCallback(GLFWwindow* window, float xposIn, float yposIn)
{
    if (!mIsMouseVisible) {
        pWorld->player.camera.ProcessMouseMovement(xposIn, yposIn);
    }
}

void Game::GLFWScrollCallback(GLFWwindow* window, float xoffset, float yoffset)
{
    pWorld->player.camera.ProcessMouseScroll(yoffset);
}

void Game::GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    pWorld->player.KeyCallback(key, scancode, action, mods);

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
    case GLFW_KEY_TAB: {
        if (action == GLFW_PRESS) {
            mIsMouseVisible = !mIsMouseVisible;
            if (mIsMouseVisible) {
                mWindow.SetMouseEnabled();
            }
            else {
                mWindow.SetMouseDisabled();
                pWorld->player.camera.isFirstMouseInput = true;
            }
        }
        break;
    }
    }
}

void Game::GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (!mIsMouseVisible) {
        pWorld->player.MouseCallback(*pWorld, button, action, mods);
    }
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
