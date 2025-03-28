#ifndef HTTP_POST_REQUESTS_H
#define HTTP_POST_REQUESTS_H

#include <WiFiNINA.h>
#include <ArduinoJson.h>
#include "IMU.h"

enum SendState { Idle, Connecting, Sending, Done };

// Helper to calculate payload length without building the full string
class LengthCalculator : public Print {
public:
  size_t len = 0;
  size_t write(uint8_t) override { len++; return 1; }
  size_t write(const uint8_t *buffer, size_t size) override { len += size; return size; }
};

// Serialise one reading to a Print target
inline void sendIMUReading(Print& out, const IMUReading& reading) {
  StaticJsonDocument<64> doc;
  doc["ax"] = reading.ax;
  doc["ay"] = reading.ay;
  doc["az"] = reading.az;
  doc["gx"] = reading.gx;
  doc["gy"] = reading.gy;
  doc["gz"] = reading.gz;

  serializeJson(doc, out);
}

inline void beginIMUSend(WiFiClient& client, const char* server, int port, const String& endpoint,
                         IMUReading* readings, int count, SendState& state) {

  // === STEP 1: Accurately calculate content length ===
  LengthCalculator calc;
  calc.write('[');
  for (int i = 0; i < count; i++) {
    if (i > 0) calc.write(',');
    sendIMUReading(calc, readings[i]);
  }
  calc.write(']');
  int contentLength = calc.len;

  Serial.println("Connecting to server...");
  if (client.connect(server, port)) {
    client.println("POST " + endpoint + " HTTP/1.1");
    client.println("Host: " + String(server));
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(contentLength);
    client.println("Connection: keep-alive");
    client.println();

    client.print("[");
    for (int i = 0; i < count; i++) {
      if (i > 0) client.print(",");
      sendIMUReading(client, readings[i]);
    }
    client.println("]"); // ✅ Close the JSON array

    state = Sending;
    Serial.println("Data sent.");
  } else {
    Serial.println("Connection failed.");
    state = Done;
  }
}

inline bool handleIMUSend(WiFiClient& client, SendState& state) {
  if (state == Sending) {
    while (client.available()) {
      String line = client.readStringUntil('\n');
      Serial.println("Response: " + line);
    }

    if (!client.connected()) {
      client.stop();
      state = Done;
      return true;
    }
  }

  return false;
}

#endif
