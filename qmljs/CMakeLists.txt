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

find_package(Qt6 REQUIRED Core5Compat Xml Network Widgets Core)

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
set(utils_source 
	"${qt_creator_libs_src}/utils/changeset.cpp"
	"${qt_creator_libs_src}/utils/commandline.cpp"
	"${qt_creator_libs_src}/utils/devicefileaccess.cpp"
	"${qt_creator_libs_src}/utils/environment.cpp"
	"${qt_creator_libs_src}/utils/filepath.cpp"
	"${qt_creator_libs_src}/utils/filesystemwatcher.cpp"
	"${qt_creator_libs_src}/utils/fileutils.cpp"
	"${qt_creator_libs_src}/utils/fsengine/fileiconprovider.cpp"
	"${qt_creator_libs_src}/utils/fsengine/fsengine.cpp"
	"${qt_creator_libs_src}/utils/futuresynchronizer.cpp"
	"${qt_creator_libs_src}/utils/globalfilechangeblocker.cpp"
	"${qt_creator_libs_src}/utils/guard.cpp"
	"${qt_creator_libs_src}/utils/hostosinfo.cpp"
	"${qt_creator_libs_src}/utils/json.cpp"
	"${qt_creator_libs_src}/utils/launcherinterface.cpp"
	"${qt_creator_libs_src}/utils/launcherpackets.cpp"
	"${qt_creator_libs_src}/utils/launchersocket.cpp"
	"${qt_creator_libs_src}/utils/macroexpander.cpp"
	"${qt_creator_libs_src}/utils/mimetypes2/mimedatabase.cpp"
	"${qt_creator_libs_src}/utils/mimetypes2/mimeglobpattern.cpp"
	"${qt_creator_libs_src}/utils/mimetypes2/mimemagicrule.cpp"
	"${qt_creator_libs_src}/utils/mimetypes2/mimemagicrulematcher.cpp"
	"${qt_creator_libs_src}/utils/mimetypes2/mimeprovider.cpp"
	"${qt_creator_libs_src}/utils/mimetypes2/mimetype.cpp"
	"${qt_creator_libs_src}/utils/mimetypes2/mimetypeparser.cpp"
	"${qt_creator_libs_src}/utils/mimetypes2/mimeutils.cpp"
	"${qt_creator_libs_src}/utils/namevaluedictionary.cpp"
	"${qt_creator_libs_src}/utils/namevalueitem.cpp"
	"${qt_creator_libs_src}/utils/processinterface.cpp"
	"${qt_creator_libs_src}/utils/processreaper.cpp"
	"${qt_creator_libs_src}/utils/processutils.cpp"
	"${qt_creator_libs_src}/utils/qrcparser.cpp"
	"${qt_creator_libs_src}/utils/qtcassert.cpp"
	"${qt_creator_libs_src}/utils/qtcprocess.cpp"
	"${qt_creator_libs_src}/utils/runextensions.cpp"
	"${qt_creator_libs_src}/utils/savefile.cpp"
	"${qt_creator_libs_src}/utils/singleton.cpp"
	"${qt_creator_libs_src}/utils/stringutils.cpp"
	"${qt_creator_libs_src}/utils/tasktree.cpp"
	"${qt_creator_libs_src}/utils/temporarydirectory.cpp"
	"${qt_creator_libs_src}/utils/terminalprocess.cpp"
	"${qt_creator_libs_src}/utils/terminalcommand.cpp"
	"${qt_creator_libs_src}/utils/textutils.cpp"
	"${qt_creator_libs_src}/utils/threadutils.cpp"
	"${qt_creator_libs_src}/utils/utilsicons.cpp"
	"${qt_creator_libs_src}/utils/winutils.cpp"
	)

add_library(utils STATIC ${utils_source})
target_compile_definitions(utils PUBLIC UTILS_STATIC_LIBRARY RELATIVE_LIBEXEC_PATH=".")
target_include_directories(utils PRIVATE "${qt_creator_libs_src}" "${qt_creator_libs_src}/utils/" "${qt_creator_libs_src}/utils/mimetypes2/")
target_link_libraries(utils PRIVATE Qt6::Widgets Qt6::Network Qt6::Xml Qt6::Core5Compat)

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