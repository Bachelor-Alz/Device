#ifndef HTTP_POST_REQUESTS_H
#define HTTP_POST_REQUESTS_H

#include <WiFiNINA.h>
#include <ArduinoJson.h>
#include "IMU.h"
#include "MAX30102.h"

enum SendState { Idle, Connecting, Sending, Done };

// Inline function for JSON streaming
inline void sendIMUReading(Print& out, const IMUReading& reading) {
  const float degToRad = 0.0174533;

  float ax = reading.ax;
  float ay = reading.ay;
  float az = reading.az;

  float gx_rad = reading.gx * degToRad;
  float gy_rad = reading.gy * degToRad;
  float gz_rad = reading.gz * degToRad;

  StaticJsonDocument<64> doc;
  doc["ax"] = ax;
  doc["ay"] = ay;
  doc["az"] = az;
  doc["gx"] = gx_rad;
  doc["gy"] = gy_rad;
  doc["gz"] = gz_rad;

  serializeJson(doc, out);
}

void postInitialGPS(WiFiClient& client, const char* server, int port, const String& endpoint);
void beginIMUSend(WiFiClient& client, const char* server, int port, const String& endpoint,
                  IMUReading* readings, int count, SendState& state);
bool handleClientSend(WiFiClient& client, SendState& state);
void processSendState(WiFiClient& imuClient, WiFiClient& maxClient, SendState& state);
void sendMAX30102Batch(WiFiClient& client, const char* server, int port, const String& endpoint,
                       const MAX30102Batch& batch, SendState& state);

#endif
