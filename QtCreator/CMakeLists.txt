cmake_minimum_required(VERSION 3.0)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "C:/Qt/5.3/msvc2013_64" CACHE PATH "Qt 5 install dir")

find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Concurrent REQUIRED)
find_package(Qt5Script REQUIRED)
find_package(Qt5Xml REQUIRED)

add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x040900 -DQT_CREATOR)

if(WIN32)
	add_definitions(-DUNICODE -DWIN32 -DWIN64 -D_CRT_SECURE_NO_WARNINGS)
endif()

if(CMAKE_COMPILER_IS_GNUCXX)
	add_definitions(-std=c++11)
endif()

# LANGUAGEUTILS
file(GLOB_RECURSE languageutils_source 
	"src/libs/languageutils/*.cpp" 
	"src/libs/languageutils/*.h")

add_library(languageutils STATIC ${languageutils_source})
target_link_libraries(languageutils PRIVATE Qt5::Core)
target_compile_definitions(languageutils PUBLIC "LANGUAGEUTILS_BUILD_STATIC_LIB")

# UTILS
file(GLOB_RECURSE utils_source 
	"src/libs/utils/*.cpp" 
	"src/libs/utils/*.h")

file(GLOB utils_source_win "src/libs/utils/consoleprocess_win.cpp" "src/libs/utils/process_ctrlc_stub.cpp")
file(GLOB utils_source_unix "src/libs/utils/consoleprocess_unix.cpp")

if(WIN32)
	list(REMOVE_ITEM utils_source ${utils_source_unix})
else()
	list(REMOVE_ITEM utils_source ${utils_source_win})
endif()

add_library(utils STATIC ${utils_source})
target_compile_definitions(utils PUBLIC "QTCREATOR_UTILS_STATIC_LIB")
target_include_directories(utils PRIVATE "src/libs/")
target_link_libraries(utils PRIVATE Qt5::Widgets Qt5::Network Qt5::Script Qt5::Concurrent)
if(WIN32)
	target_link_libraries(utils PRIVATE user32 iphlpapi ws2_32 shell32)
endif()

# CPLUSPLUS
file(GLOB_RECURSE cplusplus_source 
	"src/libs/cplusplus/*.cpp"
	"src/libs/cplusplus/*.h"
	"src/libs/3rdparty/cplusplus/*.cpp"
	"src/libs/3rdparty/cplusplus/*.h")

add_library(cplusplus STATIC ${cplusplus_source})
target_link_libraries(cplusplus PRIVATE Qt5::Widgets utils)
target_compile_definitions(cplusplus PUBLIC "CPLUSPLUS_BUILD_LIB")
target_include_directories(cplusplus PRIVATE "src/libs/3rdparty/" "src/libs/")

# QMLJS
file(GLOB_RECURSE qmljs_source
	"src/libs/qmljs/*.cpp" 
	"src/libs/qmljs/*.h")

add_library(qmljs STATIC ${qmljs_source})
target_link_libraries(qmljs PRIVATE utils languageutils cplusplus Qt5::Widgets Qt5::Script Qt5::Xml)
target_compile_definitions(qmljs PRIVATE "QML_BUILD_STATIC_LIB")
target_include_directories(qmljs PRIVATE "src/libs/3rdparty/" "src/libs/")