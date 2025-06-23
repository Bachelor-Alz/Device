#ifndef gps_h
#define gps_h

#include <Arduino.h>

//VCC -> 3.3V | GND -> GND | TX -> D0(RX1) | RX -> D1(RX0) WE DON'T USE THIS

struct GPSReading {
  float Latitude;
  float Longitude;
  bool Valid;
};

void initGPS();
GPSReading readGPS();
bool hasFix();

#endif
