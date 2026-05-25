cmake_minimum_required(VERSION 3.16)

project(HexEditorTest LANGUAGES CXX)

enable_testing()

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Test)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(ArgumentParserTest tst_argumentparsertest.cpp ../argumentparser.cpp)
add_test(NAME ArgumentParserTest COMMAND ArgumentParserTest)

target_link_libraries(ArgumentParserTest PRIVATE Qt${QT_VERSION_MAJOR}::Test)

