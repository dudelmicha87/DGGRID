cmake_minimum_required (VERSION 3.20)

set(APPEX_SOURCES
	appex.h
	appex.cpp
)


add_executable(appex
${APPEX_SOURCES}
appexmain.cpp)

add_library(appexlib SHARED
${APPEX_SOURCES}
)

target_link_libraries(appex PUBLIC dglib)
target_compile_features(appex PUBLIC cxx_std_11)
target_link_libraries(appexlib PUBLIC dglib)
target_compile_features(appexlib PUBLIC cxx_std_11)
