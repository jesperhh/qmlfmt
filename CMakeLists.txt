cmake_minimum_required(VERSION 3.0)

project(qmlfmt)

cmake_policy(SET CMP0022 NEW)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

find_package(Qt5Core REQUIRED)

include(QtCreator/QtCreator.cmake)

add_subdirectory(diff_match_patch)

add_executable(qmlfmt qmlfmt.cpp)
target_link_libraries(qmlfmt qmljs diff_match_patch Qt5::Core)

if(CMAKE_COMPILER_IS_GNUCXX)
	target_compile_options(qmlfmt PRIVATE -std=c++11 -Wall)
elseif(MSVC)
	target_compile_options(qmlfmt PRIVATE -analyze -WX -wd28182 -wd6308 -wd6385 -wd6011)
endif()
