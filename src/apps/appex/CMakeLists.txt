cmake_minimum_required (VERSION 3.20)

#add_executable(appex appex.cpp)
add_library(appex STATIC
apexFuncs.h
apexFuncs.cpp)
add_subdirectory(${CMAKE_SOURCE_DIR}/DGGRID/src/lib/dglib dglib)
add_subdirectory(${CMAKE_SOURCE_DIR}/DGGRID/src/lib/shapelib shapelib)
add_subdirectory(${CMAKE_SOURCE_DIR}/DGGRID/src/lib/proj4lib proj4lib)
target_link_libraries(appex PRIVATE dglib)
target_compile_features(appex PRIVATE cxx_std_11)
