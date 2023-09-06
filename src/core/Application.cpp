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
    
    m_Chunk = new Chunk(0, 0, 0);
    auto start = high_resolution_clock::now();
    for (int i = 0; i < 1000; i++)
        m_Chunk->CreateMesh();
    auto stop = high_resolution_clock::now();
    int microseconds_taken = duration_cast<microseconds>(stop - start).count();
    LOG_TRACE(microseconds_taken / 1000.0f);
    m_Chunk->BufferData();

    Shader chunkShader("res/shaders/chunk.shader");

    // Our skybox and texture atlas use texture unit 0, in different texture types so this is fine
    glActiveTexture(GL_TEXTURE0);
    Skybox skybox;
    unsigned int textureAtlas = loadTextureArray("res/textures/atlas.png", 16);

    // UI elements
    glActiveTexture(GL_TEXTURE1);
    Shader crosshairShader("res/shaders/crosshair.shader");
    unsigned int crosshair = loadTexture("res/textures/ui/crosshair.png");

    int crosshair_size = 64;
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

        // Draw the chunks
        glDepthMask(GL_TRUE);
        chunkShader.Bind();
        chunkShader.setMat4("projection", perspective_matrix);
        chunkShader.setMat4("view", view_matrix);
        chunkShader.SetInt("tex_array", 0);
        BlockRaycastResult result = BlockRaycast(m_Camera.m_Position, m_Camera.m_Front, 3.0f);
        chunkShader.SetInt("drawBlockHighlight", result.blockHit != AIR);
        if (result.blockHit != AIR) {
            chunkShader.setIVec3(
                "blockPos",
                static_cast<int>(result.location.x),
                static_cast<int>(result.location.y),
                static_cast<int>(result.location.z)
            );
        } 
     
        glActiveTexture(GL_TEXTURE0);
        m_Chunk->Draw(chunkShader);

        // Draw the skybox
        glDepthMask(GL_FALSE);
        Mat4 skybox_view_matrix = engine::translationRemoved(view_matrix);

        glActiveTexture(GL_TEXTURE0);
        skybox.Draw(perspective_matrix, skybox_view_matrix);

        // Render UI
        crosshairShader.Bind();
        crosshairVAO.Bind();
        crosshairShader.SetInt("screenTexture", 0);
        crosshairShader.SetInt("crosshair", 1);
        crosshairShader.setMat4("projection", ortho);

        glActiveTexture(GL_TEXTURE1);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Unbind framebuffer and render framebuffer texture to quad
        glDepthMask(GL_TRUE);
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

engine::BlockRaycastResult engine::Application::BlockRaycast(const Vec3& start, const Vec3& direction, float distance) const
{
    float tMaxX, tMaxY, tMaxZ, tDeltaX, tDeltaY, tDeltaZ;
    int int_voxel_x, int_voxel_y, int_voxel_z;

    Vec3 _start = start * INV_BLOCK_SCALE;
    Vec3 end = (start + (direction * distance)) * INV_BLOCK_SCALE;

    int dx = sign(end.x - _start.x);
    if (dx != 0) tDeltaX = fmin(dx / (end.x - _start.x), 16777215.0f); else tDeltaX = 16777215.0f;
    if (dx > 0) tMaxX = tDeltaX * fract1(_start.x); else tMaxX = tDeltaX * fract(_start.x);

    int dy = sign(end.y - _start.y);
    if (dy != 0) tDeltaY = fmin(dy / (end.y - _start.y), 16777215.0f); else tDeltaY = 16777215.0f;
    if (dy > 0) tMaxY = tDeltaY * fract1(_start.y); else tMaxY = tDeltaY * fract(_start.y);

    int dz = sign(end.z - _start.z);
    if (dz != 0) tDeltaZ = fmin(dz / (end.z - _start.z), 16777215.0f); else tDeltaZ = 16777215.0f;
    if (dz > 0) tMaxZ = tDeltaZ * fract1(_start.z); else tMaxZ = tDeltaZ * fract(_start.z);


    Vec3 voxel(_start.x, _start.y, _start.z);

    int_voxel_x = static_cast<int>(voxel.x);
    int_voxel_y = static_cast<int>(voxel.y);
    int_voxel_z = static_cast<int>(voxel.z);

    // first voxel player is in
    if (!(int_voxel_x < 0 || int_voxel_x > CHUNK_SIZE_MINUS_ONE || int_voxel_y < 0 || int_voxel_y > CHUNK_SIZE_MINUS_ONE || int_voxel_z < 0 || int_voxel_z > CHUNK_SIZE_MINUS_ONE)) {

        BlockInt block = m_Chunk->GetBlock(int_voxel_x, int_voxel_y, int_voxel_z);
        if (block != AIR) {
            return {
                0,
                block,
                voxel
            };
        }
    }

    int face_entered;
    while (true) {

        if (tMaxX < tMaxY) {
            if (tMaxX < tMaxZ) {
                voxel.x += dx;
                face_entered = 2 * -dx;
                tMaxX += tDeltaX;
            }
            else {
                voxel.z += dz;
                face_entered = 3 * -dz;
                tMaxZ += tDeltaZ;
            }
        }
        else {
            if (tMaxY < tMaxZ) {
                voxel.y += dy;
                face_entered = 1 * -dy;
                tMaxY += tDeltaY;
            }
            else {
                voxel.z += dz;
                face_entered = 3 * -dz;
                tMaxZ += tDeltaZ;
            }
        }
        if (tMaxX >= 1 && tMaxY >= 1 && tMaxZ >= 1) {
            return {
                face_entered,
                AIR,
                voxel
            };
        };

        int_voxel_x = static_cast<int>(voxel.x);
        int_voxel_y = static_cast<int>(voxel.y);
        int_voxel_z = static_cast<int>(voxel.z);

        if (!(int_voxel_x < 0 || int_voxel_x > CHUNK_SIZE_MINUS_ONE || int_voxel_y < 0 || int_voxel_y > CHUNK_SIZE_MINUS_ONE || int_voxel_z < 0 || int_voxel_z > CHUNK_SIZE_MINUS_ONE)) {

            BlockInt block = m_Chunk->GetBlock(int_voxel_x, int_voxel_y, int_voxel_z);
            if (block != AIR) {
                return {
                    face_entered,
                    block,
                    voxel
                };
            }
        }
    }
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
    if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS) {
        BlockRaycastResult result = BlockRaycast(m_Camera.m_Position, m_Camera.m_Front, 3);

        if (result.blockHit == AIR)
            return;

        m_Chunk->SetBlock(AIR, static_cast<int>(result.location.x), static_cast<int>(result.location.y), static_cast<int>(result.location.z));

        BlockDataStruct data = BlockHandler::BlockData[result.blockHit];
        MaterialDataStruct material = BlockHandler::MaterialData[data.material];
        MaterialInt randomIndex = rand() % material.breakSounds.size();

        m_Chunk->CreateMesh();
        m_Chunk->BufferData();
    }

    if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_PRESS) {
        BlockRaycastResult hit = BlockRaycast(m_Camera.m_Position, m_Camera.m_Front, 3);

        if (hit.blockHit == AIR)
            return;

        int voxelX = static_cast<int>(hit.location.x);
        int voxelY = static_cast<int>(hit.location.y);
        int voxelZ = static_cast<int>(hit.location.z);

        BlockDataStruct data = BlockHandler::BlockData[m_SelectedBlock];
        MaterialDataStruct material = BlockHandler::MaterialData[data.material];
        MaterialInt randomIndex = rand() % material.placeSounds.size();

        switch (hit.faceEntered) {
        case 1: {
            m_Chunk->SetBlock(m_SelectedBlock, voxelX, voxelY + 1, voxelZ);
            break;
        }
        case -1: {
            m_Chunk->SetBlock(m_SelectedBlock, voxelX, voxelY - 1, voxelZ);
            break;
        }
        case 2: {
            m_Chunk->SetBlock(m_SelectedBlock, voxelX+1, voxelY, voxelZ);
            break;
        }
        case -2: {
            m_Chunk->SetBlock(m_SelectedBlock, voxelX-1, voxelY, voxelZ);
            break;
        }
        case 3: {
            m_Chunk->SetBlock(m_SelectedBlock, voxelX, voxelY, voxelZ+1);
            break;
        }
        case -3: {
            m_Chunk->SetBlock(m_SelectedBlock, voxelX, voxelY, voxelZ-1);
            break;
        }
        }

        m_Chunk->CreateMesh();
        m_Chunk->BufferData();
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