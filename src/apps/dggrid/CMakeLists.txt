cmake_minimum_required (VERSION 3.20)


set(DGGRID_SOURCES
   DgHexSF.cpp
   OpBasic.cpp
   SubOpBasic.cpp
   SubOpBasicMulti.cpp
   SubOpBinPts.cpp
   SubOpDGG.cpp
   SubOpGen.cpp
   SubOpGenHelper.cpp
   SubOpIn.cpp
   SubOpMain.cpp
   SubOpOut.cpp
   SubOpStats.cpp
   SubOpTransform.cpp
   clipper.cpp
   dggrid.cpp
)


add_executable(dggrid
${DGGRID_SOURCES}
)

add_library(dggridlib SHARED
${DGGRID_SOURCES}   
)

if(GDAL_FOUND)
  target_compile_definitions(dggrid PRIVATE USE_GDAL)
  target_compile_definitions(dggridlib PRIVATE USE_GDAL)
endif()

# If GDAL isn't present than ${GDAL_LIBRARIES} gives an empty string
target_link_libraries(dggrid PRIVATE dglib dgaplib shapelib ${GDAL_LIBRARIES})
target_compile_features(dggrid PRIVATE cxx_std_11)
target_link_libraries(dggridlib PRIVATE dglib dgaplib shapelib ${GDAL_LIBRARIES})
target_compile_features(dggridlib PRIVATE cxx_std_11)

