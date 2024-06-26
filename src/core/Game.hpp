/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#ifndef GAME_H
#define GAME_H

#include <GLFW/glfw3.h>
#include <memory>
#include <opengl/Window.hpp>
#include <opengl/MSAARenderer.hpp>
#include <core/Camera.hpp>
#include <world/World.hpp>
#include <irrKlang/irrKlang.h>
#include <util/Log.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_internal.h>

constexpr int INITIAL_WINDOW_WIDTH = 1920;
constexpr int INITIAL_WINDOW_HEIGHT = 1080;

struct GLFWContext {
    GLFWContext() {
        LOG_INFO("Initialising GLFW");
        if (!glfwInit()) {
            throw std::runtime_error("Failed to initialise GLFW");
        }
    }

    ~GLFWContext() {
        LOG_INFO("Terminating GLFW");
        glfwTerminate();
    }
};

struct ImGUIContext {
    ImGUIContext(GLFWwindow* window) {
        LOG_INFO("Creating ImGUI context");
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        (void)io;
        io.IniFilename = NULL;
        ImGui::StyleColorsDark();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");
    }

    ImGUIContext(const ImGUIContext& arg) = delete;
    ImGUIContext(const ImGUIContext&& arg) = delete;
    ImGUIContext& operator=(const ImGUIContext& arg) = delete;
    ImGUIContext& operator=(const ImGUIContext&& arg) = delete;

    void NewFrame() const {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void Render() const {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    ~ImGUIContext() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        LOG_INFO("Destroying ImGUI context");
    }
};

class Game {
private:
    void ProcessKeyInput();
    void SettingsMenu(int potentialDrawCalls, int totalDrawCalls);
    GLFWContext glfwContext;
    Window mWindow = Window(this, INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT, "Craft++", false);
    std::unique_ptr<MSAARenderer> pMSAARenderer = nullptr;
    std::unique_ptr<World> pWorld = nullptr;

    bool mIsWireFrame = false;
    bool mIsMouseVisible = false;

    float mDeltaTime = 0.0f;
    float mLastFrame = 0.0f;

    int fps = 0;
    double lastFpsSwitch = 0.0f;
public:
    Game() = default;
    Game(const Game&) = delete;
    Game(Game&&) = delete;
    Game& operator=(const Game&) = delete;
    Game& operator=(const Game&&) = delete;
    void GLFWMouseMoveCallback(GLFWwindow* window, float xposIn, float yposIn);
    void GLFWScrollCallback(GLFWwindow* window, float xoffset, float yoffset);
    void GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    void GLFWFramebufferResizeCallback(GLFWwindow* window, int width, int height);
    void Run(std::string worldDirectory);
};

#endif // !GAME_H

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
