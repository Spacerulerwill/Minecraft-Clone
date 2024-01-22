/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>

class Game;

class Window {
private:
    GLFWwindow* pWindow = nullptr;
public:
    Window(Game* app, int width, int height, const char* title, bool initially_visible);
    ~Window();
    void Bind() const;
    void Unbind() const;
    void SetShouldClose(int value);
    bool ShouldClose() const;
    void SetHidden();
    void SetVisible();
    void SetMouseEnabled();
    void SetMouseDisabled();
    void SwapBuffers();
    bool IsKeyPressed(int key) const;
    GLFWwindow* GetWindow() const;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_move_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

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
