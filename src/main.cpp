/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <GLFW/glfw3.h>
#include <core/Game.hpp>
#include <util/Log.hpp>
#include <core/SoundEngine.hpp>

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

int main() {
    Log::GetLogger();

    try {
        GLFWContext context;
        SoundEngine::GetEngine();
        Game game;
        game.Run();
    }
    catch (const std::runtime_error& e) {
        LOG_CRITICAL(e.what());
        return EXIT_FAILURE;
    }
    catch (const std::exception& e) {
        LOG_CRITICAL(e.what());
        return EXIT_FAILURE;
    }
    catch (...) {
        LOG_CRITICAL("Unhandled exception caught!");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
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