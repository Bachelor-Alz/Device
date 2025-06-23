#ifndef MAX30102_H
#define MAX30102_H

#include "gps.h"
 
//VIN -> 3.3V | SDA -> A4 | SCL -> A5 | GND -> GND

struct MAX30102Reading {
  int32_t heartRate;
  int32_t spO2;
};

const int maxBatchSize = 10;

struct MAX30102Batch {
  const char* mac;
  GPSReading gps;
  MAX30102Reading readings[maxBatchSize];
  int count = 0;
  int steps = 0;
};

void initMAX30102();
bool readMAX30102(int32_t& heartRate, int8_t& validHeartRate, int32_t& spO2, int8_t& validSPO2);

#endif
