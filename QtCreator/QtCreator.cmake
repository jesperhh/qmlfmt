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

find_path(QT_CREATOR_SRC "qtcreator.pro" DOC "Path to Qt Creator source")

include(ExternalProject)

if(NOT QT_CREATOR_SRC)	
	ExternalProject_Add(
		QtCreator
		URL "https://download.qt.io/official_releases/qtcreator/4.5/4.5.2/qt-creator-opensource-src-4.5.2.tar.gz"
		UPDATE_COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_SOURCE_DIR}/QtCreator/CMakeLists.txt" .
		CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
               -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG:PATH=Debug
               -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE:PATH=Release
               -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
		INSTALL_COMMAND "")
else()
	ExternalProject_Add(
		QtCreator
		SOURCE_DIR ${QT_CREATOR_SRC}
		UPDATE_COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_SOURCE_DIR}/QtCreator/CMakeLists.txt" .
		CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
               -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG:PATH=Debug
               -DCMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE:PATH=Release
               -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
		INSTALL_COMMAND "")
endif()

ExternalProject_Get_Property(QtCreator SOURCE_DIR)
ExternalProject_Get_Property(QtCreator BINARY_DIR)

add_library(qmljs STATIC IMPORTED)
add_dependencies(qmljs QtCreator)

set_property(TARGET qmljs PROPERTY IMPORTED_LOCATION_DEBUG "${BINARY_DIR}/Debug/${CMAKE_STATIC_LIBRARY_PREFIX}qmljs${CMAKE_STATIC_LIBRARY_SUFFIX}")
set_property(TARGET qmljs PROPERTY IMPORTED_LOCATION_RELEASE "${BINARY_DIR}/Release/${CMAKE_STATIC_LIBRARY_PREFIX}qmljs${CMAKE_STATIC_LIBRARY_SUFFIX}")

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Concurrent REQUIRED)
find_package(Qt5Script REQUIRED)
find_package(Qt5Xml REQUIRED)

set_property(TARGET qmljs PROPERTY INTERFACE_LINK_LIBRARIES
	"${BINARY_DIR}/$<CONFIG>/${CMAKE_STATIC_LIBRARY_PREFIX}cplusplus${CMAKE_STATIC_LIBRARY_SUFFIX}"
	"${BINARY_DIR}/$<CONFIG>/${CMAKE_STATIC_LIBRARY_PREFIX}utils${CMAKE_STATIC_LIBRARY_SUFFIX}"
	"${BINARY_DIR}/$<CONFIG>/${CMAKE_STATIC_LIBRARY_PREFIX}languageutils${CMAKE_STATIC_LIBRARY_SUFFIX}"
	Qt5::Widgets Qt5::Script Qt5::Xml Qt5::Network Qt5::Concurrent)

file(MAKE_DIRECTORY "${SOURCE_DIR}/src/libs/")
file(MAKE_DIRECTORY "${SOURCE_DIR}/src/libs/3rdparty/")

set_property(TARGET qmljs PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${SOURCE_DIR}/src/libs/3rdparty/" "${SOURCE_DIR}/src/libs/")
set_property(TARGET qmljs PROPERTY INTERFACE_COMPILE_DEFINITIONS 
	"QML_BUILD_STATIC_LIB" "QT_CREATOR" "_CRT_SECURE_NO_WARNINGS" "QTCREATOR_UTILS_STATIC_LIB" "CPLUSPLUS_BUILD_LIB" "LANGUAGEUTILS_BUILD_STATIC_LIB")
