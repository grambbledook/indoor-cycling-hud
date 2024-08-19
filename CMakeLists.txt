cmake_minimum_required(VERSION 3.10)
project(HUD LANGUAGES CXX)
add_subdirectory(bluetooth)
add_subdirectory(model)

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
list(APPEND CMAKE_PREFIX_PATH $ENV{CMAKE_MODULES_HOME})

find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Concurrent)

#--------------------Include spdlog-----------------------
find_package(spdlog CONFIG REQUIRED)

#--------------------Include Project Files-------------------
set(ASSETS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/assets)
set(INCLUDE_DIR view/include)
set(SOURCE_DIR view/src)
set(BLE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/bluetooth/include)
set(MODEL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/model/include)

if (${CONFIG} STREQUAL "Release") # If CONFIG is set
    set(RELEASE_TYPE ${CONFIG})
else()
    set(RELEASE_TYPE "Debug")
endif ()

set(EXECUTABLE_DIR "${CMAKE_CURRENT_BINARY_DIR}/${RELEASE_TYPE}")

if (${RELEASE_TYPE} STREQUAL "Release")
    set(QT_PLATFORMS_HOME_DIR "${CMAKE_CURRENT_BINARY_DIR}/vcpkg_installed/x64-windows/Qt6/plugins/platforms")
else()
    set(QT_PLATFORMS_HOME_DIR "${CMAKE_CURRENT_BINARY_DIR}/vcpkg_installed/x64-windows/debug/Qt6/plugins/platforms")
endif()

include_directories(${ASSETS_DIR})
include_directories(${INCLUDE_DIR})
include_directories(${SOURCE_DIR})

file(GLOB_RECURSE SOURCES
        ${SOURCE_DIR}/*.cpp
        ${INCLUDE_DIR}/*.h
)

#--------------------Set QT Executable-------------------------
qt_standard_project_setup()

set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS
        $<$<CONFIG:RELWITHDEBINFO>:QT_MESSAGELOGCONTEXT>)

qt_add_executable(HUD ${SOURCES}
        view/include/Hud.h)

target_include_directories(HUD PRIVATE ${INCLUDE_DIR})
target_include_directories(HUD PRIVATE ${SOURCE_DIR})
target_include_directories(HUD PRIVATE ${ASSETS_DIR})
target_include_directories(Bluetooth PUBLIC ${BLE_DIR})
target_include_directories(Model PUBLIC ${MODEL_DIR})
target_include_directories(Model PUBLIC ${MODEL_DIR})

#--------------------Link Libraries--------------------------
target_link_libraries(HUD PRIVATE Qt6::Widgets Qt6::Core Qt6::Concurrent Bluetooth Model)
target_link_libraries(HUD PRIVATE spdlog::spdlog)

#--------------------Copy Assets-----------------------------
add_custom_command(TARGET HUD POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${ASSETS_DIR} ${EXECUTABLE_DIR}/assets VERBATIM)

#--------------------Copy QT Platform DLLs----------------------------
file(GLOB QT_PLATFORM_FILES
        ${QT_PLATFORMS_HOME_DIR}/qwindows*.dll
        ${QT_PLATFORMS_HOME_DIR}/qwindows*.pdb
)

foreach(QT_PLATFORM_FILE ${QT_PLATFORM_FILES})
    get_filename_component(FILE_NAME ${QT_PLATFORM_FILE} NAME)
    add_custom_command(TARGET HUD POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${QT_PLATFORM_FILE} "${EXECUTABLE_DIR}/platforms/${FILE_NAME}" VERBATIM)
endforeach()
