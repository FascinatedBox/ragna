project(ragna)
cmake_minimum_required(VERSION 3.5.0)

include(FindPkgConfig)

if(POLICY CMP0072)
    # Prefer newer GL libraries for OpenGL.
    cmake_policy(SET CMP0072 NEW)
endif()

if(WITH_DEBUG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g3 -O0")
endif()

find_package(
    Qt5
    REQUIRED
        Widgets
)
find_package(OpenGL REQUIRED)
pkg_check_modules(V4L2 libv4l2 REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(MOCUIC_DIR ${CMAKE_CURRENT_BINARY_DIR}/.mocuic)
set(RAGNA_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(src)
