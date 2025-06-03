cmake_minimum_required (VERSION 3.20)

<<<<<<< Updated upstream
add_executable(dggrid
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
=======
add_library(dggrid
  binpres.cpp
  binvals.cpp
  clipper.cpp
  dggrid.cpp
  DgHexSF.cpp
  gridgen.cpp
  param.cpp
  quadclip.cpp
  table.cpp
  transform.cpp
  util.cpp
>>>>>>> Stashed changes
)

if(GDAL_FOUND)
  target_compile_definitions(dggrid PRIVATE USE_GDAL)
endif()

# If GDAL isn't present than ${GDAL_LIBRARIES} gives an empty string
<<<<<<< Updated upstream
target_link_libraries(dggrid PRIVATE dglib dgaplib shapelib ${GDAL_LIBRARIES})
target_compile_features(dggrid PRIVATE cxx_std_11)
=======
# target_link_libraries(dggrid PRIVATE dglib shapelib ${GDAL_LIBRARIES})
# target_compile_features(dggrid PRIVATE cxx_std_11)

target_link_libraries(dggrid PRIVATE dglib)
target_include_directories(dggrid PRIVATE ../../lib/dglib/include)
target_link_libraries(dggrid PRIVATE shapelib)
target_include_directories(dggrid PRIVATE ../../lib/shapelib/include/shapelib)
>>>>>>> Stashed changes
