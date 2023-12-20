/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#ifndef PLAYER_H
#define PLAYER_H

#include <core/Camera.hpp>
#include <opengl/Window.hpp>

class World;

enum class PlayerMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

struct Player {
    float movementSpeed = 50.0f;
    Camera camera{ Vec3{0.0f, 400.0f, 0.0f} };
    void ProcessKeyInput(const World& world, const Window& window, float deltaTime);
    void ProcessMouseInput(const World& world, int button, int action, int mods);
    void Move(const World& world, PlayerMovement direction, float deltaTime);
};


#endif // !PLAYER_H

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