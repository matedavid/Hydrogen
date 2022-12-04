cmake_minimum_required(VERSION 3.8)

project(Hydrogen)

set(CMAKE_CXX_STANDARD 20)

# Create executable
add_library(${PROJECT_NAME} STATIC)

# Compiler flags
target_compile_options(${PROJECT_NAME} BEFORE PRIVATE -Wall -Wpedantic -Wextra -Wshadow -Wconversion)

# Sources
target_sources(${PROJECT_NAME} PRIVATE
        src/test.cpp
        )

target_include_directories(${PROJECT_NAME} PRIVATE src/)