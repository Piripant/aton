cmake_minimum_required(VERSION 3.1)
project(Aton)

message( "Using ${CMAKE_CXX_COMPILER_ID} c++ compiler" )

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "bin")

file(GLOB_RECURSE sources src/*.cpp)
file(GLOB_RECURSE ext ext/*.cpp)

include_directories(ext/imgui)
include_directories(ext/imguifilesystem)
link_directories(libs)

add_executable(Aton ${sources} ${ext})

target_link_libraries(Aton lua53)
target_link_libraries(Aton SDL2)
target_link_libraries(Aton GLEW)
target_link_libraries(Aton GL)
target_link_libraries(Aton dl)
target_link_libraries(Aton pthread)
