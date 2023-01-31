cmake_minimum_required(VERSION 3.10.2)
project(tinyRPC LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/example/bin)
# set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
set(CMAKE_BUILD_TYPE "Debug")
set(CXX_FLAGS ${CXX_FLAGS} -g)
# set(CMAKE_CXX_FLAGS_RELEASE "-O2")

include_directories(include)
include_directories(tinyWebLib/include)
link_directories(tinyWebLib/lib)

file(GLOB SRC ./src/*)

set(TESTS ON)
if(DEFINED TESTS)
    add_subdirectory(example)
endif()

add_library(tinyRPC SHARED ${SRC})
target_link_libraries(tinyRPC tinyWebLib asyncLogger protobuf zookeeper_mt pthread)
