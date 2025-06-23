#include "HTTP_Post_Requests.h"
#include <ArduinoJson.h>
#include "serialisation.h"
#include "MAX30102.h"
#include "GPS.h"

extern const char* macAddress;

void postInitialGPS(WiFiClient& client, const char* server, int port, const String& endpoint) {
  Serial.println("Waiting for GPS fix before posting initial location...");

  const unsigned long timeout = 900000; // 15 minutes
  unsigned long start = millis();

  GPSReading gps;
  while (!hasFix() && (millis() - start < timeout)) {
    gps = readGPS();
    delay(1000);
  }

  if (!hasFix()) {
    Serial.println("Timed out waiting for GPS fix.");
    return;
  }
  
  Serial.println("GPS fix acquired:");
  Serial.print("Latitude: "); Serial.println(gps.Latitude, 6);
  Serial.print("Longitude: "); Serial.println(gps.Longitude, 6);

  StaticJsonDocument<256> doc;
  JsonArray gpsArray = doc.to<JsonArray>();

  JsonObject gpsObj = gpsArray.createNestedObject();
  gpsObj["macAddress"] = macAddress;
  gpsObj["Latitude"] = gps.Latitude;
  gpsObj["Longitude"] = gps.Longitude;


  String payload;
  serializeJson(doc, payload);

  if (client.connect(server, port)) {
    client.print(F("POST "));
    client.print(endpoint);
    client.println(F(" HTTP/1.1"));
    client.print(F("Host: "));
    client.println(server);
    client.println(F("Content-Type: application/json"));
    client.print(F("Content-Length: "));
    client.println(payload.length());
    client.println(F("Connection: close"));
    client.println();
    client.println(payload);

    Serial.println("Initial GPS fix posted:");
    Serial.println(payload);

    // Wait for headers to finish
    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") break;
    }

    client.stop();
  } else {
    Serial.println("Failed to connect to server for initial GPS post.");
  }
}

class LengthCalculator : public Print {
public:
  size_t len = 0;
  size_t write(uint8_t) override { len++; return 1; }
  size_t write(const uint8_t *buffer, size_t size) override { len += size; return size; }
};


void beginIMUSend(WiFiClient& client, const char* server, int port, const String& endpoint,
                  IMUReading* readings, int count, SendState& state) {
// STEP 1: Calculate content length
  LengthCalculator calc;
  calc.print("{\"mac\":\"");
  calc.print(macAddress);
  calc.print("\",\"readings\":[");
  for (int i = 0; i < count; ++i) {
    if (i > 0) calc.print(",");
    sendIMUReading(calc, readings[i]);
  }
  calc.print("]}");
  int contentLength = calc.len;

  // STEP 2: Connect and send
  Serial.print("Connecting to server: ");
  Serial.print(server);
  Serial.print(":");
  Serial.println(port);

  if (client.connect(server, port)) {
    Serial.println("IMU client connected to server.");

    client.print(F("POST "));
    client.print(endpoint);
    client.println(F(" HTTP/1.1"));
    client.print(F("Host: "));
    client.println(server);
    client.println(F("Content-Type: application/json"));
    client.print(F("Content-Length: "));
    client.println(contentLength);
    client.println(F("Connection: close"));
    client.println();

    // STEP 3: Stream JSON data
    client.print("{\"mac\":\"");
    client.print(macAddress);
    client.print("\",\"readings\":[");
    for (int i = 0; i < count; ++i) {
      if (i > 0) client.print(",");
      sendIMUReading(client, readings[i]);
    }
    client.println("]}");

    Serial.println("IMU data streamed.");
    state = Sending;
  } else {
    Serial.println("IMU connection to server failed.");
    state = Done;
  }
}

bool handleClientSend(WiFiClient& client, SendState& state) {
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

void sendMAX30102Batch(WiFiClient& client, const char* server, int port, const String& endpoint,
                       const MAX30102Batch& batch, SendState& state) {
  StaticJsonDocument<1024> doc;
  serialiseMAX30102Batch(doc, batch);

  String payload;
  serializeJson(doc, payload);
  Serial.println("DEBUG: Combined Payload:");
  Serial.println(payload);

  Serial.print("Connecting to MAX endpoint: ");
  Serial.print(server);
  Serial.print(":");
  Serial.println(port);

  if (client.connect(server, port)) {
    client.print(F("POST "));
    client.print(endpoint);
    client.println(F(" HTTP/1.1"));
    client.print(F("Host: "));
    client.println(server);
    client.println(F("Content-Type: application/json"));
    client.print(F("Content-Length: "));
    client.println(payload.length());
    client.println(F("Connection: close"));
    client.println();
    client.println(payload);

    Serial.println("MAX30102 batch with GPS and steps sent.");
    state = Sending;

    /*
    // Read the response line by line
    String statusLine = client.readStringUntil('\n');
    Serial.print("HTTP Response: ");
    Serial.println(statusLine);

    // Optional: print remaining headers/body
    while (client.available()) {
      String line = client.readStringUntil('\n');
      Serial.println(line);
    }
    */
  } else {
    Serial.println("Connection to batch endpoint failed.");
    state = Done;
  }
}
