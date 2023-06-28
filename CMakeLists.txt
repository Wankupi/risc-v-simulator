cmake_minimum_required(VERSION 3.15)

project(risc-v-simulate)

set(CMAKE_CXX_STANDARD 17)
include_directories(utils)

file(GLOB sources RELATIVE ${PROJECT_SOURCE_DIR} src/*.cpp)

add_executable(code ${sources})


enable_testing()
file(GLOB testcases RELATIVE ${PROJECT_SOURCE_DIR}/data data/*.data)
foreach (testcase ${testcases})
    string(REPLACE ".data" "" testcase ${testcase})
    if (testcase STREQUAL "pi")
        continue()
    endif()
    add_test(NAME ${testcase} WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/run
            COMMAND bash -c "$<TARGET_FILE:code> <../data/${testcase}.data")
    set_property(TEST ${testcase} PROPERTY TIMEOUT 3)
endforeach ()
