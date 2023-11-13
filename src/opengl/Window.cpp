/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include <opengl/Window.hpp>
#include <core/Application.hpp>
#include <stdexcept>


engine::Window::Window(Application* app, unsigned int width, unsigned int height, const char* name) {
     // Create our window, and add its callbacks
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	p_Window = glfwCreateWindow(width, height, name, NULL, NULL);

	if (p_Window == NULL)
	{
		glfwTerminate();
		throw std::runtime_error("Failed to create GLFW Window");
	}

    glfwSetWindowSizeCallback(p_Window, framebuffer_size_callback);
    glfwSetCursorPosCallback(p_Window, mouse_move_callback);
    glfwSetMouseButtonCallback(p_Window, mouse_button_callback);
    glfwSetScrollCallback(p_Window, scroll_callback);
    glfwSetKeyCallback(p_Window, key_callback);
    glfwSetWindowUserPointer(p_Window, reinterpret_cast<void*>(app));
	glfwMakeContextCurrent(p_Window);

    // Set input mode to cursor disabled so use can't move mouse out of window
    glfwSetInputMode(p_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

engine::Window::~Window() {
    glfwDestroyWindow(p_Window);
    glfwTerminate();
}

void engine::Window::Bind() const {
    glfwMakeContextCurrent(p_Window);
}

void engine::Window::Unbind() const {
    glfwMakeContextCurrent(NULL);
}

void engine::Window::SetShouldClose(int value) {
    glfwSetWindowShouldClose(p_Window, value);
}

bool engine::Window::ShouldClose() const {
    return glfwWindowShouldClose(p_Window);
}

void engine::Window::SetHidden() {
    glfwHideWindow(p_Window);
}

void engine::Window::SetVisible() {
    glfwShowWindow(p_Window);
}

void engine::Window::SwapBuffers() {
    glfwSwapBuffers(p_Window);
}

GLFWwindow* engine::Window::GetWindow() const {
    return p_Window;
}

bool engine::Window::IsKeyPressed(int key) const {
    return glfwGetKey(p_Window, key) == GLFW_PRESS;
}

void engine::mouse_move_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    Application* app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    app->GLFWMouseMoveCallback(window, xposIn, yposIn);
}

void engine::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    Application* app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    app->GLFWScrollCallback(window, xoffset, yoffset);
}

void engine::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Application* app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    app->GLFWKeyCallback(window, key, scancode, action, mods);
}

void engine::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    Application* app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    app->GLFWMouseButtonCallback(window, button, action, mods);
}

void engine::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    Application* app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    app->GLFWFramebufferResizeCallback(window, width, height);
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