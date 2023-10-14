cmake_minimum_required(VERSION 3.22.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(MinecraftClone VERSION 1.0)

add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/gl.c
    src/core/Application.cpp 
    src/core/Application.hpp
    src/core/Shader.cpp 
    src/core/Shader.hpp
    src/core/Camera.cpp
    src/core/Camera.hpp
    src/math/Mat2.hpp
    src/math/Mat3.hpp
    src/math/Mat4.hpp
    src/math/Vec2.hpp
    src/math/Vec3.hpp
    src/math/Vec4.hpp
    src/math/Math.hpp
    src/math/VoxelRaycast.cpp
    src/math/VoxelRaycast.hpp
    src/opengl/BufferObject.hpp
    src/opengl/Texture.hpp
    src/opengl/Framebuffer.cpp
    src/opengl/Framebuffer.hpp
    src/opengl/VertexArray.cpp
    src/opengl/VertexArray.hpp
    src/opengl/VertexBufferLayout.cpp
    src/opengl/VertexBufferLayout.hpp
    src/world/Block.cpp
    src/world/Block.hpp
    src/world/Skybox.cpp
    src/world/Skybox.hpp
    src/world/Chunk.cpp
    src/world/Chunk.hpp
    src/world/World.cpp
    src/world/World.hpp
    src/world/Player.cpp
    src/world/Player.hpp
    src/world/ChunkMesher.cpp
    src/world/ChunkMesher.hpp
    src/util/Log.cpp
    src/util/Log.hpp
    src/util/Concepts.hpp
    src/util/Constants.hpp
    src/util/IO.hpp
)

add_subdirectory(submodules/glfw)
add_subdirectory(submodules/spdlog)
add_subdirectory(submodules/fmt)
add_subdirectory(submodules/yaml-cpp)

target_include_directories(${PROJECT_NAME} 
    PUBLIC submodules/glfw/include
    PUBLIC submodules/spdlog/include
    PUBLIC submodules/fmt/include
    PUBLIC submodules/yaml-cpp/include
    PUBLIC include
    PUBLIC src
)

target_link_directories(${PROJECT_NAME}
    PRIVATE submodules/glfw/src
    PRIVATE submodules/spdlog/src
    PRIVATE submodules/fmt/src
    PRIVATE submodules/yaml-cpp/src
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC spdlog
    PUBLIC glfw
    PUBLIC fmt
    PUBLIC yaml-cpp
)

# Copy resources folder to binary location
file(COPY res DESTINATION ${CMAKE_BINARY_DIR})
