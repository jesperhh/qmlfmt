set(sources
	diff_match_patch.cpp
	diff_match_patch.h
	)

add_library(diff_match_patch STATIC ${sources})

target_link_libraries(diff_match_patch Qt5::Core)
target_include_directories(diff_match_patch PUBLIC .)
target_compile_definitions(diff_match_patch PRIVATE "QT_DISABLE_DEPRECATED_BEFORE=0x040900")