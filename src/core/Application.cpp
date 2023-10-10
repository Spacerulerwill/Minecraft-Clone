/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include <glad/gl.h>
#include <util/Log.hpp>
#include <core/Application.hpp>
#include <core/Shader.hpp>
#include <opengl/Texture.hpp>
#include <opengl/BufferObject.hpp>
#include <opengl/VertexArray.hpp>
#include <opengl/VertexBufferLayout.hpp>
#include <world/Block.hpp>
#include <world/Skybox.hpp>
#include <world/Chunk.hpp>
#include <world/World.hpp>
#include <math/Math.hpp>
#include <algorithm>
#include <filesystem>
#include <memory>
#include <chrono>
#include <PerlinNoise.hpp>
#include <yaml-cpp/yaml.h>
#include <fstream>

using namespace std::chrono;

// Unique pointer to the singleton instance
std::unique_ptr<engine::Application> engine::Application::s_Instance = nullptr;

// free floating callback functions
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_move_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

constexpr GLsizei SCREEN_WIDTH = 1280;
constexpr GLsizei SCREEN_HEIGHT = 720;

engine::Application::Application()
{

}

// Used to initailise the singleton - will have any effect once
void engine::Application::Init()
{
    if (Application::s_Instance == nullptr) {
        Application::s_Instance = std::unique_ptr<Application>(new Application);
    }
}

void engine::Application::Run(const char* worldName)
{    
    // Try and intialise GLFW
    if (!glfwInit()) {
        glfwTerminate();
		throw std::runtime_error("Failed to intialise GLFW");
	}

    // Create our window, and add its callbacks
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Voxel Engine", NULL, NULL);

	if (window == NULL)
	{
		glfwTerminate();
		throw std::runtime_error("Failed to create GLFW Window");
	}

    p_Window = window;

    glfwSetWindowSizeCallback(p_Window, framebuffer_size_callback);
    glfwSetCursorPosCallback(p_Window, mouse_move_callback);
    glfwSetMouseButtonCallback(p_Window, mouse_button_callback);
    glfwSetScrollCallback(p_Window, scroll_callback);
    glfwSetKeyCallback(p_Window, key_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwMakeContextCurrent(window);

    // Load OpenGL function pointers
    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
	{
		throw std::runtime_error("Failed to intialise GLAD");
	}

    /*
	OpenGl setting configuration. We are enabling blend for alpha transparency blending and using the default blend function
	We also enable culling faces as an optimisation using the default winding order.
	*/
    glEnable(GL_MULTISAMPLE);  
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // Load our block data - this must happen before any other part of the game starts
    InitBlocks();

    /*
    Create an anonymous scope to ensure that all OpenGL objects created are deleted and cleaned up
    before calling glfwTerminate()
    */

    // Load texture atlases
    glActiveTexture(GL_TEXTURE0);
    
    std::unique_ptr<Texture<GL_TEXTURE_2D_ARRAY>> atlases[MAX_ANIMATION_FRAMES];

    for (int i = 0; i < MAX_ANIMATION_FRAMES; i++) {
        atlases[i] = std::make_unique<Texture<GL_TEXTURE_2D_ARRAY>>(fmt::format("res/textures/atlases/atlas{}.png", i).c_str(), TEXTURE_SIZE);
    }

    // Create skybox 
    Skybox skybox;

    // Create framebuffer object and quad
    p_Framebuffer = new Framebuffer(SCREEN_WIDTH, SCREEN_HEIGHT);

    float quadVerts[24] = {
        -1.0f, -1.0f, 0.0f, 0.0f, // bottom left
        1.0f, -1.0f, 1.0f, 0.0f, // bottom right
        1.0f, 1.0f, 1.0f, 1.0f, // top right
        -1.0f, -1.0f, 0.0f, 0.0f, // bottom left
        1.0f, 1.0f, 1.0f, 1.0f, // top right
        -1.0f, 1.0f, 0.0f, 1.0f // top left
    };
    
    BufferObject<GL_ARRAY_BUFFER> VBO;
    VBO.BufferData((const void*)quadVerts, sizeof(quadVerts), GL_STATIC_DRAW);
    VertexBufferLayout bufferLayout;
    bufferLayout.AddAttribute<float>(2); // 2 Vertex Coordinates
    bufferLayout.AddAttribute<float>(2); // 2 Texture Coordinates
    VertexArray VAO;
    VAO.AddBuffer(VBO, bufferLayout);

    // Orthographic projection matrix maps normalised device coordinates to pixel screen space coordinates
	Mat4 ortho = orthographic(0.0f, SCREEN_HEIGHT, 0.0f, SCREEN_WIDTH, -1.0f, 100.0f);
    // Load crosshair texture
    glActiveTexture(GL_TEXTURE1);
    Texture<GL_TEXTURE_2D> crosshairTexture("res/textures/ui/crosshair.png"); 

    // Create vertices, VBO and VAO for crosshair
    int crosshair_size = 32;
    float bottom_corner_x = (SCREEN_WIDTH / 2.0f) - crosshair_size / 2.0f;
    float bottom_corner_y = (SCREEN_HEIGHT / 2.0f) - crosshair_size / 2.0f;

    float crosshairVerts[24] = {
        bottom_corner_x, bottom_corner_y, 0.0f, 0.0f, // bottom left
        bottom_corner_x + crosshair_size, bottom_corner_y, 1.0f, 0.0f, // bottom right
        bottom_corner_x + crosshair_size,  bottom_corner_y + crosshair_size, 1.0f, 1.0f, // top right
        bottom_corner_x, bottom_corner_y, 0.0f, 0.0f, // bottom left
        bottom_corner_x + crosshair_size,  bottom_corner_y + crosshair_size, 1.0f, 1.0f, // top right
        bottom_corner_x, bottom_corner_y + crosshair_size, 0.0f, 1.0f,  // top left
    };

    BufferObject<GL_ARRAY_BUFFER> crosshairVBO;
    crosshairVBO.BufferData((const void*)crosshairVerts, sizeof(crosshairVerts), GL_STATIC_DRAW);
    VertexBufferLayout bufferLayout2;
    bufferLayout2.AddAttribute<float>(4);
    VertexArray crosshairVAO;
    crosshairVAO.AddBuffer(crosshairVBO, bufferLayout2);  

    // Load grass mask texture
    glActiveTexture(GL_TEXTURE2);
    Texture<GL_TEXTURE_2D> grassMaskTexture("res/textures/block/mask/grass_side_mask.png");

    // Load shaders
    Shader chunkShader("res/shaders/chunk.shader");
    Shader waterShader("res/shaders/water.shader");
    Shader crosshairShader("res/shaders/crosshair.shader");
    Shader customModelShader("res/shaders/custom_model.shader");
    Shader framebufferShader("res/shaders/framebuffer.shader");

    double lastTime = 0.0f;
    int currentAtlasIndex = 0;
    int totalFrameDuration = 0;
    
    YAML::Node playerYaml = YAML::LoadFile(fmt::format("worlds/{}/player.yml", worldName));
    Vec3 playerPosition = playerYaml["position"].as<Vec3<float>>();
    float playerYaw = playerYaml["yaw"].as<float>();
    float playerPitch = playerYaml["pitch"].as<float>();

    m_Camera = new Camera(playerPosition, playerPitch, playerYaw);
    m_World = new World(worldName);
    const int radius = 5;

    // Main game loop
    while (!glfwWindowShouldClose(p_Window)) { 
        auto start = high_resolution_clock::now();
        // Calculate delta time
        float currentFrame = static_cast<float>(glfwGetTime());
        m_DeltaTime = currentFrame - m_LastFrame;
        m_LastFrame = currentFrame;

        ProcessInput();

        // Calculate player chunk position
        int chunkX = m_Camera->GetPosition().x / CS;
        if (m_Camera->GetPosition().x < 0) chunkX -= 1;

        int chunkY = m_Camera->GetPosition().y / CS;
        if (m_Camera->GetPosition().y < 0) chunkY -= 1;

        int chunkZ = m_Camera->GetPosition().z / CS;
        if (m_Camera->GetPosition().z < 0) chunkZ -= 1;

        // Cycle through texture atlases 10 times a second
        double currentTime = glfwGetTime();
        if (currentTime - lastTime > 0.1) {
            currentAtlasIndex = (currentAtlasIndex + 1) % MAX_ANIMATION_FRAMES;
            glActiveTexture(GL_TEXTURE0);
            atlases[currentAtlasIndex]->Bind();
            lastTime = currentTime;
        }

        // Create chunks every frame
        m_World->CreateChunks(chunkX, chunkY, chunkZ, radius, 12);

        // Raycast outwards to find a block to highlight
        Vec3<int> raycastBlockPos = Vec3<int>(0);
        m_BlockSelectRaycastResult = VoxelRaycast(m_World, m_Camera->GetPosition(), m_Camera->GetDirection(), 15.0f);
        Chunk* chunk = m_BlockSelectRaycastResult.chunk;

        if (chunk != nullptr) {
            raycastBlockPos = Vec3<int>(
                chunk->m_Pos.x * CS + m_BlockSelectRaycastResult.blockPos.x - 1, 
                chunk->m_Pos.y * CS + m_BlockSelectRaycastResult.blockPos.y - 1,
                chunk->m_Pos.z * CS + m_BlockSelectRaycastResult.blockPos.z - 1
            );
        } else {
            raycastBlockPos = Vec3<int>(0,0,0);
        }

        // Bind our framebuffer and render to it
        p_Framebuffer->Bind();

        if (m_Wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glEnable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        Mat4<float> perspective = m_Camera->GetPerspectiveMatrix();
        Mat4<float> view = m_Camera->GetViewMatrix();

        chunkShader.Bind();
        chunkShader.setMat4("projection", perspective);
        chunkShader.setMat4("view", view);
        chunkShader.SetInt("tex_array", 0);
        chunkShader.SetInt("grass_mask", 2);
        chunkShader.SetInt("drawBlockHighlight", m_BlockSelectRaycastResult.blockHit != AIR);
        if (m_BlockSelectRaycastResult.blockHit != AIR  && chunk != nullptr)
            chunkShader.setIVec3("blockPos", raycastBlockPos);
        m_World->DrawBlocks(chunkShader);

        glDisable(GL_CULL_FACE);
        customModelShader.Bind();
        customModelShader.setMat4("projection", perspective);
        customModelShader.setMat4("view", view);
        customModelShader.SetInt("tex_array", 0);
        chunkShader.SetInt("drawBlockHighlight", m_BlockSelectRaycastResult.blockHit != AIR);
        if (m_BlockSelectRaycastResult.blockHit != AIR  && chunk != nullptr)
            chunkShader.setIVec3("blockPos", raycastBlockPos);
        m_World->DrawCustomModelBlocks(customModelShader);
        glEnable(GL_CULL_FACE);

        glDepthFunc(GL_LEQUAL);
        skybox.Draw(perspective, translationRemoved(view));
        glDepthFunc(GL_LESS);

        waterShader.Bind();
        waterShader.setMat4("projection", perspective);
        waterShader.setMat4("view", view);
        waterShader.SetInt("tex_array", 0);
        m_World->DrawWater(waterShader);
        
        /* 
        Unbind our framebuffer, binding the default framebuffer and render the result texture to a quad
        */
        p_Framebuffer->Unbind();

        if(m_Wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        framebufferShader.Bind();
        VAO.Bind();
        glDisable(GL_DEPTH_TEST);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Render UI last
		crosshairShader.Bind();
		crosshairVAO.Bind();
		crosshairShader.SetInt("screenTexture", 0);
		crosshairShader.SetInt("crosshair", 1);
		crosshairShader.setMat4<float>("projection", ortho);

		glDrawArrays(GL_TRIANGLES, 0, 6);

        auto end = high_resolution_clock::now();
        totalFrameDuration =  duration_cast<microseconds>(end - start).count();

        glfwPollEvents();
        glfwSwapBuffers(p_Window);
    }
    delete m_World;

     // save player position into player.yaml
    std::string playerYamlLocation = fmt::format("worlds/{}/player.yml", worldName);
    YAML::Node worldYaml = YAML::LoadFile(playerYamlLocation);
    worldYaml["position"] = m_Camera->GetPosition();
    worldYaml["yaw"] = m_Camera->GetYaw();
    worldYaml["pitch"] = m_Camera->GetPitch();
    std::ofstream fout(playerYamlLocation); 
    fout << worldYaml;
    fout.close();

    delete m_Camera;
    delete p_Framebuffer;
    glfwDestroyWindow(p_Window);
    spdlog::shutdown();
}
void engine::Application::GLFWFramebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	// On resize, we adjust glViewport and regenerate our framebuffer at the new resolution
    glViewport(0, 0, width, height);
	glActiveTexture(GL_TEXTURE0);
    delete p_Framebuffer;
    p_Framebuffer = new Framebuffer(width, height);
}

void engine::Application::ProcessInput()
{
    if (glfwGetKey(p_Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(p_Window, true);
    if (glfwGetKey(p_Window, GLFW_KEY_W) == GLFW_PRESS)
        m_Camera->ProcessKeyboard(FORWARD, m_DeltaTime);
    if (glfwGetKey(p_Window, GLFW_KEY_S) == GLFW_PRESS)
        m_Camera->ProcessKeyboard(BACKWARD, m_DeltaTime);
    if (glfwGetKey(p_Window, GLFW_KEY_A) == GLFW_PRESS)
        m_Camera->ProcessKeyboard(LEFT, m_DeltaTime);
    if (glfwGetKey(p_Window, GLFW_KEY_D) == GLFW_PRESS)
        m_Camera->ProcessKeyboard(RIGHT, m_DeltaTime);
    if (glfwGetKey(p_Window, GLFW_KEY_LEFT_SHIFT))
        m_Camera->SetMovementSpeed(100.0f);
    else
        m_Camera->SetMovementSpeed(2.5f);
}

void engine::Application::GLFWMouseMoveCallback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    m_Camera->ProcessMouseMovement(xpos, ypos);
}

void engine::Application::GLFWScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    m_Camera->ProcessMouseScroll(static_cast<float>(yoffset));
}

void engine::Application::GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT: {
            if (action == GLFW_PRESS) {
                Chunk* chunk = m_BlockSelectRaycastResult.chunk;
                if (chunk != nullptr) {
                    chunk->SetBlock(AIR, m_BlockSelectRaycastResult.blockPos);
                    chunk->CreateMesh();
                    chunk->BufferData();
                }
            }
            break;
        }
        case GLFW_MOUSE_BUTTON_RIGHT: {
            if (action == GLFW_PRESS) {
                Chunk* chunk = m_BlockSelectRaycastResult.chunk;
                if (chunk != nullptr && m_BlockSelectRaycastResult.blockHit != AIR) {
                    BlockInt blockToPlaceOn = chunk->GetBlock(m_BlockSelectRaycastResult.blockPos);
                    if (blockToPlaceOn != AIR) {
                        Vec3<int> blockPlacePosition = m_BlockSelectRaycastResult.blockPos + m_BlockSelectRaycastResult.normal;

                        // if making a new vertical chunk we can exit early
                        if (blockPlacePosition.y == CS_P_MINUS_ONE) {
                            blockPlacePosition.y = 1;
                            Chunk* newChunk = m_World->CreateChunk((chunk->m_Pos + Vec3<int>(0,1,0)));
                            newChunk->SetBlock(m_SelectedBlock, blockPlacePosition);
                            newChunk->UnloadToFile(m_World->m_WorldName);
                            newChunk->CreateMesh();
                            newChunk->BufferData();
                            m_World->m_ChunkDrawVector.push_back(newChunk);
                            return;
                        }

                        // chunk edge cases 
                        if (blockPlacePosition.x == CS_P_MINUS_ONE) {
                            blockPlacePosition.x = 1;
                            chunk = m_World->GetChunk(chunk->m_Pos.x + 1, chunk->m_Pos.y, chunk->m_Pos.z);
                        }

                        if (blockPlacePosition.x == 0) {
                            blockPlacePosition.x = CS;
                            chunk = m_World->GetChunk(chunk->m_Pos.x - 1, chunk->m_Pos.y, chunk->m_Pos.z);
                        }

                        if (blockPlacePosition.z == CS_P_MINUS_ONE) {
                            blockPlacePosition.z = 1;
                            chunk = m_World->GetChunk(chunk->m_Pos.x, chunk->m_Pos.y, chunk->m_Pos.z + 1);
                        }

                        if (blockPlacePosition.z == 0) {
                            blockPlacePosition.z = CS;
                            chunk = m_World->GetChunk(chunk->m_Pos.x, chunk->m_Pos.y, chunk->m_Pos.z - 1);
                        }

                        BlockInt blockAtPlacePosition = chunk->GetBlock(blockPlacePosition);
                        if (blockAtPlacePosition == AIR) {
                            chunk->SetBlock(m_SelectedBlock, blockPlacePosition);
                            chunk->CreateMesh();
                            chunk->BufferData();
                        }
                    }
                }
            }
            break;
        }
        case GLFW_MOUSE_BUTTON_MIDDLE: {
            if (m_BlockSelectRaycastResult.blockHit != AIR) {
                m_SelectedBlock = m_BlockSelectRaycastResult.blockHit;
            }
            break;
        }
    }
}
       
void engine::Application::GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
        m_Wireframe = !m_Wireframe;
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS) {
        if (m_SelectedBlock == NUM_BLOCKS-1) {
            m_SelectedBlock = 1;
        }
        else {
            m_SelectedBlock++;
        }
    }
}

std::unique_ptr<engine::Application>& engine::Application::GetInstance()
{
    return s_Instance;
}

// input callback that wraps the application singleton mouse callback function
void mouse_move_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    engine::Application::GetInstance()->GLFWMouseMoveCallback(window, xposIn, yposIn);
}

// input callback that wraps the application singleton scroll callback function
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    engine::Application::GetInstance()->GLFWScrollCallback(window, xoffset, yoffset);
}

// input callback that wraps the application singleton key callback function
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    engine::Application::GetInstance()->GLFWKeyCallback(window, key, scancode, action, mods);
}

// input callback that wraps the application singleton mouse button callback function
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    engine::Application::GetInstance()->GLFWMouseButtonCallback(window, button, action, mods);
}

// screen resize callback that wraps the application framebuffer resize callback
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    engine::Application::GetInstance()->GLFWFramebufferResizeCallback(window, width, height);
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