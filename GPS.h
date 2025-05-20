#ifndef gps_h
#define gps_h

#include <Arduino.h>

struct GPSReading {
  float Latitude;
  float Longitude;
  bool Valid;
};

void initGPS();
GPSReading readGPS();
bool hasFix();

#endif
