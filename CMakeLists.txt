project(ragna)
cmake_minimum_required(VERSION 3.5.0)

if(POLICY CMP0072)
    # Prefer newer GL libraries for OpenGL.
    cmake_policy(SET CMP0072 NEW)
endif()

include(FindPkgConfig)
find_package(
    Qt5
    COMPONENTS
        Widgets
    REQUIRED
)
find_package(OpenGL REQUIRED)
pkg_check_modules(
    V4L2
        libv4l2
    REQUIRED
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(MOCUIC_DIR ${CMAKE_CURRENT_BINARY_DIR}/.mocuic)

if(WITH_DEBUG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -O0")
endif()

qt5_add_resources(
    RCC_SOURCES
        src/resources.qrc
)

add_executable(
    ragna
        src/capture.cpp
        src/paint.cpp
        src/ragnaconfigcombobox.cpp
        src/ragnaconfigwindow.cpp
        src/ragnacontroller.cpp
        src/ragna.cpp
        src/ragnaprefs.cpp
        src/ragnascrollarea.cpp
        src/v4l-common/codec-fwht.c
        src/v4l-common/codec-v4l2-fwht.c
        src/v4l-common/v4l2-info.cpp
        src/v4l-common/v4l2-tpg-colors.c
        src/v4l-common/v4l2-tpg-core.c
        src/v4l-common/v4l-stream.c
        ${RCC_SOURCES}
)

set_target_properties(
    ragna
    PROPERTIES
        AUTOGEN_BUILD_DIR
            ${MOCUIC_DIR}
)

target_include_directories(
    ragna
    PRIVATE
        src/v4l-common/
)

target_link_libraries(
    ragna
        OpenGL
        Qt5::Widgets
        v4l2
)

install(
    TARGETS
        ragna
    DESTINATION
        bin
)
