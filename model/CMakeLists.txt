cmake_minimum_required(VERSION 3.10)
project(Model LANGUAGES CXX)
include(FetchContent)

add_subdirectory(tests)

SET(CMAKE_CXX_STANDARD 23)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zi /Od")
set(CMAKE_EXE_LINKER_FLAGS "/DEBUG")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#--------------------Setup paths-----------------------------
set(CMAKE_INCLUDE_CURRENT_DIR ON)
list(APPEND CMAKE_PREFIX_PATH $ENV{CMAKE_MODULES_HOME})

message(CMAKE_PREFIX_PATH="${CMAKE_PREFIX_PATH}")


#--------------------Include Project Files-------------------
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(BLE_DIR ${CMAKE_SOURCE_DIR}/bluetooth/include)

include_directories(${INCLUDE_DIR})
include_directories(${SOURCE_DIR})

file(GLOB_RECURSE SOURCES
        ${SOURCE_DIR}/*.cpp
        ${INCLUDE_DIR}/*.h
)

add_library(Model STATIC ${SOURCES})

target_include_directories(Model PUBLIC ${INCLUDE_DIR})
target_include_directories(Model PUBLIC ${SOURCE_DIR})
target_include_directories(Bluetooth PUBLIC ${BLE_DIR})
#--------------------Find Packages---------------------------
find_package(spdlog CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)

#--------------------Link Libraries--------------------------
target_link_libraries(Model PUBLIC Bluetooth)
target_link_libraries(Model PUBLIC unofficial::sqlite3::sqlite3)
target_link_libraries(Model PRIVATE spdlog::spdlog)

if (WIN32)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    add_definitions(-DUNICODE)
    add_definitions(-D_UNICODE)
endif ()
