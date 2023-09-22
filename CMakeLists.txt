cmake_minimum_required(VERSION 3.22.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(MinecraftClone VERSION 1.0)

add_executable(${PROJECT_NAME} 
    src/main.cpp 
    src/gl.c
    src/core/Application.cpp
    src/core/Camera.cpp
    src/core/Shader.cpp
    src/core/Window.cpp
    src/math/Mat2.cpp
    src/math/Mat3.cpp
    src/math/Mat4.cpp
    src/math/Vec2.cpp
    src/math/Vec3.cpp
    src/math/Vec4.cpp
    src/math/VoxelRaycast.cpp
    src/opengl/Framebuffer.cpp
    src/opengl/IndexBuffer.cpp
    src/opengl/VertexArray.cpp
    src/opengl/VertexBuffer.cpp
    src/opengl/VertexBufferLayout.cpp
    src/world/Block.cpp
    src/world/Chunk.cpp
    src/world/ChunkMesher.cpp
    src/world/Skybox.cpp
    src/world/World.cpp
    src/util/Log.cpp
    src/util/TextureLoad.cpp
    src/core/Application.hpp
    src/core/Camera.hpp
    src/core/Shader.hpp
    src/core/Window.hpp
    src/math/Mat2.hpp
    src/math/Mat3.hpp
    src/math/Mat4.hpp
    src/math/Vec2.hpp
    src/math/Vec3.hpp
    src/math/Vec4.hpp
    src/math/VoxelRaycast.hpp
    src/opengl/Framebuffer.hpp
    src/opengl/IndexBuffer.hpp
    src/opengl/VertexArray.hpp
    src/opengl/VertexBuffer.hpp
    src/opengl/VertexBufferLayout.hpp
    src/world/Block.hpp
    src/world/Chunk.hpp
    src/world/ChunkMesher.hpp
    src/world/Skybox.hpp
    src/world/World.hpp
    src/util/Log.hpp
    src/util/TextureLoad.hpp
    src/math/Math.hpp
    src/util/Constants.hpp
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
