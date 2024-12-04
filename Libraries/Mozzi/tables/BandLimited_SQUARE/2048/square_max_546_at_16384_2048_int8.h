#ifndef SQUARE_MAX_546_AT_16384_2048INT8_H_
#define SQUARE_MAX_546_AT_16384_2048INT8_H_

#include <Arduino.h>
#include "mozzi_pgmspace.h"


#define SQUARE_MAX_546_AT_16384_2048_NUM_CELLS 2048
#define SQUARE_MAX_546_AT_16384_2048_SAMPLERATE 2048

CONSTTABLE_STORAGE(int8_t) SQUARE_MAX_546_AT_16384_2048_DATA [] = 
{
0, 3, 6, 10, 13, 16, 20, 23, 26, 30, 33, 36, 39, 42, 46, 49, 52, 55, 58, 61, 64, 66, 69, 72, 75, 77, 80, 82, 85, 87, 89, 92, 94, 
96, 98, 100, 102, 104, 105, 107, 109, 110, 112, 113, 115, 116, 117, 118, 119, 120, 121, 122, 123, 123, 124, 125, 125, 126, 126, 126, 127, 127, 127, 127, 127, 
127, 127, 127, 127, 126, 126, 126, 125, 125, 125, 124, 124, 123, 123, 122, 121, 121, 120, 119, 119, 118, 117, 117, 116, 115, 114, 114, 113, 112, 112, 111, 110, 
109, 109, 108, 107, 107, 106, 105, 105, 104, 103, 103, 102, 102, 101, 101, 100, 100, 99, 99, 99, 98, 98, 98, 98, 97, 97, 97, 97, 97, 97, 97, 97, 
97, 97, 97, 97, 97, 97, 97, 98, 98, 98, 98, 99, 99, 99, 99, 100, 100, 101, 101, 101, 102, 102, 103, 103, 103, 104, 104, 105, 105, 106, 106, 107, 
107, 107, 108, 108, 109, 109, 110, 110, 110, 111, 111, 111, 112, 112, 112, 113, 113, 113, 114, 114, 114, 114, 114, 115, 115, 115, 115, 115, 115, 115, 115, 115, 
115, 115, 115, 115, 115, 115, 115, 115, 114, 114, 114, 114, 114, 113, 113, 113, 113, 112, 112, 112, 112, 111, 111, 111, 110, 110, 110, 109, 109, 109, 108, 108, 
108, 107, 107, 107, 106, 106, 106, 105, 105, 105, 105, 104, 104, 104, 104, 103, 103, 103, 103, 103, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 
102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 103, 103, 103, 103, 103, 103, 104, 104, 104, 104, 105, 105, 105, 105, 106, 106, 106, 106, 107, 107, 107, 
108, 108, 108, 108, 109, 109, 109, 109, 110, 110, 110, 110, 111, 111, 111, 111, 111, 111, 112, 112, 112, 112, 112, 112, 112, 112, 113, 113, 113, 113, 113, 113, 
113, 113, 113, 113, 113, 112, 112, 112, 112, 112, 112, 112, 112, 112, 111, 111, 111, 111, 111, 110, 110, 110, 110, 110, 109, 109, 109, 109, 108, 108, 108, 108, 
107, 107, 107, 107, 107, 106, 106, 106, 106, 105, 105, 105, 105, 105, 104, 104, 104, 104, 104, 104, 104, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 
103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 104, 104, 104, 104, 104, 104, 104, 105, 105, 105, 105, 105, 106, 106, 106, 106, 106, 107, 107, 107, 107, 107, 
108, 108, 108, 108, 109, 109, 109, 109, 109, 110, 110, 110, 110, 110, 110, 111, 111, 111, 111, 111, 111, 111, 111, 112, 112, 112, 112, 112, 112, 112, 112, 112, 
112, 112, 112, 112, 112, 112, 112, 112, 111, 111, 111, 111, 111, 111, 111, 111, 110, 110, 110, 110, 110, 110, 109, 109, 109, 109, 109, 108, 108, 108, 108, 108, 
107, 107, 107, 107, 107, 106, 106, 106, 106, 106, 105, 105, 105, 105, 105, 105, 104, 104, 104, 104, 104, 104, 104, 104, 103, 103, 103, 103, 103, 103, 103, 103, 
103, 103, 103, 103, 103, 103, 103, 104, 104, 104, 104, 104, 104, 104, 104, 105, 105, 105, 105, 105, 105, 106, 106, 106, 106, 106, 107, 107, 107, 107, 107, 108, 
108, 108, 108, 108, 109, 109, 109, 109, 109, 110, 110, 110, 110, 110, 110, 111, 111, 111, 111, 111, 111, 111, 111, 112, 112, 112, 112, 112, 112, 112, 112, 112, 
112, 112, 112, 112, 112, 112, 112, 112, 111, 111, 111, 111, 111, 111, 111, 111, 110, 110, 110, 110, 110, 110, 109, 109, 109, 109, 109, 108, 108, 108, 108, 107, 
107, 107, 107, 107, 106, 106, 106, 106, 106, 105, 105, 105, 105, 105, 104, 104, 104, 104, 104, 104, 104, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 
103, 103, 103, 103, 103, 103, 103, 103, 103, 103, 104, 104, 104, 104, 104, 104, 104, 105, 105, 105, 105, 105, 106, 106, 106, 106, 107, 107, 107, 107, 107, 108, 
108, 108, 108, 109, 109, 109, 109, 110, 110, 110, 110, 110, 111, 111, 111, 111, 111, 112, 112, 112, 112, 112, 112, 112, 112, 112, 113, 113, 113, 113, 113, 113, 
113, 113, 113, 113, 113, 112, 112, 112, 112, 112, 112, 112, 112, 111, 111, 111, 111, 111, 111, 110, 110, 110, 110, 109, 109, 109, 109, 108, 108, 108, 108, 107, 
107, 107, 106, 106, 106, 106, 105, 105, 105, 105, 104, 104, 104, 104, 103, 103, 103, 103, 103, 103, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 
102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 102, 103, 103, 103, 103, 103, 104, 104, 104, 104, 105, 105, 105, 105, 106, 106, 106, 107, 107, 107, 108, 108, 
108, 109, 109, 109, 110, 110, 110, 111, 111, 111, 112, 112, 112, 112, 113, 113, 113, 113, 114, 114, 114, 114, 114, 115, 115, 115, 115, 115, 115, 115, 115, 115, 
115, 115, 115, 115, 115, 115, 115, 115, 114, 114, 114, 114, 114, 113, 113, 113, 112, 112, 112, 111, 111, 111, 110, 110, 110, 109, 109, 108, 108, 107, 107, 107, 
106, 106, 105, 105, 104, 104, 103, 103, 103, 102, 102, 101, 101, 101, 100, 100, 99, 99, 99, 99, 98, 98, 98, 98, 97, 97, 97, 97, 97, 97, 97, 97, 
97, 97, 97, 97, 97, 97, 97, 98, 98, 98, 98, 99, 99, 99, 100, 100, 101, 101, 102, 102, 103, 103, 104, 105, 105, 106, 107, 107, 108, 109, 109, 110, 
111, 112, 112, 113, 114, 114, 115, 116, 117, 117, 118, 119, 119, 120, 121, 121, 122, 123, 123, 124, 124, 125, 125, 125, 126, 126, 126, 127, 127, 127, 127, 127, 
127, 127, 127, 127, 126, 126, 126, 125, 125, 124, 123, 123, 122, 121, 120, 119, 118, 117, 116, 115, 113, 112, 110, 109, 107, 105, 104, 102, 100, 98, 96, 94, 
92, 89, 87, 85, 82, 80, 77, 75, 72, 69, 66, 64, 61, 58, 55, 52, 49, 46, 42, 39, 36, 33, 30, 26, 23, 20, 16, 13, 10, 6, 3, 0, 
-3, -6, -10, -13, -16, -20, -23, -26, -30, -33, -36, -39, -42, -46, -49, -52, -55, -58, -61, -64, -66, -69, -72, -75, -77, -80, -82, -85, -87, -89, -92, -94, 
-96, -98, -100, -102, -104, -105, -107, -109, -110, -112, -113, -115, -116, -117, -118, -119, -120, -121, -122, -123, -123, -124, -125, -125, -126, -126, -126, -127, -127, -127, -127, -127, 
-127, -127, -127, -127, -126, -126, -126, -125, -125, -125, -124, -124, -123, -123, -122, -121, -121, -120, -119, -119, -118, -117, -117, -116, -115, -114, -114, -113, -112, -112, -111, -110, 
-109, -109, -108, -107, -107, -106, -105, -105, -104, -103, -103, -102, -102, -101, -101, -100, -100, -99, -99, -99, -98, -98, -98, -98, -97, -97, -97, -97, -97, -97, -97, -97, 
-97, -97, -97, -97, -97, -97, -97, -98, -98, -98, -98, -99, -99, -99, -99, -100, -100, -101, -101, -101, -102, -102, -103, -103, -103, -104, -104, -105, -105, -106, -106, -107, 
-107, -107, -108, -108, -109, -109, -110, -110, -110, -111, -111, -111, -112, -112, -112, -113, -113, -113, -114, -114, -114, -114, -114, -115, -115, -115, -115, -115, -115, -115, -115, -115, 
-115, -115, -115, -115, -115, -115, -115, -115, -114, -114, -114, -114, -114, -113, -113, -113, -113, -112, -112, -112, -112, -111, -111, -111, -110, -110, -110, -109, -109, -109, -108, -108, 
-108, -107, -107, -107, -106, -106, -106, -105, -105, -105, -105, -104, -104, -104, -104, -103, -103, -103, -103, -103, -102, -102, -102, -102, -102, -102, -102, -102, -102, -102, -102, -102, 
-102, -102, -102, -102, -102, -102, -102, -102, -102, -102, -102, -103, -103, -103, -103, -103, -103, -104, -104, -104, -104, -105, -105, -105, -105, -106, -106, -106, -106, -107, -107, -107, 
-108, -108, -108, -108, -109, -109, -109, -109, -110, -110, -110, -110, -111, -111, -111, -111, -111, -111, -112, -112, -112, -112, -112, -112, -112, -112, -113, -113, -113, -113, -113, -113, 
-113, -113, -113, -113, -113, -112, -112, -112, -112, -112, -112, -112, -112, -112, -111, -111, -111, -111, -111, -110, -110, -110, -110, -110, -109, -109, -109, -109, -108, -108, -108, -108, 
-107, -107, -107, -107, -107, -106, -106, -106, -106, -105, -105, -105, -105, -105, -104, -104, -104, -104, -104, -104, -104, -103, -103, -103, -103, -103, -103, -103, -103, -103, -103, -103, 
-103, -103, -103, -103, -103, -103, -103, -103, -103, -103, -104, -104, -104, -104, -104, -104, -104, -105, -105, -105, -105, -105, -106, -106, -106, -106, -106, -107, -107, -107, -107, -107, 
-108, -108, -108, -108, -109, -109, -109, -109, -109, -110, -110, -110, -110, -110, -110, -111, -111, -111, -111, -111, -111, -111, -111, -112, -112, -112, -112, -112, -112, -112, -112, -112, 
-112, -112, -112, -112, -112, -112, -112, -112, -111, -111, -111, -111, -111, -111, -111, -111, -110, -110, -110, -110, -110, -110, -109, -109, -109, -109, -109, -108, -108, -108, -108, -108, 
-107, -107, -107, -107, -107, -106, -106, -106, -106, -106, -105, -105, -105, -105, -105, -105, -104, -104, -104, -104, -104, -104, -104, -104, -103, -103, -103, -103, -103, -103, -103, -103, 
-103, -103, -103, -103, -103, -103, -103, -104, -104, -104, -104, -104, -104, -104, -104, -105, -105, -105, -105, -105, -105, -106, -106, -106, -106, -106, -107, -107, -107, -107, -107, -108, 
-108, -108, -108, -108, -109, -109, -109, -109, -109, -110, -110, -110, -110, -110, -110, -111, -111, -111, -111, -111, -111, -111, -111, -112, -112, -112, -112, -112, -112, -112, -112, -112, 
-112, -112, -112, -112, -112, -112, -112, -112, -111, -111, -111, -111, -111, -111, -111, -111, -110, -110, -110, -110, -110, -110, -109, -109, -109, -109, -109, -108, -108, -108, -108, -107, 
-107, -107, -107, -107, -106, -106, -106, -106, -106, -105, -105, -105, -105, -105, -104, -104, -104, -104, -104, -104, -104, -103, -103, -103, -103, -103, -103, -103, -103, -103, -103, -103, 
-103, -103, -103, -103, -103, -103, -103, -103, -103, -103, -104, -104, -104, -104, -104, -104, -104, -105, -105, -105, -105, -105, -106, -106, -106, -106, -107, -107, -107, -107, -107, -108, 
-108, -108, -108, -109, -109, -109, -109, -110, -110, -110, -110, -110, -111, -111, -111, -111, -111, -112, -112, -112, -112, -112, -112, -112, -112, -112, -113, -113, -113, -113, -113, -113, 
-113, -113, -113, -113, -113, -112, -112, -112, -112, -112, -112, -112, -112, -111, -111, -111, -111, -111, -111, -110, -110, -110, -110, -109, -109, -109, -109, -108, -108, -108, -108, -107, 
-107, -107, -106, -106, -106, -106, -105, -105, -105, -105, -104, -104, -104, -104, -103, -103, -103, -103, -103, -103, -102, -102, -102, -102, -102, -102, -102, -102, -102, -102, -102, -102, 
-102, -102, -102, -102, -102, -102, -102, -102, -102, -102, -102, -103, -103, -103, -103, -103, -104, -104, -104, -104, -105, -105, -105, -105, -106, -106, -106, -107, -107, -107, -108, -108, 
-108, -109, -109, -109, -110, -110, -110, -111, -111, -111, -112, -112, -112, -112, -113, -113, -113, -113, -114, -114, -114, -114, -114, -115, -115, -115, -115, -115, -115, -115, -115, -115, 
-115, -115, -115, -115, -115, -115, -115, -115, -114, -114, -114, -114, -114, -113, -113, -113, -112, -112, -112, -111, -111, -111, -110, -110, -110, -109, -109, -108, -108, -107, -107, -107, 
-106, -106, -105, -105, -104, -104, -103, -103, -103, -102, -102, -101, -101, -101, -100, -100, -99, -99, -99, -99, -98, -98, -98, -98, -97, -97, -97, -97, -97, -97, -97, -97, 
-97, -97, -97, -97, -97, -97, -97, -98, -98, -98, -98, -99, -99, -99, -100, -100, -101, -101, -102, -102, -103, -103, -104, -105, -105, -106, -107, -107, -108, -109, -109, -110, 
-111, -112, -112, -113, -114, -114, -115, -116, -117, -117, -118, -119, -119, -120, -121, -121, -122, -123, -123, -124, -124, -125, -125, -125, -126, -126, -126, -127, -127, -127, -127, -127, 
-127, -127, -127, -127, -126, -126, -126, -125, -125, -124, -123, -123, -122, -121, -120, -119, -118, -117, -116, -115, -113, -112, -110, -109, -107, -105, -104, -102, -100, -98, -96, -94, 
-92, -89, -87, -85, -82, -80, -77, -75, -72, -69, -66, -64, -61, -58, -55, -52, -49, -46, -42, -39, -36, -33, -30, -26, -23, -20, -16, -13, -10, -6, -3, 
 };

#endif