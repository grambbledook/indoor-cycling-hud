cmake_minimum_required(VERSION 3.10)
project(HelloWorld LANGUAGES CXX)

SET(CMAKE_CXX_STANDARD 23)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;RelWithDebInfo" CACHE STRING "" FORCE)

#--------------------Include QT------------------------------
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_PREFIX_PATH $ENV{QT_HOME})

find_package(Qt6Core CONFIG REQUIRED)
find_package(Qt6Widgets CONFIG REQUIRED)

#--------------------Include Project Files-------------------
set(ASSETS_DIR ${CMAKE_SOURCE_DIR}/assets)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)

include_directories(${ASSETS_DIR})
include_directories(${INCLUDE_DIR})
include_directories(${SOURCE_DIR})

file(GLOB_RECURSE SOURCES
        ${SOURCE_DIR}/*.cpp
        ${INCLUDE_DIR}/*.h
)

#--------------------Set Executable-------------------------
set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS
        $<$<CONFIG:RELWITHDEBINFO>:QT_MESSAGELOGCONTEXT>)

# no forms so far
#set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_AUTOUIC_SEARCH_PATHS} ${FORMS_DIR})

add_executable(HelloWorld ${SOURCES}
        include/WorkoutWindow.h)

target_include_directories(HelloWorld PRIVATE ${INCLUDE_DIR})
target_include_directories(HelloWorld PRIVATE ${SOURCE_DIR})
target_include_directories(HelloWorld PRIVATE ${ASSETS_DIR})

#--------------------Link Libraries--------------------------
target_link_libraries(HelloWorld Qt6::Widgets Qt6::Core)
