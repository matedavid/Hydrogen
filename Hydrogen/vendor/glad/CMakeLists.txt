cmake_minimum_required(VERSION 3.8)
project(glad LANGUAGES C)

add_library(glad SHARED ${CMAKE_CURRENT_SOURCE_DIR}/include/glad/glad.h ${CMAKE_CURRENT_SOURCE_DIR}/src/glad.c)
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include/)
