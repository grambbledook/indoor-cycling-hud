cmake_minimum_required(VERSION 3.10)
project(HelloWorld LANGUAGES CXX)

SET(CMAKE_CXX_STANDARD 23)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo" CACHE STRING "" FORCE)

#--------------------Include QT------------------------------
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
list(APPEND CMAKE_PREFIX_PATH $ENV{QT_HOME} $ENV{CMAKE_MODULES_HOME})

message(CMAKE_PREFIX_PATH="${CMAKE_PREFIX_PATH}")

find_package(Qt6Core CONFIG REQUIRED)
find_package(Qt6Widgets CONFIG REQUIRED)

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
set(ASSETS_DIR ${CMAKE_SOURCE_DIR}/assets)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)

include_directories(${ASSETS_DIR})
include_directories(${INCLUDE_DIR})
include_directories(${SOURCE_DIR})
include_directories(${WindowsSDK_INCLUDE_DIR})

file(GLOB_RECURSE SOURCES
        ${SOURCE_DIR}/*.cpp
        ${INCLUDE_DIR}/*.h
)

#--------------------Set QT Executable-------------------------
set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS
        $<$<CONFIG:RELWITHDEBINFO>:QT_MESSAGELOGCONTEXT>)

# no forms so far
#set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_AUTOUIC_SEARCH_PATHS} ${FORMS_DIR})

add_executable(HelloWorld ${SOURCES}
        include/Hud.h)

target_include_directories(HelloWorld PRIVATE ${INCLUDE_DIR})
target_include_directories(HelloWorld PRIVATE ${SOURCE_DIR})
target_include_directories(HelloWorld PRIVATE ${ASSETS_DIR})

#--------------------Link Libraries--------------------------
target_link_libraries(HelloWorld Qt6::Widgets Qt6::Core)

#--------------------Set WinRT Executable-------------------------

add_executable(WinRt src/WinRT.cpp include/WinRT.h)
include_directories(${WindowsSDK_INCLUDE_DIR})
target_link_libraries(WinRt
        WindowsApp.lib
        windowsapp
)

if (WIN32)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    add_definitions(-DUNICODE)
    add_definitions(-D_UNICODE)
endif()
