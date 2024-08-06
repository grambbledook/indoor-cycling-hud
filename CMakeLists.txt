cmake_minimum_required(VERSION 3.10)
project(HelloWorld LANGUAGES CXX)
add_subdirectory(bluetooth)
add_subdirectory(model)

message(PROJECT="BAZA")
message(CMAKE_PREFIX_PATH="${CMAKE_PREFIX_PATH}")
message(CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")
message(CMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}")
message(PROJECT_BINARY_DIR="${PROJECT_BINARY_DIR}")

# control where the static and shared libraries are built so that on windows
# we don't need to tinker with the path to run the executable
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

SET(CMAKE_CXX_STANDARD 23)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS "/Z7 /Zi /ZI")
set(CMAKE_EXE_LINKER_FLAGS "/DEBUG")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#--------------------Include QT------------------------------
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
list(APPEND CMAKE_PREFIX_PATH $ENV{QT_HOME} $ENV{CMAKE_MODULES_HOME})

message(CMAKE_PREFIX_PATH="${CMAKE_PREFIX_PATH}")

find_package(Qt6Core CONFIG REQUIRED)
find_package(Qt6Widgets CONFIG REQUIRED)

#--------------------Include Project Files-------------------
set(ASSETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets)
set(INCLUDE_DIR view/include)
set(SOURCE_DIR view/src)
set(BLE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bluetooth/include)
set(MODEL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/model/include)

include_directories(${ASSETS_DIR})
include_directories(${INCLUDE_DIR})
include_directories(${SOURCE_DIR})

file(GLOB_RECURSE SOURCES
        ${SOURCE_DIR}/*.cpp
        ${INCLUDE_DIR}/*.h
)

#--------------------Set QT Executable-------------------------
set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS
        $<$<CONFIG:RELWITHDEBINFO>:QT_MESSAGELOGCONTEXT>)

add_executable(HelloWorld ${SOURCES}
        view/include/Hud.h)

target_include_directories(HelloWorld PRIVATE ${INCLUDE_DIR})
target_include_directories(HelloWorld PRIVATE ${SOURCE_DIR})
target_include_directories(HelloWorld PRIVATE ${ASSETS_DIR})
target_include_directories(Bluetooth PUBLIC ${BLE_DIR})
target_include_directories(Model PUBLIC ${MODEL_DIR})

#--------------------Link Libraries--------------------------
target_link_libraries(HelloWorld Qt6::Widgets Qt6::Core Bluetooth Model)
