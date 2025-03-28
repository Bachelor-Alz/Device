#include <WiFiNINA.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "secrets.h"
#include "IMU.h"
#include "HTTP_Post_Requests.h"

// WiFi credentials
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int status = WL_IDLE_STATUS;

// Server config
const char* server = "192.168.1.18";
int port = 9999;
const String imuEndpoint = "/predict";

// NTP
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);

// IMU sampling
const unsigned long imuInterval = 20;
const int maxSamples = 1000;
int sampleCount = 0;
IMUReading imuBuffer[maxSamples];

// Sending
SendState sendState = Idle;
WiFiClient client;
String payload;
bool sendTriggered = false;

void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("No WiFi module found!");
    while (true);
  }

  while (status != WL_CONNECTED) {
    Serial.print("Connecting to ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(5000);
  }

  Serial.println("WiFi connected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  timeClient.begin();
  while (!timeClient.update()) timeClient.forceUpdate();
  initIMU();
}

void loop() {
  if (sampleCount < maxSamples) {
    if (readIMU(imuBuffer[sampleCount])) {
      Serial.print("Buffered IMU #");
      Serial.println(sampleCount + 1);
      sampleCount++;
    }
  }

  // === Check if we need to send ===
  if (!sendTriggered && (sampleCount >= maxSamples)) {
    sendTriggered = true;
    beginIMUSend(client, server, port, imuEndpoint, imuBuffer, sampleCount, sendState);
  }

  if (sendTriggered) {
    if (handleIMUSend(client, sendState)) {
      sendTriggered = false;
      sampleCount = 0;
    }
  }
}
