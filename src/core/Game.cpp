/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include <core/Game.hpp>
#include <core/SoundEngine.hpp>
#include <opengl/Shader.hpp>
#include <world/Block.hpp>
#include <math/Matrix.hpp>
#include <math/Raycast.hpp>
#include <math/Frustum.hpp>
#include <util/Log.hpp>
#include <util/Util.hpp>
#include <ui/Crosshair.hpp>

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <fmt/format.h>
#include <irrKlang/irrKlang.h>

#include <stdexcept>

void Game::Run(std::string worldDirectory) {
    mWindow.Bind();
    if (!gladLoadGL())
    {
        throw std::runtime_error("Failed to load GLAD!");
    }
    glViewport(0, 0, INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    InitBlocks();
    ImGUIContext imGuiContext = ImGUIContext(mWindow.GetWindow());
    pMSAARenderer = std::make_unique<MSAARenderer>(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT);
    Shader framebufferShader("shaders/framebuffer.shader");
    Crosshair crosshair(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, 32);
    pWorld = std::make_unique<World>(worldDirectory);    
    ScopedSound backgroundMusic("sound/music.mp3", true); 
    Mat4 ortho = orthographic(0.0f, INITIAL_WINDOW_HEIGHT, 0.0f, INITIAL_WINDOW_WIDTH, -1.0f, 100.0f);
    mWindow.SetVisible();
    while (!mWindow.ShouldClose()) {
        // World events
        pWorld->mPlayer.ApplyGravity(*pWorld, mDeltaTime);
        pWorld->GenerateChunks();
        pWorld->TrySwitchToNextTextureAtlas();

        // Delta time calculations
        double currentFrame = glfwGetTime();
        mDeltaTime = static_cast<float>(currentFrame - mLastFrame);
        mLastFrame = static_cast<float>(currentFrame);

        if (currentFrame - lastFpsSwitch > 0.2) {
            fps = static_cast<int>(1.0f / mDeltaTime);
            lastFpsSwitch = currentFrame;
        }
        
        // Input
        ProcessKeyInput();
        glfwPollEvents();

        // Set sound engine listener to players position
        SoundEngine::GetEngine()->setListenerPosition(
            irrklang::vec3df((pWorld->mPlayer.camera.position)),
            irrklang::vec3df((pWorld->mPlayer.camera.front))
        );

        // Draw World
        pMSAARenderer->BindMSAAFBO();    
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        int potentialDrawCalls = 0;
        int totalDrawCalls = 0;
        Frustum frustum = pWorld->mPlayer.camera.GetFrustum();
        if (mIsWireFrame)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else 
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        pWorld->Draw(frustum, &potentialDrawCalls, &totalDrawCalls);
        if (mIsWireFrame)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        pMSAARenderer->Draw(framebufferShader);

        // Draw UI
        crosshair.Draw(ortho);    
        imGuiContext.NewFrame();
        SettingsMenu(potentialDrawCalls, totalDrawCalls);
        imGuiContext.Render(); 
        mWindow.SwapBuffers();    
    }
}

void Game::SettingsMenu(int potentialDrawCalls, int totalDrawCalls) {
    ImGui::SetNextWindowSize(ImVec2{ 0,0 });
    ImGui::SetNextWindowPos(ImVec2{ 0,0 });
    ImGui::Begin("Settings");
    ImGui::SliderFloat("Sensitivity", &pWorld->mPlayer.camera.mouseSensitivity, 0.0f, 1.0f);
    ImGui::SliderFloat("Movement Speed", &pWorld->mPlayer.movementSpeed, 0.0f, 100.0f, "%.3f", ImGuiSliderFlags_NoInput);
    ImGui::SliderInt("Chunk load distance", &pWorld->mChunkLoadDistance, 3, 30, "%d", ImGuiSliderFlags_NoInput);
    ImGui::SliderInt("Chunk partial load distance", &pWorld->mChunkPartialLoadDistance, 3, 30, "%d", ImGuiSliderFlags_NoInput);
    ImGui::SliderInt("Max tasks per frame", &pWorld->mMaxTasksPerFrame, 1, 50, "%d", ImGuiSliderFlags_NoInput);

    iVec3 blockPos = GetWorldBlockPosFromGlobalPos(pWorld->mPlayer.camera.position);
    iVec3 chunkPos = GetChunkPosFromGlobalBlockPos(blockPos);
    iVec3 chunkLocalBlockPos = GetChunkBlockPosFromGlobalBlockPos(blockPos);

    ImGui::Text("Block coordinates: %s", std::string(blockPos).c_str());
    ImGui::Text("Chunk local block coordinates: %s", std::string(chunkLocalBlockPos).c_str());
    ImGui::Text("Chunk coordinates: %s", std::string(chunkPos).c_str());
    ImGui::Text("FPS: %d", fps);
    ImGui::Text("Potential draw calls: %d", potentialDrawCalls);
    ImGui::Text("Total draw calls: %d", totalDrawCalls);
    ImGui::Text("Time: %f", pWorld->mCurrentTime);
    ImGui::End();
}

void Game::ProcessKeyInput()
{
    if (!mIsMouseVisible) {
        pWorld->mPlayer.ProcessKeyInput(*pWorld, mWindow, mDeltaTime);
    }
}

void Game::GLFWMouseMoveCallback(GLFWwindow* window, float xposIn, float yposIn)
{
    UNUSED(window);

    if (!mIsMouseVisible) {
        pWorld->mPlayer.camera.ProcessMouseMovement(xposIn, yposIn);
    }
}

void Game::GLFWScrollCallback(GLFWwindow* window, float xoffset, float yoffset)
{
    UNUSED(window, xoffset);

    pWorld->mPlayer.camera.ProcessMouseScroll(yoffset);
}

void Game::GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    UNUSED(window);

    pWorld->mPlayer.KeyCallback(key, scancode, action, mods);
    switch (key) {
    case GLFW_KEY_ESCAPE: {
        mWindow.SetShouldClose(GLFW_TRUE);
        break;
    }
    case GLFW_KEY_Z: {
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
                pWorld->mPlayer.camera.isFirstMouseInput = true;
            }
        }
        break;
    }
    }
}

void Game::GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    UNUSED(window);
    
    if (!mIsMouseVisible) {
        pWorld->mPlayer.MouseCallback(*pWorld, button, action, mods);
    }
}

void Game::GLFWFramebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    UNUSED(window);

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
