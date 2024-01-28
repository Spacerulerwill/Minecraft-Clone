/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include <opengl/Window.hpp>
#include <core/Game.hpp>
#include <stdexcept>

Window::Window(Game* app, int width, int height, const char* name, bool initiallyVisible) {
    // Create our window, and add its callbacks
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_VISIBLE, initiallyVisible ? GLFW_TRUE : GLFW_FALSE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    pWindow = glfwCreateWindow(width, height, name, NULL, NULL);

    if (pWindow == NULL)
    {
        throw std::runtime_error("Failed to create GLFW Window");
    }

    glfwSetWindowSizeCallback(pWindow, framebuffer_size_callback);
    glfwSetCursorPosCallback(pWindow, mouse_move_callback);
    glfwSetMouseButtonCallback(pWindow, mouse_button_callback);
    glfwSetScrollCallback(pWindow, scroll_callback);
    glfwSetKeyCallback(pWindow, key_callback);
    glfwSetWindowUserPointer(pWindow, reinterpret_cast<void*>(app));

    SetMouseDisabled();
}

Window::~Window() {
    glfwDestroyWindow(pWindow);
}

void Window::Bind() const {
    glfwMakeContextCurrent(pWindow);
}

void Window::Unbind() const {
    glfwMakeContextCurrent(NULL);
}

void Window::SetShouldClose(int value) {
    glfwSetWindowShouldClose(pWindow, value);
}

bool Window::ShouldClose() const {
    return glfwWindowShouldClose(pWindow);
}

void Window::SetHidden() {
    glfwHideWindow(pWindow);
}

void Window::SetVisible() {
    glfwShowWindow(pWindow);
}

void Window::SetMouseEnabled()
{
    glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Window::SetMouseDisabled()
{
    glfwSetInputMode(pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Window::SwapBuffers() {
    glfwSwapBuffers(pWindow);
}

GLFWwindow* Window::GetWindow() const {
    return pWindow;
}

bool Window::IsKeyPressed(int key) const {
    return glfwGetKey(pWindow, key) == GLFW_PRESS;
}

void mouse_move_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    Game* game = reinterpret_cast<Game*>(glfwGetWindowUserPointer(window));
    game->GLFWMouseMoveCallback(window, static_cast<float>(xposIn), static_cast<float>(yposIn));
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    Game* game = reinterpret_cast<Game*>(glfwGetWindowUserPointer(window));
    game->GLFWScrollCallback(window, static_cast<float>(xoffset), static_cast<float>(yoffset));
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Game* game = reinterpret_cast<Game*>(glfwGetWindowUserPointer(window));
    game->GLFWKeyCallback(window, key, scancode, action, mods);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    Game* game = reinterpret_cast<Game*>(glfwGetWindowUserPointer(window));
    game->GLFWMouseButtonCallback(window, button, action, mods);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    Game* game = reinterpret_cast<Game*>(glfwGetWindowUserPointer(window));
    game->GLFWFramebufferResizeCallback(window, width, height);
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
