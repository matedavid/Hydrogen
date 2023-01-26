cmake_minimum_required(VERSION 3.8)

project(Hydrogen)

set(CMAKE_CXX_STANDARD 20)


# Create executable
add_library(${PROJECT_NAME} STATIC)

# Compiler flags
target_compile_options(${PROJECT_NAME} BEFORE PUBLIC -Wall -Wpedantic -Wextra -Wshadow -Wconversion)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)

# Sources
target_sources(${PROJECT_NAME} PRIVATE
        src/application.cpp
        src/window.cpp
        src/input.cpp

        src/material.cpp
        src/shader_compiler.cpp

        src/models/model.cpp
        src/models/mesh.cpp

        src/systems/shader_system.cpp
        src/systems/texture_system.cpp

        src/renderer/vertex_array.cpp
        src/renderer/buffers.cpp
        src/renderer/shader.cpp
        src/renderer/texture.cpp
        src/renderer/renderer_api.cpp
        src/renderer/renderer2d.cpp
        src/renderer/renderer3d.cpp
        src/renderer/camera.cpp
        src/renderer/orthographic_camera.cpp
        src/renderer/perspective_camera.cpp
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

# assimp
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/assimp)
target_link_libraries(${PROJECT_NAME} assimp)

# spdlog
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/spdlog )
target_link_libraries(${PROJECT_NAME} spdlog::spdlog)
set_target_properties(spdlog PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/vendor/spdlog/include/)