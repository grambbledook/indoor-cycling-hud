cmake_minimum_required(VERSION 3.10)

SET(CMAKE_CXX_STANDARD 23)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS "/Z7")

#--------------------Include QT------------------------------
set(CMAKE_INCLUDE_CURRENT_DIR ON)
list(APPEND CMAKE_PREFIX_PATH $ENV{CMAKE_MODULES_HOME})

message(CMAKE_PREFIX_PATH="${CMAKE_PREFIX_PATH}")

#--------------------Include WinRT------------------------------

# Add the Windows SDK include directories
include_directories(
        "$ENV{WINDOWS_SDK_HOME}/um"
        "$ENV{WINDOWS_SDK_HOME}/ucrt"
        "$ENV{WINDOWS_SDK_HOME}/shared"
        "$ENV{WINDOWS_SDK_HOME}/cppwinrt"
)

# Add the Windows SDK library directories
link_directories(
        "$ENV{WINDOWS_SDK_HOME}/um/x64"
        "$ENV{WINDOWS_SDK_HOME}/ucrt/x64"
)

#--------------------Include Project Files-------------------
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)

include_directories(${ASSETS_DIR})
include_directories(${SOURCE_DIR})
include_directories(${WindowsSDK_INCLUDE_DIR})

file(GLOB_RECURSE SOURCES
        ${SOURCE_DIR}/*.cpp
        ${INCLUDE_DIR}/*.h
)

#--------------------Set QT Executable-------------------------
add_library(Bluetooth STATIC ${SOURCES})

target_include_directories(Bluetooth PRIVATE ${INCLUDE_DIR})
target_include_directories(Bluetooth PRIVATE ${SOURCE_DIR})

include_directories(${WindowsSDK_INCLUDE_DIR})

#--------------------Link Libraries--------------------------
target_link_libraries(Bluetooth Qt WindowsApp.lib windowsapp)

if (WIN32)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    add_definitions(-DUNICODE)
    add_definitions(-D_UNICODE)
endif()
