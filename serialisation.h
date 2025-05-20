// Serialisation.h
#ifndef SERIALISATION_H
#define SERIALISATION_H

#include <ArduinoJson.h>
#include "IMU.h"
#include "MAX30102.h"

void serialiseIMUReading(JsonArray& array, const IMUReading& reading);
void serialiseIMUBuffer(JsonArray& array, IMUReading* buffer, int count);
void serialiseMAX30102Batch(JsonDocument& doc, const MAX30102Batch& batch);

#endif
