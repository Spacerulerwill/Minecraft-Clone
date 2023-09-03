/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>

namespace engine {

    class Window
    {
    private:
        static GLFWwindow* m_Window;

    public:
        constexpr static int SCREEN_WIDTH = 1280;
        constexpr static int SCREEN_HEIGHT = 720;

        static void Init(const char* title);
        inline static void Terminate() { glfwDestroyWindow(m_Window); }
        inline static GLFWwindow*& GetWindow() { return m_Window; };
        Window(const Window& arg) = delete; // Copy constructor
        Window(const Window&& arg) = delete;  // Move constructor
        Window& operator=(const Window& arg) = delete; // Assignment operator
        Window& operator=(const Window&& arg) = delete; // Move operator
    };
};

#endif // !WINDOW_H

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