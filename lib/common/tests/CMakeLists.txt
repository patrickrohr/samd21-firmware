cmake_minimum_required(VERSION 3.11)

find_package(GoogleTest REQUIRED)

add_executable(common_utils_test)

target_sources(common_utils_test
    PRIVATE
        src/array_test.cpp
        src/list_test.cpp
        src/main.cpp
        src/map_test.cpp
        src/vector_test.cpp
)

target_link_libraries(common_utils_test
    PUBLIC
        common_utils
        gtest
)

gtest_add_tests(TARGET common_utils_test)
