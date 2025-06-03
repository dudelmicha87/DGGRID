cmake_minimum_required (VERSION 3.20)

set(APPEX_SOURCES
	appex.h
	appex.cpp
)


add_executable(appex
${APPEX_SOURCES}
appexmain.cpp)

add_library(appexlib
${APPEX_SOURCES}
)

target_link_libraries(appex PRIVATE dglib)
target_compile_features(appex PRIVATE cxx_std_11)
target_link_libraries(appexlib PRIVATE dglib)
target_compile_features(appexlib PRIVATE cxx_std_11)
