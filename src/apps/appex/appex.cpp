/*******************************************************************************
    Copyright (C) 2023 Kevin Sahr

    This file is part of DGGRID.

    DGGRID is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    DGGRID is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*******************************************************************************/
////////////////////////////////////////////////////////////////////////////////
//
// appex.cpp: simple application that demonstrates using the dglib library to
//            manipulate DGG cells.
//
////////////////////////////////////////////////////////////////////////////////

#include <cstddef>
#include <iostream>

using namespace std;

#include "appex.h"
#include <dglib/DgIDGGS3H.h>
#include <dglib/DgIDGGS4H.h>
#include <dglib/DgIDGGS7H.h>
#include <geoarrow/geoarrow.hpp>
#include <limits.h>
#include <map>
#include <nanoarrow/nanoarrow.h>
#include <stdio.h>
#include <vector>
#include <Python.h>
////////////////////////////////////////////////////////////////////////////////

int doSomething(void) {
  ///// create the DGG /////

  // create the reference frame (RF) conversion network
  DgRFNetwork net0;

  // create the geodetic reference frame
  // reference frames must be created dynamically using makeRF
  // they will be deleted by the Network
  const DgGeoSphRF &geoRF = *(DgGeoSphRF::makeRF(net0, "GS0"));

  // create the ISEA4H grid system with resolutions 0-9; requires a
  // fixed icosahedron vertex and edge azimuth
  DgGeoCoord vert0(11.25L, 58.28252559L, false); // args: lon, lat, isRadians
  long double azimuth = 0.0L;

  // all DGGS's must be created using a factory makeRF method
  // the DGGS is memory managed by the DgRFNetwork
  const DgIDGGS4H *idggsPtr =
      DgIDGGS4H::makeRF(net0, geoRF, vert0, azimuth, 10);
  const DgIDGGS4H &idggs = *idggsPtr;

  // get the resolution 7 dgg from the dggs
  const DgIDGG &dgg = idggs.idgg(7);
  cout << dgg.gridStats() << endl;

  //////// now use the DGG /////////

  ///// given a point in lon/lat, get the cell it lies within /////

  // first create a DgLocation in geoRF coordinates
  DgGeoCoord geoAddress(-122.7083, 42.1947, false);
  DgLocation *thePt = geoRF.makeLocation(geoAddress);
  cout << "the point " << *thePt << endl;

  // converting the point location to the dgg RF determines which cell it's in
  dgg.convert(thePt);
  cout << "* lies in cell " << *thePt << endl;

  // we can get the cell's vertices, which are defined in geoRF
  DgPolygon verts;
  int ptsPerEdgeDensify = 3;
  dgg.setVertices(*thePt, verts, ptsPerEdgeDensify);
  cout << "* with densified cell boundary:\n" << verts << endl;

  // we can get the cell's center point by converting the cell back to geoRF
  geoRF.convert(thePt);
  cout << "* and cell center point:" << *thePt << endl;

  // we can extract the coordinates into primitive data types
  const DgGeoCoord &centCoord = *geoRF.getAddress(*thePt);
  double latRads = centCoord.lat();
  double lonRads = centCoord.lon();
  cout << "* center point lon,lat in radians: " << lonRads << ", " << latRads
       << endl;

  const DgGeoCoord &firstVert = *geoRF.getAddress(verts[0]);
  double latDegs = firstVert.latDegs();
  double lonDegs = firstVert.lonDegs();
  cout << "* first boundary vertex lon,lat in degrees: " << lonDegs << ", "
       << latDegs << endl;

  delete thePt;

  return 0;
}
std::size_t current_key = 0;
std::map<std::size_t, DgRFNetwork> networks;
std::map<std::size_t, const DgHexIDGGS *> hexes;
std::map<std::size_t, const DgGeoSphRF *> geoSpheres;
std::map<std::size_t, const DgIDGG *> dgIDGG;
std::map<std::size_t, vector<std::size_t>> hexesOwner;
std::map<std::size_t, vector<std::size_t>> geoSpheresOwner;
std::map<std::size_t, vector<std::size_t>> dgIDGGOwner;

extern "C" {

enum Resoulution { ISEA3H = 1, ISEA4H = 2, ISEA7H = 3 };
std::size_t makeKey() {
  std::size_t newkey;
  current_key = current_key + 1;
  newkey = current_key;
  return newkey;
}
std::size_t makeNetwork() {
  std::size_t key = makeKey();
  networks[key] = DgRFNetwork();
  return key;
}
void destroyNetwork(std::size_t key) {
  networks.erase(key);
  for (const int &i : hexesOwner[key]) {
    for (const int &j : dgIDGGOwner[i])
      dgIDGG.erase(j);
    hexes.erase(i);
  }
  for (const int &i : geoSpheresOwner[key])
    geoSpheres.erase(i);
}
std::size_t makeGeoSphere(std::size_t networkKey, char *type) {
  std::size_t key = makeKey();
  geoSpheres[key] = DgGeoSphRF::makeRF(networks[networkKey], type);
  geoSpheresOwner[networkKey].push_back(key);
  return key;
}
std::size_t makeHex(std::size_t networkKey, std::size_t geoSphereKey,
                    Resoulution resolution, long double lon, long double lat,
                    bool rads, long double azimuth, int res) {
  std::size_t key = makeKey();
  switch (resolution) {
  case ISEA3H:
    hexes[key] =
        DgIDGGS3H::makeRF(networks[networkKey], *geoSpheres[geoSphereKey],
                          DgGeoCoord(lon, lat, rads), azimuth, res);
    break;
  case ISEA4H:
    hexes[key] =
        DgIDGGS4H::makeRF(networks[networkKey], *geoSpheres[geoSphereKey],
                          DgGeoCoord(lon, lat, rads), azimuth, res);
    break;
  case ISEA7H:
    hexes[key] =
        DgIDGGS7H::makeRF(networks[networkKey], *geoSpheres[geoSphereKey],
                          DgGeoCoord(lon, lat, rads), azimuth, res);
    break;
  }
  hexesOwner[networkKey].push_back(key);
  return key;
}
std::size_t makeIDGG(std::size_t hexKey, int resolution) {
  std::size_t key = makeKey();
  const DgHexIDGGS *idggsPtr = hexes[hexKey];
  const DgHexIDGGS &idggs = *idggsPtr;
  const DgIDGG &idgg = idggs.idgg(7);
  dgIDGGOwner[hexKey].push_back(key);
  dgIDGG[key] = &idgg;
  return key;
}
unsigned long long int nCells(std::size_t idggkey) {
  return dgIDGG[idggkey]->gridStats().nCells();
}
long double cls(std::size_t idggkey) {
  return dgIDGG[idggkey]->gridStats().cls();
}
long double cellAreaKM(std::size_t idggkey) {
  return dgIDGG[idggkey]->gridStats().cellAreaKM();
}
unsigned long long int doSomething2(void) {
  ///// create the DGG /////

  // create the reference frame (RF) conversion network
  size_t net0Key = makeNetwork();

  // create the geodetic reference frame
  // reference frames must be created dynamically using makeRF
  // they will be deleted by the Network
  size_t geoSphereKey = makeGeoSphere(net0Key, "GS0");

  // all DGGS's must be created using a factory makeRF method
  // the DGGS is memory managed by the DgRFNetwork
  size_t hexKey = makeHex(net0Key, geoSphereKey, ISEA4H, 11.25L, 58.28252559L,
                          false, 0, 10);

  // get the resolution 7 dgg from the dggs
  size_t idggkey = makeIDGG(hexKey, 7);
  // const DgIDGG &dgg = dgIDGG[idggkey];
  //  cout << dgg.gridStats() << endl;

  return nCells(idggkey);
}
void process_arrow_table(struct ArrowSchema *schema, struct ArrowArray *array) {
  // Process the Arrow data
  printf("Received table with %ld columns, %ld rows\n", schema->n_children,
         array->length);

  // Print data contents
  for (int i = 0; i < array->length; i++) {
    // Access and print row data
    printf("Row %d: ...\n", i);
  }
}

void process_arrow_table2(struct ArrowSchema *schema,
                          struct ArrowArray **arrays, int len) {
  for (int i = 0; i < 3; i++) {
    struct ArrowArray *array = arrays[i];
    // Process the Arrow data
    printf("Received table with %ld columns, %ld rows\n", schema->n_children,
           array->length);

    // Print data contents
    for (int i = 0; i < array->length; i++) {
      // Access and print row data
      printf("Row %d: ...\n", i);
    }
  }
}

void inspect_with_nanoarrow(struct ArrowArray *array,
                            struct ArrowSchema *schema) {
  struct ArrowArrayView array_view;
  struct ArrowSchemaView schema_view;

  printf("Length: %ld\n", array_view.array->length);
  // printf("Schema format: %s\n", schema->format);

  // For string-based types (like WKT/WKB), print data as strings
  if (array_view.storage_type == NANOARROW_TYPE_STRING ||
      array_view.storage_type == NANOARROW_TYPE_BINARY) {
    for (int64_t i = 0; i < array_view.array->length; i++) {
      if (ArrowArrayViewIsNull(&array_view, i)) {
        printf("Row %ld: NULL\n", i);
      } else {
        struct ArrowBufferView view;
        ArrowArrayViewGetBytesUnsafe(&array_view, i);
        printf("Row %ld: %.*s\n", i, (int)view.size_bytes,
               (const char *)view.data.data);
      }
    }
  } else {
    printf("Unsupported type for printing\n");
  }
}

#include <nanoarrow/nanoarrow.h>
#include <stdio.h>
#define MAX_WKT_LEN 512

static struct ArrowError global_error;

void print_arrow_array_view(struct ArrowArrayView *array_view) {
  if (!array_view) {
    printf("Invalid ArrowArrayView\n");
    return;
  }

  printf("Array length: %lld\n", (long long)array_view->length);
  printf("Number of buffers: %d\n", array_view->array->n_buffers);

  // Iterate over buffers manually
  for (int i = 0; i <= array_view->array->n_buffers; i++) {
    if (array_view->array->buffers[i] != NULL) {
      printf("Buffer %d: %p\n", i, array_view->array->buffers[i]);

      // Example: Print first few bytes (assuming int32 data in buffer 1)
      if (i == 1) {
        int32_t *data = (int32_t *)array_view->array->buffers[i];
        printf("First value: %d\n", data[0]);

        for (int64_t j = 0; j < array_view->length; j++) {
          printf("%d ", data[j]);
        }
        printf("\n");
      }
    } else {
      printf("Buffer %d is NULL\n", i);
    }
  }
}
  void print_arrow_table7(PyObject * capsule) {
    struct ArrowArrayStream *stream =
        (struct ArrowArrayStream *)PyCapsule_GetPointer(capsule,
                                                        "arrow_array_stream");
    if (!stream) {
      printf("Failed to extract ArrowArrayStream\n");
      return;
    }

    // Retrieve schema
    struct ArrowSchema schema;
    if (stream->get_schema(stream, &schema) == 0) {
      printf("Schema format: %s\n", schema.format);
    }

    // Iterate over stream
    struct ArrowArray array;
    while (stream->get_next(stream, &array) == 0 ) {
      printf("Chunk with %lld rows:\n", array.length);

      // Example: Print first buffer (assuming int32 data)
      //if (array.n_buffers > 1 && array.buffers[1] != NULL) {
        ArrowArray *data = (ArrowArray *)array.buffers[1];
        printf("First value: %d\n", data[0]);

        for (int64_t i = 0; i < array.length; i++) {
          printf("%d ", data[i]);
        }
        printf("\n");
     // }

      if (array.release == NULL ) 
        break;
      // Release chunk
      array.release(&array);
    }
    // Release schema and stream
    schema.release(&schema);
    stream->release(stream);
  }
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
