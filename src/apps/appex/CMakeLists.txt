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
find_package(Python3 REQUIRED COMPONENTS Development)
target_include_directories(appex PUBLIC ${Python3_INCLUDE_DIRS} ${CMAKE_BINARY_DIR}/geoarrow-c/src/ ${CMAKE_BINARY_DIR}/geoarrow-c/generated/ ${CMAKE_BINARY_DIR}/geoarrow-c/src/vendor/nanoarrow DGGRID/src/apps/appex  )
target_include_directories(appexlib PUBLIC ${Python3_INCLUDE_DIRS}  ${CMAKE_BINARY_DIR}/geoarrow-c/src/ ${CMAKE_BINARY_DIR}/geoarrow-c/generated/ ${CMAKE_BINARY_DIR}/geoarrow-c/src/vendor/nanoarrow DGGRID/src/apps/appex )

target_link_libraries(appex PUBLIC dglib ${Python3_LIBRARIES} ${CMAKE_BINARY_DIR}/geoarrow-c/libgeoarrow.so ${CMAKE_BINARY_DIR}/geoarrow-c/libgeoarrow_nanoarrow.a)
target_compile_features(appex PUBLIC cxx_std_11)
target_link_libraries(appexlib PUBLIC dglib ${Python3_LIBRARIES} ${CMAKE_BINARY_DIR}/geoarrow-c/libgeoarrow.so ${CMAKE_BINARY_DIR}/geoarrow-c/libgeoarrow_nanoarrow.a)
target_compile_features(appexlib PUBLIC cxx_std_11)