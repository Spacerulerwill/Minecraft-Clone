/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#ifndef APPLICATION_H
#define APPLICATION_H

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <math/Vec3.hpp>
#include <core/Camera.hpp>
#include <core/Window.hpp>
#include <world/Chunk.hpp>
#include <world/Block.hpp>
#include <opengl/Framebuffer.hpp>
#include <memory>
#include <unordered_map>

namespace engine {
	class Application {
	private:
		Application();
		static std::unique_ptr<Application> s_Instance;

		void ResourceCleanup();
		void ProcessInput(float deltaTime);
		float m_DeltaTime = 0.0f;
		float m_LastFrame = 0.0f;
		Camera m_Camera;
		BlockInt m_SelectedBlock = GRASS;

		bool m_Wireframe = false;
		bool firstMouse = true;
		float lastX = 0.0f;
		float lastY = 0.0f;

		Framebuffer* p_Framebuffer = nullptr;
	public:
		static void Init();
		void Run();
		static std::unique_ptr<Application>& GetInstance();
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