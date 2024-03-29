#  Copyright (c) 2015-2020, Jesper Hellesø Hansen
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


set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED Core5Compat Qml Xml Concurrent Network Widgets Core)

add_definitions(-DQT_NO_CAST_TO_ASCII -DQT_RESTRICTED_CAST_FROM_ASCII -DQT_USE_FAST_OPERATOR_PLUS -DQT_USE_FAST_CONCATENATION -DQT_DISABLE_DEPRECATED_BEFORE=0x050900 -DQT_CREATOR -DQTC_REL_TOOLS_PATH="")

if(WIN32)
	add_definitions(-DUNICODE -DWIN32 -DWIN64 -D_CRT_SECURE_NO_WARNINGS)
endif()

# QtCreator sources
set(qt_creator_libs_src "../qt-creator/src/libs")

# LANGUAGEUTILS
file(GLOB_RECURSE languageutils_source 
	"${qt_creator_libs_src}/languageutils/*.cpp" 
	"${qt_creator_libs_src}/languageutils/*.h")

add_library(languageutils STATIC ${languageutils_source})
target_link_libraries(languageutils PRIVATE Qt6::Core)
target_compile_definitions(languageutils PUBLIC "LANGUAGEUTILS_STATIC_LIBRARY")

# UTILS
file(GLOB_RECURSE utils_source 
	"${qt_creator_libs_src}/utils/*.cpp" 
	"${qt_creator_libs_src}/utils/*.h")

file(GLOB_RECURSE utils_source_apple
	"${qt_creator_libs_src}/utils/*.mm"
	"${qt_creator_libs_src}/utils/*_mac*.h")

file(GLOB utils_source_non_apple
	"${qt_creator_libs_src}/utils/touchbar.cpp")

file(GLOB utils_source_console_process
	"${qt_creator_libs_src}/utils/consoleprocess_win.cpp"
	"${qt_creator_libs_src}/utils/consoleprocess_unix.cpp"
	"${qt_creator_libs_src}/utils/process_ctrlc_stub.cpp")

# Not needed, and platform dependent, so just remove them
list(REMOVE_ITEM utils_source ${utils_source_console_process})

If(APPLE)
	list(APPEND utils_source ${utils_source_apple})
	list(REMOVE_ITEM utils_source ${utils_source_non_apple})
else()
	list(REMOVE_ITEM utils_source ${utils_source_apple})
endif()

add_library(utils STATIC ${utils_source})
target_compile_definitions(utils PUBLIC UTILS_STATIC_LIBRARY RELATIVE_LIBEXEC_PATH=".")
target_include_directories(utils PRIVATE "${qt_creator_libs_src}" "${qt_creator_libs_src}/utils/" "${qt_creator_libs_src}/utils/mimetypes2/" ${Qt${QT_VERSION_MAJOR}Quick_PRIVATE_INCLUDE_DIRS})
target_link_libraries(utils PRIVATE Qt6::Widgets Qt6::Network Qt6::Concurrent Qt6::Qml Qt6::Xml Qt6::Core5Compat Qt6::CorePrivate)

if(WIN32)
	target_link_libraries(utils PRIVATE user32 iphlpapi ws2_32 shell32 dbghelp)
endif()

if (APPLE)
	find_library(AppKit AppKit)
	find_library(Foundation Foundation)
	
	target_link_libraries(utils PRIVATE ${AppKit} ${Foundation})
endif()

# CPLUSPLUS
file(GLOB_RECURSE cplusplus_source 
	"${qt_creator_libs_src}/cplusplus/*.cpp"
	"${qt_creator_libs_src}/cplusplus/*.h"
	"${qt_creator_libs_src}/3rdparty/cplusplus/*.cpp"
	"${qt_creator_libs_src}/3rdparty/cplusplus/*.h")

add_library(cplusplus STATIC ${cplusplus_source})
target_link_libraries(cplusplus PRIVATE Qt6::Widgets utils)
target_compile_definitions(cplusplus PUBLIC "CPLUSPLUS_BUILD_STATIC_LIB")
target_include_directories(cplusplus PRIVATE "${qt_creator_libs_src}/3rdparty/" "${qt_creator_libs_src}/")

# QMLJS
file(GLOB_RECURSE qmljs_source
	"${qt_creator_libs_src}/qmljs/*.cpp" 
	"${qt_creator_libs_src}/qmljs/*.h")

add_library(qmljs STATIC ${qmljs_source})
target_link_libraries(qmljs PRIVATE utils languageutils cplusplus Qt6::Widgets Qt6::Xml Qt6::Core5Compat)
target_compile_definitions(qmljs PUBLIC "QMLJS_STATIC_LIBRARY" "QT_CREATOR" "UTILS_STATIC_LIBRARY" "CPLUSPLUS_BUILD_STATIC_LIB" "LANGUAGEUTILS_STATIC_LIBRARY")
target_include_directories(qmljs PUBLIC "${qt_creator_libs_src}/3rdparty/" "${qt_creator_libs_src}/")
