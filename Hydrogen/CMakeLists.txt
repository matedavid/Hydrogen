cmake_minimum_required(VERSION 3.8)

project(Hydrogen)

set(CMAKE_CXX_STANDARD 20)

# Create executable
add_library(${PROJECT_NAME} STATIC)

# Compiler flags
target_compile_options(${PROJECT_NAME} BEFORE PRIVATE -Wall -Wpedantic -Wextra -Wshadow -Wconversion -Werror)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)

# Sources
target_sources(${PROJECT_NAME} PRIVATE
        src/core/application.cpp
        src/core/window.cpp
        src/core/model.cpp
        src/core/mesh.cpp
        src/core/material.cpp
        src/core/camera.cpp
        src/core/orthographic_camera.cpp
        src/core/perspective_camera.cpp

        src/input/input.cpp

        src/systems/shader_system.cpp
        src/systems/shader_compiler.cpp
        src/systems/texture_system.cpp

        src/renderer/vertex_array.cpp
        src/renderer/buffers.cpp
        src/renderer/shader.cpp
        src/renderer/texture.cpp
        src/renderer/renderer_api.cpp
        src/renderer/renderer3d.cpp
        )


# Function to configure assets, such as builtin shaders
set(SHADERS_PATH ${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders/ PARENT_SCOPE)

function(configure_assets TARGET_NAME TARGET_PATH)
    message("Copying assets to: ${TARGET_PATH}")
    file(MAKE_DIRECTORY ${TARGET_PATH}/shaders/)
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${SHADERS_PATH}/*
            ${TARGET_PATH}/shaders/)
endfunction(configure_assets)

# Include Interface
target_include_directories(${PROJECT_NAME} PUBLIC src/
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
        $<INSTALL_INTERFACE:include/Hydrogen>
        )

# OpenGL
#find_package(OpenGL REQUIRED)
#target_link_libraries(${PROJECT_NAME} OpenGL::GL)

# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/glfw-3.3.8)
target_link_libraries(${PROJECT_NAME} glfw)

# Glad
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/glad)
target_link_libraries(${PROJECT_NAME} glad)

# Glm
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/glm)
target_link_libraries(${PROJECT_NAME} glm::glm)

# stb libraries
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/vendor/stb/)

# assimp
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/assimp)
target_link_libraries(${PROJECT_NAME} assimp)

# spdlog
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/spdlog)
target_link_libraries(${PROJECT_NAME} spdlog::spdlog)
set_target_properties(spdlog PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/vendor/spdlog/include/)