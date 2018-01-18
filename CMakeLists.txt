cmake_minimum_required(VERSION 3.9)
project(GPUBasedRendering_Project)

FIND_PACKAGE(OpenGL REQUIRED)
INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp main.h global.h system.cpp system.h utility.cpp utility.h Shader.cpp Shader.h
        drawable.cpp drawable.h glutils.cpp glutils.h vbobird.cpp vbobird.h)

include_directories(include)

link_directories(lib)

add_executable(GPUBasedRendering_Project ${SOURCE_FILES})

target_link_libraries(GPUBasedRendering_Project ${OPENGL_LIBRARIES} glut32.lib glut.lib glew32.lib glew32s.lib)