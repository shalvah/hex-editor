cmake_minimum_required(VERSION 3.19)
project(hex-editor LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(hex-editor
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    argumentparser.h argumentparser.cpp
    bytebuffer.h bytebuffer.cpp
    filemanager.h filemanager.cpp
    constants.h
)

target_link_libraries(hex-editor
    PRIVATE
        Qt::Core
        Qt::Widgets
)

install(TARGETS hex-editor
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET hex-editor
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
add_subdirectory(hex-editor-tests)