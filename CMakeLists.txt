cmake_minimum_required(VERSION 3.1)
project(Aton)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin")

file(GLOB_RECURSE sources src/*.cpp)

link_directories(libs)

add_executable(Aton ${sources})
target_link_libraries(Aton lua53)
target_link_libraries(Aton SDL2)
target_link_libraries(Aton dl)
target_link_libraries(Aton pthread)