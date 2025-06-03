cmake_minimum_required (VERSION 3.20)

add_executable(appex
appex.h
appex.cpp
appexmain.cpp)

add_library(appexlib
appex.h
appex.cpp)

target_link_libraries(appex PRIVATE dglib)
target_compile_features(appex PRIVATE cxx_std_11)
target_link_libraries(appexlib PRIVATE dglib)
target_compile_features(appexlib PRIVATE cxx_std_11)
