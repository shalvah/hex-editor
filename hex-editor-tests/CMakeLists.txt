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

