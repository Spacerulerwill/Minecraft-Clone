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
    src/core/SoundEngine.cpp
    src/core/SoundEngine.hpp
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
    src/world/Player.cpp
    src/world/Player.hpp
    src/math/Vector.cpp
    src/math/Vector.hpp
    src/math/Matrix.cpp
    src/math/Matrix.hpp
    src/math/Math.cpp
    src/math/Math.hpp
    src/math/Raycast.cpp
    src/math/Raycast.hpp 
    src/math/AABB.cpp
    src/math/AABB.hpp
    src/util/Log.cpp
    src/util/Log.hpp
    src/util/Concepts.hpp
    lib/glad.c
    lib/imgui/imgui_impl_glfw.cpp
    lib/imgui/imgui_impl_opengl3.cpp
    lib/imgui/imgui.cpp
    lib/imgui/imgui_demo.cpp
    lib/imgui/imgui_draw.cpp
    lib/imgui/imgui_tables.cpp
    lib/imgui/imgui_widgets.cpp
)

add_subdirectory(submodules/glfw)
add_subdirectory(submodules/spdlog)
add_subdirectory(submodules/yaml-cpp)

target_include_directories(${PROJECT_NAME} 
    SYSTEM PRIVATE submodules/glfw/include
    SYSTEM PRIVATE submodules/spdlog/include
    SYSTEM PRIVATE submodules/yaml-cpp/include
    SYSTEM PRIVATE include/imgui
    SYSTEM PRIVATE include
    PUBLIC src
)

target_link_directories(${PROJECT_NAME}
    PRIVATE submodules/glfw/src
    PRIVATE submodules/spdlog/src
    PRIVATE submodules/yaml-cpp/src
)

target_link_libraries(${PROJECT_NAME}
    PUBLIC spdlog
    PUBLIC glfw
    PUBLIC yaml-cpp
)

# copy our resources to executable location
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/res $<TARGET_FILE_DIR:${PROJECT_NAME}>)

if(WIN32)
    # copy dynamic libraries to executable location
    add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/lib/windows/dynamic $<TARGET_FILE_DIR:${PROJECT_NAME}>)
    # link static libraries
    target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/lib/windows/irrKlang.lib)
endif(WIN32)

if(UNIX)
    # copy dynamic libraries to executable location
    add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/lib/unix/dynamic $<TARGET_FILE_DIR:${PROJECT_NAME}>)
    # link static libraries
    target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/lib/unix/irrKlang.a)
endif(UNIX)

add_compile_definitions(GLFW_INCLUDE_NONE)

if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /wd4996 /external:W0)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
