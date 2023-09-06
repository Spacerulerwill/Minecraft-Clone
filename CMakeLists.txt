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
    src/math/Math.hpp
    src/opengl/Framebuffer.cpp
    src/opengl/IndexBuffer.cpp
    src/opengl/VertexArray.cpp
    src/opengl/VertexBuffer.cpp
    src/opengl/VertexBufferLayout.cpp
    src/world/Block.cpp
    src/world/Chunk.cpp
    src/world/Skybox.cpp
    src/util/Log.cpp
    src/util/TextureLoad.cpp
)

# GET ALL SUBMODULES
find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(
            COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            RESULT_VARIABLE GIT_SUBMOD_RESULT
        )
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}")
        endif()
    endif()
endif()

# CHECK ALL SUBMODULES
if (NOT GIT_SUBMODULE)
    if(NOT EXISTS "${PROJECT_SOURCE_DIR}/submodules/glfw/CMAKELists.txt")
        message(FATAL_ERROR, "GLFW submodule not downloaded!")
    endif()
    if(NOT EXISTS "${PROJECT_SOURCE_DIR}/submodules/spdlog/CMAKELists.txt")
        message(FATAL_ERROR, "SPDLOG submodule not downloaded!")
    endif()
    if(NOT EXISTS "${PROJECT_SOURCE_DIR}/submodules/fmt/CMAKELists.txt")
        message(FATAL_ERROR, "fmt submodule not downloaded!")
    endif()
    if(NOT EXISTS "${PROJECT_SOURCE_DIR}/submodules/yaml-cpp/CMAKELists.txt")
        message(FATAL_ERROR, "yaml-cpp submodule not downloaded!")
    endif()
endif()

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