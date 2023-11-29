/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#ifndef APPLICATION_H
#define APPLICATION_H

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <math/Vec3.hpp>
#include <math/Vec4.hpp>
#include <math/VoxelRaycast.hpp>
#include <world/Player.hpp>
#include <opengl/MSAARenderer.hpp>
#include <world/World.hpp>
#include <opengl/Texture.hpp>
#include <opengl/Window.hpp>

namespace engine {
    class Application {
    private:
        std::unique_ptr<Window> m_Window;
        MSAARenderer* p_MSAARenderer = nullptr;
        World* m_World = nullptr;
        BlockInt m_SelectedBlock = GRASS;

        bool m_Wireframe = false;
        bool m_PlayingGame = false;

        float m_DeltaTime = 0.0f;
        float m_LastFrame = 0.0f;

        std::string MainMenu();
        void ProcessInput(Camera& camera);
    public:
        Application();

        void Run();
        void GLFWMouseMoveCallback(GLFWwindow* window, double xposIn, double yposIn);
        void GLFWScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
        void GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        void GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        void GLFWFramebufferResizeCallback(GLFWwindow* window, int width, int height);
        Application(const Application& arg) = delete; // Copy constructor
        Application(const Application&& arg) = delete;  // Move constructor
        Application& operator=(const Application& arg) = delete; // Assignment operator
        Application& operator=(const Application&& arg) = delete; // Move operator
    };
};
#endif // !APPLICATION_H

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