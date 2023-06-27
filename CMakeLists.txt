cmake_minimum_required(VERSION 3.20)

project(risc-v-simulate)

set(CMAKE_CXX_STANDARD 20)
include_directories(utils)

file(GLOB sources RELATIVE ${PROJECT_SOURCE_DIR} src/*.cpp)

add_executable(code ${sources})
