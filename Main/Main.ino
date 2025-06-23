#include "secrets.h"
#include "IMU.h"
#include "MAX30102.h"
#include "mac_address.h"
#include "serialisation.h"
#include "HTTP_Post_Requests.h"
#include <WiFiNINA.h>

// Network Clients
WiFiClient imuClient;
WiFiClient maxClient;

// Constants
const int maxIMUSamples = 750;
const int maxMAXSamples = 20;
const unsigned long imuInterval = 20;         // 50 Hz
unsigned long lastIMUSampleTime = 0;
int sampleCountIMU = 0;
int imuSendCount = 0;
int sampleCountMAX = 0;

// Buffers
IMUReading imuBuffer[maxIMUSamples];
MAX30102Reading maxBuffer[maxMAXSamples];

// Sending
// Batching and Step Counter
const unsigned long batchInterval = 5 * 60 * 1000; // 5 minutes
unsigned long lastBatchSend = 0;
MAX30102Batch maxBatch;

SendState sendState = Idle;
String payload;
bool sendTriggered = false;

const char* macAddress;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to Wi-Fi...");
  }

  Serial.println("WiFi connected.");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  macAddress = getMacAddress();
  Serial.print("MAC: ");
  Serial.println(macAddress);

  initGPS();
  postInitialGPS(maxClient, HOST, API_PORT, GPS_ENDPOINT);
  initIMU();
  initMAX30102();
}

void loop() {
  unsigned long now = millis();

  if (sampleCountIMU < maxIMUSamples && (now - lastIMUSampleTime >= imuInterval)) {
  lastIMUSampleTime = now;

  if (readIMU(imuBuffer[sampleCountIMU])) {
    //Serial.print("Buffered IMU #");
    //Serial.println(sampleCountIMU + 1);
    sampleCountIMU++;
    }
  }

if (sampleCountIMU >= maxIMUSamples && sendState == Idle) {
  int steps = getStepCount();
  Serial.print("Estimated Steps: ");
  Serial.println(steps);
  Serial.println("Sending IMU data...");

  beginIMUSend(imuClient, HOST, AI_API_PORT, IMU_ENDPOINT,
               imuBuffer, sampleCountIMU, sendState);
  sampleCountIMU = 0;

  // Take one MAX30102 reading immediately after IMU send
  if (sampleCountMAX < maxMAXSamples) {
    int32_t heartRate, spo2;
    int8_t validHeartRate, validSPO2;

    if (readMAX30102(heartRate, validHeartRate, spo2, validSPO2)) {
      maxBuffer[sampleCountMAX].heartRate = heartRate;
      maxBuffer[sampleCountMAX].spO2 = spo2;
      sampleCountMAX++;

      Serial.print("MAX30102 (after IMU send): HR=");
      Serial.print(heartRate);
      Serial.print(" SpO2=");
      Serial.println(spo2);
    } else {
      Serial.println("MAX30102 reading failed");
    }
  }
}

  // Batch Send
  if ((now - lastBatchSend >= batchInterval) && sendState == Idle) {
    Serial.println("Sending MAX30102 batch + GPS + steps...");
    maxBatch.mac = macAddress;
    maxBatch.gps = readGPS(); 
    maxBatch.steps = getStepCount(); 

  // Fill batch from buffer
    maxBatch.count = sampleCountMAX;
      for (int i = 0; i < sampleCountMAX; i++) {
        maxBatch.readings[i] = maxBuffer[i];
    }

    sendMAX30102Batch(maxClient, HOST, API_PORT, DATA_ENDPOINT, maxBatch, sendState);
    resetStepCount(); 
    sampleCountMAX = 0;
    imuSendCount = 0;
    lastBatchSend = now;
  }

  handleClientSend(imuClient, sendState);
  handleClientSend(maxClient, sendState);
  if (sendState == Done) {
    sendState = Idle;
  }
}





