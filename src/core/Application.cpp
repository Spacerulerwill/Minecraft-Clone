/*
Copyright (C) 2023 William Redding - All Rights Reserved
License: MIT
*/

#include <util/Log.hpp>
#include <glad/gl.h>
#include <core/Application.hpp>
#include <core/Shader.hpp>
#include <opengl/Texture.hpp>
#include <opengl/BufferObject.hpp>
#include <opengl/VertexArray.hpp>
#include <opengl/VertexBufferLayout.hpp>
#include <world/Block.hpp>
#include <world/chunk/Chunk.hpp>
#include <world/World.hpp>
#include <util/IO.hpp>
#include <util/Exceptions.hpp>
#include <math/Math.hpp>
#include <algorithm>
#include <filesystem>
#include <memory>
#include <chrono>

// free floating callback functions
namespace engine {
    void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    void mouse_move_callback(GLFWwindow* window, double xposIn, double yposIn);
    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

    constexpr GLsizei SCREEN_WIDTH = 1280;
    constexpr GLsizei SCREEN_HEIGHT = 720;
}

std::string engine::Application::MainMenu() {
    std::string option;

    while ((option != "l" && option != "c") || (option == "l" && NumberOfFoldersInDirectory("worlds") == 0)) {
        std::cout << "Select option:\n";
        std::cout << "C - Create world\n";
        if (NumberOfFoldersInDirectory("worlds") != 0) {
            std::cout << "L - Load world\n";
            std::cout << "D - Delete world\n";
        }
        std::cout << "E - Exit\n";
        std::cin >> option;
        std::transform(option.begin(), option.end(), option.begin(), ::tolower);

        if (option == "e") {
            m_Window->SetShouldClose(GLFW_TRUE);
            return "";
        }

        if (option == "d") {
            if (NumberOfFoldersInDirectory("worlds") == 0) {
                continue;
            }
            std::string worldTodelete;
            std::cin.ignore();
            std::cout << "Enter world name: ";
            std::getline(std::cin, worldTodelete);
            uintmax_t numFilesDeleted = std::filesystem::remove_all(fmt::format("worlds/{}", worldTodelete));
            if (numFilesDeleted == 0) {
                LOG_ERROR(fmt::format("No world named {} found!", worldTodelete));
            } else {
                LOG_ERROR(fmt::format("Deleted world {}", worldTodelete));
            }
            continue;
        }
    }
    std::cin.ignore();

    std::string worldName;
    if (option == "l") {
        // Get the names of all the worlds available
        std::vector<std::string> worldOptions;
        for(auto& p : std::filesystem::directory_iterator("worlds"))
            if (p.is_directory())
                worldOptions.push_back(p.path().filename());
        std::cout << "Select world: \n";
        for(std::string& worldOption : worldOptions) {
            std::cout << "* " << worldOption << "\n";
        }
        do {
            std::getline(std::cin, worldName);
        } while (!std::filesystem::is_directory(fmt::format("worlds/{}", worldName)));
    } 

    if (option == "c") {
        while (true) {
            std::cout << "Enter world name: ";
            std::getline(std::cin, worldName);

            if (worldName == ""){
                LOG_ERROR("World name cannot be empty!");
                continue;
            }

            bool worldCreated = std::filesystem::create_directory(fmt::format("worlds/{}", worldName));

            if (worldCreated) {
                break;
            } else {
                LOG_ERROR("World already exists!");
            }
        }

        std::filesystem::create_directory(fmt::format("worlds/{}/regions", worldName));

        // Get world seed by hashing the seed string the user provided, or generating a random one if its an empty seed
        const size_t MAXIMUM_CHARS = 32;
        static char buffer[MAXIMUM_CHARS + 1]; // An extra for the terminating nul character.
        std::cout << "Enter seed (Leave blank for random): ";
        std::cin.getline(buffer, MAXIMUM_CHARS, '\n');
        const std::string seedString(buffer);
        siv::PerlinNoise::seed_type seed;
        if (seedString == "") {
            seed = rand() % std::numeric_limits<siv::PerlinNoise::seed_type>().max();
        } else {
            std::hash<std::string> hasher;
            siv::PerlinNoise::seed_type seed = static_cast<siv::PerlinNoise::seed_type>(hasher(seedString));
        }

        // Create world data file with seed and last played
        engine::WorldSave worldSave {
            .seed = seed,
        };
    
        engine::WriteStructToDisk(fmt::format("worlds/{}/world.dat", worldName), worldSave);

        // Create player save data with their starting position and rotation
        engine::PlayerSave playerSave {
            .position = engine::Vec3<float>(0.0f, 1000.0f, 0.0f),
            .pitch = 0.0f,
            .yaw = -90.0f
        };
        
        // Write player save data to file
        engine::WriteStructToDisk(fmt::format("worlds/{}/player.dat", worldName), playerSave);
    }
    return worldName;
}

engine::Application::Application() {
    
}

void engine::Application::Run()
{    
    srand(time(NULL));
    std::filesystem::create_directory("worlds");

     // Try and intialise GLFW
    if (!glfwInit()) {
        glfwTerminate();
		throw std::runtime_error("Failed to intialise GLFW");
	}

    // Create our window
    m_Window = std::make_unique<Window>(this, SCREEN_WIDTH, SCREEN_HEIGHT, "Voxel Engine");
    m_Window->Bind();

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
    glEnable(GL_POLYGON_SMOOTH);
    glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    InitBlocks();

    // Load texture atlases into an array so we can cycle through them
    glActiveTexture(GL_TEXTURE0);
    std::unique_ptr<TexArray2D> atlases[MAX_ANIMATION_FRAMES];
    for (int i = 0; i < MAX_ANIMATION_FRAMES; i++) {
        atlases[i] = std::make_unique<TexArray2D>(fmt::format("res/textures/atlases/atlas{}.png", i).c_str(), TEXTURE_SIZE);
    }

    // Create framebuffer object and quad
    p_MSAARenderer = new MSAARenderer(SCREEN_WIDTH, SCREEN_HEIGHT);
    
    // Orthographic projection matrix maps normalised device coordinates to pixel screen space coordinates
    Mat4 ortho = orthographic(0.0f, SCREEN_HEIGHT, 0.0f, SCREEN_WIDTH, -1.0f, 100.0f);
    
    // Load crosshair texture
    glActiveTexture(GL_TEXTURE1);
    Tex2D crosshairTexture("res/textures/ui/crosshair.png"); 

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

    VertexBuffer crosshairVBO;
    crosshairVBO.BufferData((const void*)crosshairVerts, sizeof(crosshairVerts), GL_STATIC_DRAW);
    VertexBufferLayout bufferLayout2;
    bufferLayout2.AddAttribute<float>(4);
    VertexArray crosshairVAO;
    crosshairVAO.AddBuffer(crosshairVBO, bufferLayout2);  

    // Load grass mask texture
    glActiveTexture(GL_TEXTURE2);
    Tex2D grassMaskTexture("res/textures/block/mask/grass_side_mask.png");

    // Load shaders
    Shader framebufferShader("res/shaders/framebuffer.shader");
    Shader crosshairShader("res/shaders/crosshair.shader");
    Shader chunkShader = Shader("res/shaders/chunk.shader");
    Shader waterShader = Shader("res/shaders/water.shader");
    Shader customModelShader = Shader("res/shaders/custom_model.shader");

    double lastTime = 0.0f;
    int currentAtlasIndex = 0;
    
    while (!m_Window->ShouldClose()) {
        std::string worldName = MainMenu();

        if (worldName == "") {
            break;
        }

        try {
            m_World = new World(worldName.c_str());
        } catch (WorldCorruptException& e) {
            LOG_ERROR(e.what());
            continue;
        }

        Player& player = m_World->GetPlayer();
        Camera& camera = player.GetCamera();

        Vec3<int> prevChunkPos = player.GetChunkPosition();
        Vec3<int> chunkPos = player.GetChunkPosition();

        m_Window->SetVisible();
        m_PlayingGame = true;
        
        // Main game loop
        while (m_PlayingGame) { 
            // Calculate delta time
            float currentFrame = static_cast<float>(glfwGetTime());
            m_DeltaTime = currentFrame - m_LastFrame;
            m_LastFrame = currentFrame;

            ProcessInput(camera);

            prevChunkPos = chunkPos;
            chunkPos = player.GetChunkPosition();

            // Cycle through texture atlases 10 times a second
            double currentTime = glfwGetTime();
            if (currentTime - lastTime > 0.1) {
                currentAtlasIndex = (currentAtlasIndex + 1) % MAX_ANIMATION_FRAMES;
                glActiveTexture(GL_TEXTURE0);
                atlases[currentAtlasIndex]->Bind();
                lastTime = currentTime;
            }

            player.BlockRaycast(m_World);

            if (m_Wireframe)
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            else
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            // Bind our framebuffer and render to it
            p_MSAARenderer->BindMSAAFBO();

            glEnable(GL_DEPTH_TEST);
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            m_World->Draw(chunkShader, waterShader, customModelShader);

            if(m_Wireframe)
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            
            /* 
            Unbind our framebuffer, binding the default framebuffer and render the result texture to a quad
            */
            glDisable(GL_DEPTH_TEST);
            p_MSAARenderer->Draw(framebufferShader);

            // Render UI last
            crosshairShader.Bind();
            crosshairVAO.Bind();
            crosshairShader.SetInt("screenTexture", 0);
            crosshairShader.SetInt("crosshair", 1);
            crosshairShader.SetMat4("projection", ortho);

            glDrawArrays(GL_TRIANGLES, 0, 6);

            m_World->GenerateChunks();

            glfwPollEvents();
            m_Window->SwapBuffers();
        }
        delete m_World;
    }
    delete p_MSAARenderer;
    spdlog::shutdown();
}

void engine::Application::GLFWFramebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	// On resize, we adjust glViewport and regenerate our framebuffer at the new resolution
    glViewport(0, 0, width, height);
	glActiveTexture(GL_TEXTURE0);
    delete p_MSAARenderer;
    p_MSAARenderer = new MSAARenderer(width, height);
}

void engine::Application::ProcessInput(Camera& camera)
{
    if (glfwGetKey(m_Window->GetWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        m_PlayingGame = false;
        m_Window->SetHidden();
    }
    if (m_Window->IsKeyPressed(GLFW_KEY_W))
        camera.ProcessKeyboard(FORWARD, m_DeltaTime);
    if (m_Window->IsKeyPressed(GLFW_KEY_S))
        camera.ProcessKeyboard(BACKWARD, m_DeltaTime);
    if (m_Window->IsKeyPressed(GLFW_KEY_A))
        camera.ProcessKeyboard(LEFT, m_DeltaTime);
    if (m_Window->IsKeyPressed(GLFW_KEY_D))
        camera.ProcessKeyboard(RIGHT, m_DeltaTime);
    if (m_Window->IsKeyPressed(GLFW_KEY_LEFT_SHIFT))
        camera.SetMovementSpeed(300.0f);
    else
        camera.SetMovementSpeed(10.0f);
}

void engine::Application::GLFWMouseMoveCallback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    m_World->GetPlayer().GetCamera().ProcessMouseMovement(xpos, ypos);
}

void engine::Application::GLFWScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    m_World->GetPlayer().GetCamera().ProcessMouseScroll(static_cast<float>(yoffset));
}

void engine::Application::GLFWMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    switch (button) {
        case GLFW_MOUSE_BUTTON_LEFT: {
            if (action == GLFW_PRESS) {
                const VoxelRaycastResult& raycastResult = m_World->GetPlayer().GetBlockRaycastResult();
                Chunk* chunk = raycastResult.chunk;
                if (chunk != nullptr) {
                    chunk->SetBlock(AIR, raycastResult.blockPos);
                    chunk->CreateMesh();
                    chunk->BufferData();
                }
            }
            break;
        }
        case GLFW_MOUSE_BUTTON_RIGHT: {
            if (action == GLFW_PRESS) {
                const VoxelRaycastResult& raycastResult = m_World->GetPlayer().GetBlockRaycastResult();
                Chunk* chunk = raycastResult.chunk;
                if (chunk != nullptr && raycastResult.blockHit != AIR) {
                    BlockInt blockToPlaceOn = chunk->GetBlock(raycastResult.blockPos);
                    if (blockToPlaceOn != AIR) {
                        Vec3<int> blockPlacePosition = raycastResult.blockPos + raycastResult.normal;

                        // chunk edge cases - if we place and the block will end up in another chunk
                        if (blockPlacePosition.x == CS_P_MINUS_ONE) {
                            return;
                        }

                        else if (blockPlacePosition.x == 0) {
                            return;
                        }

                        if (blockPlacePosition.y == CS_P_MINUS_ONE) {
                            return;
                        }

                        if (blockPlacePosition.y == 0) {
                            return;
                        }

                        if (blockPlacePosition.z == CS_P_MINUS_ONE) {
                            return;
                        }   

                        else if (blockPlacePosition.z == 0) {
                            return;
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
            const VoxelRaycastResult& raycastResult = m_World->GetPlayer().GetBlockRaycastResult();
            if (raycastResult.blockHit != AIR) {
                m_SelectedBlock = raycastResult.blockHit;
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