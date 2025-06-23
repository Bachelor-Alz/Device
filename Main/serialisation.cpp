#include "Serialisation.h"

void serialiseIMUReading(JsonArray& array, const IMUReading& reading) {
  JsonObject obj = array.createNestedObject();
  obj["ax"] = reading.ax;
  obj["ay"] = reading.ay;
  obj["az"] = reading.az;
  obj["gx"] = reading.gx;
  obj["gy"] = reading.gy;
  obj["gz"] = reading.gz;
}

void serialiseIMUBuffer(JsonArray& array, IMUReading* buffer, int count) {
  for (int i = 0; i < count; ++i) {
    serialiseIMUReading(array, buffer[i]);
  }
}

void serialiseMAX30102Batch(JsonDocument& doc, const MAX30102Batch& batch) {
  doc["macAddress"] = batch.mac;
  doc["latitude"] = batch.gps.Latitude;
  doc["longitude"] = batch.gps.Longitude;
  doc["steps"] = batch.steps;

  JsonArray readings = doc.createNestedArray("max30102");
  for (int i = 0; i < batch.count; ++i) {
    JsonObject r = readings.createNestedObject();
    r["heartRate"] = batch.readings[i].heartRate;
    r["spO2"] = batch.readings[i].spO2;
  }
}

