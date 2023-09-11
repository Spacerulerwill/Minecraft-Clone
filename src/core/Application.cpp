/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include <core/Application.hpp>
#include <util/TextureLoad.hpp>
#include <world/Skybox.hpp>
#include <core/Camera.hpp>
#include <core/Shader.hpp>
#include <util/Log.hpp>
#include <math/Math.hpp>
#include <opengl/Framebuffer.hpp>
#include <opengl/VertexArray.hpp>
#include <opengl/VertexBuffer.hpp>
#include <opengl/VertexBufferLayout.hpp>
#include <math.h>
#include <random>
#include <chrono>
#include <limits>

using namespace std::chrono;

std::unique_ptr<engine::Application> engine::Application::s_Instance = nullptr;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_move_callback(GLFWwindow* window, double xposIn, double yposIn);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

engine::Application::Application()
{

}

void engine::Application::Init()
{
    if (Application::s_Instance == nullptr) {
        Application::s_Instance = std::unique_ptr<Application>(new Application);
    }
}

void engine::Application::Run()
{
    srand(time(NULL));
    // Utilities setup
	Log::Init();

	if (!glfwInit()) {
		throw std::runtime_error("Failed to intialise GLFW");
	}

    // Create GLFW Window and setup callbacks for input
	Window::Init("Voxel Game");
    glfwSetWindowSizeCallback(Window::GetWindow(), framebuffer_size_callback);
    glfwSetCursorPosCallback(Window::GetWindow(), mouse_move_callback);
    glfwSetMouseButtonCallback(Window::GetWindow(), mouse_button_callback);
    glfwSetScrollCallback(Window::GetWindow(), scroll_callback);
    glfwSetKeyCallback(Window::GetWindow(), key_callback);

    // OpenGL function pointers
	if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress))
	{
		throw std::runtime_error("Failed to intialise GLAD");
	}

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    // Create a framebuffer, postprocess shader and quad to render to so we can use Render-To-Texture
    float quadVerts[24] = {
        -1.0f, -1.0f, 0.0f, 0.0f, // bottom left
        1.0f, -1.0f, 1.0f, 0.0f, // bottom right
        1.0f, 1.0f, 1.0f, 1.0f, // top right
        -1.0f, -1.0f, 0.0f, 0.0f, // bottom left
        1.0f, 1.0f, 1.0f, 1.0f, // top right
        -1.0f, 1.0f, 0.0f, 1.0f // top left
    };

    VertexBuffer VBO;
    VBO.BufferData((const void*)quadVerts, sizeof(quadVerts));
    VertexBufferLayout bufferLayout;
    bufferLayout.AddAttribute<float>(2); // 2 Vertex Coordinates
    bufferLayout.AddAttribute<float>(2); // 2 Texture Coordinates
    VertexArray VAO;
    VAO.AddBuffer(VBO, bufferLayout);

    Shader postProcessShader = Shader("res/shaders/postprocess.shader");

    glActiveTexture(GL_TEXTURE0);
    p_Framebuffer = new Framebuffer(Window::SCREEN_WIDTH, Window::SCREEN_HEIGHT);

    // Initialise Block data from .yml files and create a demo chunk
    BlockHandler::InitBlocks();

    Shader chunkShader("res/shaders/chunk.shader");
    Shader waterShader("res/shaders/water.shader");

    const siv::PerlinNoise::seed_type seed = time(NULL);
    siv::PerlinNoise perlin {seed};
        
    for (int x = 0; x < CHUNKS_X; x++){
        for (int z = 0; z < CHUNKS_Y; z++){
            Chunk* chunk = new Chunk(x, 0, z);
            chunk->TerrainGen(perlin);
            chunk->CreateMesh();
            chunk->BufferData();
            m_Chunks[x * CHUNKS_X + z] = chunk;
        }
    }

    // Our skybox and texture atlas use texture unit 0, in different texture types so this is fine
    glActiveTexture(GL_TEXTURE0);
    Skybox skybox;
    unsigned int textureAtlas = loadTextureArray("res/textures/atlas.png", 16);

    // UI elements
    glActiveTexture(GL_TEXTURE1);
    Shader crosshairShader("res/shaders/crosshair.shader");
    unsigned int crosshair = loadTexture("res/textures/ui/crosshair.png");

    int crosshair_size = 32;
    float bottom_corner_x = (Window::SCREEN_WIDTH / 2.0f) - crosshair_size / 2.0f;
    float bottom_corner_y = (Window::SCREEN_HEIGHT / 2.0f) - crosshair_size / 2.0f;

    float crosshairVerts[24] = {
        bottom_corner_x, bottom_corner_y, 0.0f, 0.0f, // bottom left
        bottom_corner_x + crosshair_size, bottom_corner_y, 1.0f, 0.0f, // bottom right
        bottom_corner_x + crosshair_size,  bottom_corner_y + crosshair_size, 1.0f, 1.0f, // top right
        bottom_corner_x, bottom_corner_y, 0.0f, 0.0f, // bottom left
        bottom_corner_x + crosshair_size,  bottom_corner_y + crosshair_size, 1.0f, 1.0f, // top right
        bottom_corner_x, bottom_corner_y + crosshair_size, 0.0f, 1.0f,  // top left
    };

    Mat4 ortho = orthographic(0.0f, Window::SCREEN_HEIGHT, 0.0f, Window::SCREEN_WIDTH, -1.0f, 100.0f);

    VertexBuffer crosshairVBO;
    crosshairVBO.BufferData((const void*)crosshairVerts, sizeof(crosshairVerts));
    VertexBufferLayout bufferLayout2;
    bufferLayout2.AddAttribute<float>(4);
    VertexArray crosshairVAO;
    crosshairVAO.AddBuffer(crosshairVBO, bufferLayout2);

    Vec3 lightDir(1.0f, 0.0f, 0.0f);

    // Main game loop
	while (!glfwWindowShouldClose(Window::GetWindow())) {

        float currentFrame = static_cast<float>(glfwGetTime());
        m_DeltaTime = currentFrame - m_LastFrame;
        m_LastFrame = currentFrame;

        ProcessInput(m_DeltaTime);
        
        // Draw scene to framebuffer
        p_Framebuffer->Bind();

        if (m_Wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glEnable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Mat4 perspective_matrix = perspective(radians(m_Camera.m_FOV), static_cast<float>(Window::SCREEN_WIDTH) / Window::SCREEN_HEIGHT, 0.1f, 100.0f);
        Mat4 view_matrix = m_Camera.GetViewMatrix();

        lightDir.x  = cos(radians(1)) * lightDir.x - sin(radians(1)) * lightDir.y;
        lightDir.y = sin(radians(1)) * lightDir.x + cos(radians(1)) * lightDir.y;

        // Draw the skybox
        glDepthMask(GL_FALSE);
        Mat4 skybox_view_matrix = engine::translationRemoved(view_matrix);

        glActiveTexture(GL_TEXTURE0);
        skybox.Draw(perspective_matrix, skybox_view_matrix);

        glDepthMask(GL_TRUE);
        // Draw the chunks
        chunkShader.Bind();
        chunkShader.setVec3("lightDir", lightDir);
        chunkShader.setMat4("projection", perspective_matrix);
        chunkShader.setMat4("view", view_matrix);
        chunkShader.SetInt("tex_array", 0);
     
        glActiveTexture(GL_TEXTURE0);
        for (int i = 0; i < CHUNKS_X * CHUNKS_Y; i++){
            m_Chunks[i]->Draw(chunkShader);
        }

        waterShader.Bind();
        waterShader.setMat4("projection", perspective_matrix);
        waterShader.setMat4("view", view_matrix);
        waterShader.SetInt("tex_array", 0);
        for (int i = 0; i < CHUNKS_X * CHUNKS_Y; i++){
            m_Chunks[i]->DrawWater(waterShader);
        }        

        // Render UI
        crosshairShader.Bind();
        crosshairVAO.Bind();
        crosshairShader.SetInt("screenTexture", 0);
        crosshairShader.SetInt("crosshair", 1);
        crosshairShader.setMat4("projection", ortho);

        glActiveTexture(GL_TEXTURE1);
        glDrawArrays(GL_TRIANGLES, 0, 6);


        // Unbind framebuffer and render framebuffer texture to quad
        p_Framebuffer->Unbind();

        if(m_Wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        postProcessShader.Bind();
        VAO.Bind();
        glDisable(GL_DEPTH_TEST);
        glActiveTexture(GL_TEXTURE0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwPollEvents();
		glfwSwapBuffers(Window::GetWindow());
	}
	ResourceCleanup();
}

std::unique_ptr<engine::Application>& engine::Application::GetInstance()
{
    return s_Instance;
}

void engine::Application::GLFWFramebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    //m_OrthographicMatrix = orthographic(0.0f, height, 0.0f, width, -1.0f, 100.0f);
    glActiveTexture(GL_TEXTURE0);
    delete p_Framebuffer;
    p_Framebuffer = new Framebuffer(width, height);
}

void engine::Application::ResourceCleanup()
{
    delete p_Framebuffer;
    for (int i =0; i < CHUNKS_X * CHUNKS_Y; i++){
        delete m_Chunks[i];
    }
	Window::Terminate();
	glfwTerminate();
}

void engine::Application::ProcessInput(float deltaTime)
{
    GLFWwindow* window = Window::GetWindow();
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        m_Camera.ProcessKeyboard(FORWARD, deltaTime, m_Chunk);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_Camera.ProcessKeyboard(BACKWARD, deltaTime, m_Chunk);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        m_Camera.ProcessKeyboard(LEFT, deltaTime, m_Chunk);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        m_Camera.ProcessKeyboard(RIGHT, deltaTime, m_Chunk);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
        m_Camera.m_MovementSpeed = 20.0f;
    else
        m_Camera.m_MovementSpeed = 2.5f;
}

void engine::Application::GLFWMouseMoveCallback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) // initially set to true
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    m_Camera.ProcessMouseMovement(xoffset, yoffset, true);
}

void engine::Application::GLFWScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    m_Camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

void engine::Application::GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    
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

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_move_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    engine::Application::GetInstance()->GLFWMouseMoveCallback(window, xposIn, yposIn);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    engine::Application::GetInstance()->GLFWScrollCallback(window, xoffset, yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    engine::Application::GetInstance()->GLFWKeyCallback(window, key, scancode, action, mods);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    engine::Application::GetInstance()->GLFWMouseButtonCallback(window, button, action, mods);
}

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