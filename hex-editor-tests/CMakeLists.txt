cmake_minimum_required(VERSION 3.16)

project(HexEditorTest LANGUAGES CXX)

enable_testing()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Test Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test Gui)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(ArgumentParserTest tst_argumentparser.cpp ../argumentparser.cpp)
add_test(NAME ArgumentParserTest COMMAND ArgumentParserTest)
target_link_libraries(ArgumentParserTest PRIVATE Qt${QT_VERSION_MAJOR}::Test)

add_executable(ByteBufferTest tst_bytebuffer.cpp ../bytebuffer.cpp)
add_test(NAME ByteBufferTest COMMAND ByteBufferTest)
target_link_libraries(ByteBufferTest PRIVATE Qt${QT_VERSION_MAJOR}::Test)

add_executable(FileManagerTest tst_filemanager.cpp ../filemanager.cpp ../bytebuffer.cpp)
add_test(NAME FileManagerTest COMMAND FileManagerTest)
target_link_libraries(FileManagerTest PRIVATE Qt${QT_VERSION_MAJOR}::Test)

add_executable(EditorModelTest tst_editormodel.cpp ../editormodel.cpp ../bytebuffer.cpp)
add_test(NAME EditorModelTest COMMAND EditorModelTest)
target_link_libraries(EditorModelTest PRIVATE Qt${QT_VERSION_MAJOR}::Test Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui)

add_executable(FindPanelTest tst_findpanel.cpp ../findpanel.cpp ../editormodel.cpp ../bytebuffer.cpp)
add_test(NAME FindPanelTest COMMAND FindPanelTest)
target_link_libraries(FindPanelTest PRIVATE Qt${QT_VERSION_MAJOR}::Test Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Widgets)

add_executable(EditorViewTest tst_editorview.cpp ../editorview.cpp ../editormodel.cpp ../bytebuffer.cpp)
add_test(NAME EditorViewTest COMMAND EditorViewTest)
target_link_libraries(EditorViewTest PRIVATE Qt${QT_VERSION_MAJOR}::Test Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Widgets)

add_executable(MainWindowTest tst_mainwindow.cpp ../mainwindow.cpp ../findpanel.cpp ../editorview.cpp ../editormodel.cpp ../bytebuffer.cpp ../filemanager.cpp)
add_test(NAME MainWindowTest COMMAND MainWindowTest)
target_link_libraries(MainWindowTest PRIVATE Qt${QT_VERSION_MAJOR}::Test Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Widgets)

add_executable(HexEditorAppTest tst_hexeditorapp.cpp ../hexeditorapp.cpp ../mainwindow.cpp ../findpanel.cpp ../editorview.cpp ../editormodel.cpp ../bytebuffer.cpp ../filemanager.cpp ../argumentparser.cpp)
add_test(NAME HexEditorAppTest COMMAND HexEditorAppTest)
target_link_libraries(HexEditorAppTest PRIVATE Qt${QT_VERSION_MAJOR}::Test Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Widgets)

