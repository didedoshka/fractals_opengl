cmake_minimum_required(VERSION 3.26)
project(main)
set(CMAKE_C_STANDARD 99)

find_package(OpenGL REQUIRED)
find_package(GLFW3 REQUIRED)
add_subdirectory(glad)

add_executable(main main.c)
target_include_directories(main PUBLIC "./include")
target_link_libraries(main glfw glad)
