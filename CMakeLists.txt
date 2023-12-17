cmake_minimum_required(VERSION 3.28.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(MinecraftClone VERSION 1.0)

add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/core/Game.cpp
    src/core/Game.hpp
    src/core/Camera.cpp
    src/core/Camera.hpp
    src/opengl/Window.cpp
    src/opengl/Window.hpp
    src/opengl/Texture.cpp
    src/opengl/Texture.hpp
    src/opengl/BufferObject.cpp
    src/opengl/BufferObject.hpp
    src/opengl/VertexArray.cpp
    src/opengl/VertexArray.hpp
    src/opengl/VertexBufferLayout.cpp
    src/opengl/VertexBufferLayout.hpp
    src/opengl/MSAARenderer.cpp
    src/opengl/MSAARenderer.hpp
    src/opengl/Shader.cpp
    src/opengl/Shader.hpp
    src/world/Skybox.cpp
    src/world/Skybox.hpp
    src/world/World.cpp
    src/world/World.hpp
    src/world/chunk/Chunk.cpp
    src/world/chunk/Chunk.hpp
    src/world/chunk/ChunkMesher.cpp
    src/world/chunk/ChunkMesher.hpp
    src/world/chunk/ChunkStack.cpp
    src/world/chunk/ChunkStack.hpp
    src/world/chunk/Constants.hpp
    src/world/Block.cpp
    src/world/Block.hpp
    src/math/Vector.cpp
    src/math/Vector.hpp
    src/math/Matrix.cpp
    src/math/Matrix.hpp
    src/math/Math.cpp
    src/math/Math.hpp
    src/util/Log.cpp
    src/util/Log.hpp
    src/util/Concepts.hpp
    lib/gl.c
)

add_subdirectory(submodules/glfw)
add_subdirectory(submodules/spdlog)

target_include_directories(${PROJECT_NAME} 
    SYSTEM PRIVATE submodules/glfw/include
    SYSTEM PRIVATE submodules/spdlog/include
    SYSTEM PRIVATE include
    PUBLIC src
)

target_link_directories(${PROJECT_NAME}
    PRIVATE submodules/glfw/src
    PRIVATE submodules/spdlog/src
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC spdlog
    PUBLIC glfw
)

add_compile_definitions(GLFW_INCLUDE_NONE)

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /wd4996 /external:W0)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
