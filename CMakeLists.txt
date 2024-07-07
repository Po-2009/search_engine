cmake_minimum_required(VERSION 3.27)
project(search_engine)
enable_testing()
include(FetchContent)

# FetchContent для GoogleTest
FetchContent_Declare(
        test/googletest
        URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)



set(CMAKE_CXX_STANDARD 17)

add_subdirectory("nlohmann json")

set(JSON_BuildTests OFF CACHE INTERNAL "")

add_executable(search_engine main.cpp
        include/SearchServer.h
        include/JsonConverter.h
        include/InvertedIndex.h)

target_link_libraries(search_engine PRIVATE nlohmann_json::nlohmann_json)

add_compile_definitions(PROJECT_BINARY_DIR="${CMAKE_BINARY_DIR}")
add_definitions(-DEXECUTABLE_NAME="${CMAKE_PROJECT_NAME}")

add_subdirectory(test)
