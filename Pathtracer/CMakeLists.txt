cmake_minimum_required(VERSION 3.28.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(Pathtracer VERSION 1.0)

add_executable(${PROJECT_NAME} 
    src/main.cpp
    src/Ray.hpp
    src/Color.hpp
    src/Hittable.hpp
    src/Sphere.hpp
    src/Vector.cpp
    src/Vector.hpp
)

target_include_directories(${PROJECT_NAME} 
    PUBLIC src
)
