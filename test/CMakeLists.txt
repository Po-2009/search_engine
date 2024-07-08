cmake_minimum_required(VERSION 3.10)
project(MyProject)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/googletest" "googletest")

add_executable(all_tests all_tests.cpp
                    ../src/InvertedIndex.cpp
                    ../src/JsonConverter.cpp
                    ../src/SearchServer.cpp
)

target_include_directories(all_tests PRIVATE
        ${GTEST_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}
)


target_link_libraries(all_tests PRIVATE
        ${GTEST_BOTH_LIBRARIES}
        gtest
        gtest_main
        nlohmann_json::nlohmann_json
)

include(CTest)

add_test(NAME all_tests COMMAND all_tests)
