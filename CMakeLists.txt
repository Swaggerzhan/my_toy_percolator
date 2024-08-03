cmake_minimum_required(VERSION 3.10)

project(percolator)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

include_directories(${CMAKE_SOURCE_DIR})
aux_source_directory(src SOURCE_CODE)


# test_basic
set(TEST_NAME test_basic)
add_executable(${TEST_NAME}
    test/${TEST_NAME}.cc
	${SOURCE_CODE}
)

target_link_libraries(
    ${TEST_NAME}
    libgtest.a
)
