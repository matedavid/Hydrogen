cmake_minimum_required(VERSION 3.8)

project(Sandbox)

set(CMAKE_CXX_STANDARD 20)

# Create executable
add_executable(${PROJECT_NAME})

# Compiler flags
target_compile_options(${PROJECT_NAME} BEFORE PRIVATE -Wall -Wpedantic -Wextra -Wshadow -Wconversion)

# Sources
target_sources(${PROJECT_NAME} PRIVATE
        src/main.cpp
        )

target_include_directories(${PROJECT_NAME} PRIVATE src/)
# target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/Hydrogen/src)

target_link_libraries(${PROJECT_NAME} Hydrogen)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION Hydrogen)
