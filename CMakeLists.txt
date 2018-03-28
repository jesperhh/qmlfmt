#  Copyright (c) 2015-2016, Jesper Hellesø Hansen
#  jesperhh@gmail.com
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#      * Redistributions of source code must retain the above copyright
#        notice, this list of conditions and the following disclaimer.
#      * Redistributions in binary form must reproduce the above copyright
#        notice, this list of conditions and the following disclaimer in the
#        documentation and/or other materials provided with the distribution.
#      * Neither the name of the <organization> nor the
#        names of its contributors may be used to endorse or promote products
#        derived from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
#  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
#  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
#  DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
#  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
#  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
#  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
#  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
#  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

cmake_minimum_required(VERSION 3.1)

project(qmlfmt)

cmake_policy(SET CMP0022 NEW)

set_property(GLOBAL PROPERTY USE_FOLDERS On)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC true)
set(CMAKE_INCLUDE_CURRENT_DIR true)

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
	target_compile_options(qmlfmt PRIVATE -Wall)
elseif(MSVC)
	target_compile_options(qmlfmt PRIVATE -analyze -WX -wd28182 -wd6308 -wd6385 -wd6011)
endif()

option(BUILD_TESTING "Build the testing tree." ON)

if(BUILD_TESTING)
	enable_testing()
	add_subdirectory(test)
    add_test(NAME qmlfmt-test COMMAND testrunner $<TARGET_FILE:qmlfmt>)
    add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIG> --verbose --output-on-failure)
endif()
