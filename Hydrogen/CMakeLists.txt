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
        src/application.cpp
        src/window.cpp
        src/input.cpp

        src/renderer/vertex_array.cpp
        src/renderer/buffers.cpp
        src/renderer/shader.cpp
        src/renderer/texture.cpp
        src/renderer/renderer_api.cpp
        src/renderer/renderer2d.cpp
        src/renderer/renderer3d.cpp
        src/renderer/camera.cpp
        )

# Include Interface
target_include_directories(${PROJECT_NAME} PUBLIC src/
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

# stb libraries
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb/)