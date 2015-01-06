cmake_minimum_required(VERSION 3.0)

project(qmlfmt)

cmake_policy(SET CMP0022 NEW)

include(QtCreator/QtCreator.cmake)

set(CMAKE_PREFIX_PATH "C:/Qt/5.3/msvc2013_64")

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Concurrent REQUIRED)
find_package(Qt5Script REQUIRED)
find_package(Qt5Xml REQUIRED)

add_subdirectory(diff_match_patch)

add_executable(qmlfmt qmlfmt.cpp)
target_link_libraries(qmlfmt Qt5::Widgets Qt5::Script Qt5::Xml Qt5::Network Qt5::Concurrent diff_match_patch qmljs)