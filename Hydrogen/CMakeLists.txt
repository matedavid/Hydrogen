cmake_minimum_required(VERSION 3.8)

project(Hydrogen)

set(CMAKE_CXX_STANDARD 20)

# Compiler flags
add_compile_options(-Wall -Wpedantic -Wextra -Wshadow -Wconversion)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)

# Create executable
add_library(${PROJECT_NAME} SHARED)

# Sources
target_sources(${PROJECT_NAME} PRIVATE
        src/vertex_array.cpp
        src/buffers.cpp
        src/shader.cpp
        src/renderer.cpp
        src/application.cpp
        )

target_include_directories(${PROJECT_NAME} PRIVATE src/)

# Include Interface
target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
        $<INSTALL_INTERFACE:include/Hydrogen>
        )

# GLFW
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/glfw-3.3.8)
target_link_libraries(${PROJECT_NAME} glfw)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# Glad
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad)
target_link_libraries(${PROJECT_NAME} glad)

# OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} OpenGL::GL)

# Glm
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/glm)
target_link_libraries(${PROJECT_NAME} glm::glm)
