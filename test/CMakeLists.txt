cmake_minimum_required(VERSION 3.27)
project(TestProject)

add_executable(all_tests all_tests.cpp)

target_link_libraries(all_tests PRIVATE gtest gtest_main  nlohmann_json::nlohmann_json)
include(GoogleTest)
gtest_discover_tests(all_tests)