#include <TinyGPS++.h>
#include "GPS.h"

static TinyGPSPlus gps;
const long GPSBaud = 9600;

void initGPS() {
  Serial1.begin(GPSBaud);
  delay(100);
  if (Serial1.available()) {
    Serial.println("GPS initialised");
  } else {
    Serial.println("Failed to initialise GPS (no data received)");
  }
}


GPSReading readGPS() {
  while (Serial1.available()) {
    gps.encode(Serial1.read());
  }

  GPSReading reading = {0.0, 0.0, false};

  if (gps.location.isValid()) {
    reading.Latitude = gps.location.lat();
    reading.Longitude = gps.location.lng();
    reading.Valid = true;
  }

  return reading;
}

bool hasFix() {
  return gps.location.isValid() &&
         !isnan(gps.location.lat()) &&
         !isnan(gps.location.lng()) &&
         gps.location.lat() != 0.0 &&
         gps.location.lng() != 0.0;
}


