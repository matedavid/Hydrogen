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
        src/test.cpp
        )

target_include_directories(${PROJECT_NAME} PRIVATE src/)

# Include Interface
target_include_directories(${PROJECT_NAME} PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
        $<INSTALL_INTERFACE:include/Hydrogen>
        )