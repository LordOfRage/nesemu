cmake_minimum_required(VERSION 3.18)
set(CMAKE_CXX_COMPILER cl.exe)

add_compile_definitions(GLEW_STATIC)
link_directories("${CMAKE_SOURCE_DIR}/glfw-3.4.bin.WIN64/lib-vc2022" "${CMAKE_SOURCE_DIR}/glew-2.1.0/lib/Release/x64")

project(main)
add_executable(main main.cpp)

target_include_directories(main PUBLIC "${CMAKE_SOURCE_DIR}/glfw-3.4.bin.WIN64/include/GLFW" "/glew-2.1.0/include/GL" "${CMAKE_SOURCE_DIR}/glm")

add_library(cpu CPU.hpp CPU.cpp instructions.cpp opcodes.cpp)
add_library(rom ROM.hpp ROM.cpp)
add_library(dbg debug.hpp debug.cpp)
add_library(render renderer.hpp renderer.cpp)
target_include_directories(render PUBLIC "${CMAKE_SOURCE_DIR}/glfw-3.4.bin.WIN64/include/GLFW" "${CMAKE_SOURCE_DIR}/glew-2.1.0/include/GL" "${CMAKE_SOURCE_DIR}/glm")

target_link_libraries(main PUBLIC cpu rom dbg render glfw3.lib user32.lib kernel32.lib opengl32.lib glew32s.lib)
