qt5_add_resources(
    RCC_SOURCES
    ragna.qrc
)

add_executable(
    ragna
        capture.cpp
        paint.cpp
        ragna.cpp
        ragnaconfigcombobox.cpp
        ragnaconfigwindow.cpp
        ragnacontroller.cpp
        ragnaprefs.cpp
        v4l-common/codec-fwht.c
        v4l-common/codec-v4l2-fwht.c
        v4l-common/v4l2-info.cpp
        v4l-common/v4l2-tpg-colors.c
        v4l-common/v4l2-tpg-core.c
        v4l-common/v4l-stream.c
        ${RCC_SOURCES}
)

set_target_properties(
    ragna
    PROPERTIES
        AUTOGEN_BUILD_DIR
            ${MOCUIC_DIR}
        RUNTIME_OUTPUT_DIRECTORY
            ${RAGNA_BINARY_DIR}
)

target_include_directories(
    ragna
    PRIVATE
        v4l-common
)

target_link_libraries(
    ragna
        OpenGL
        Qt5::Widgets
        v4l2
)

install(TARGETS ragna DESTINATION bin)
