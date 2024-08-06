cmake_minimum_required(VERSION 3.10)
SET(CMAKE_CXX_STANDARD 23)

find_package(GTest CONFIG REQUIRED)

file(GLOB_RECURSE TESTS src/*.cpp)

add_executable(ModelTests ${TESTS} ${sqlite3_SOURCE_DIR})

target_include_directories(ModelTests PRIVATE include)
target_link_libraries(ModelTests PRIVATE Model)
target_link_libraries(ModelTests PRIVATE GTest::gtest GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(ModelTests)