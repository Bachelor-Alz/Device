#include "MAX30102.h"
#include <MAX30105.h> 
#include <Wire.h>
#include "spo2_algorithm.h"
#include <Arduino.h>

MAX30105 particleSensor;
uint32_t irBuffer[100];
uint32_t redBuffer[100];

void initMAX30102() {
  Serial.println("Initialising MAX30102...");

  if (!particleSensor.begin(Wire, I2C_SPEED_STANDARD)) {
    Serial.println("MAX30102 not found. Check wiring.");
    while (1);
  }

  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x3F);
  particleSensor.setPulseAmplitudeIR(0x3F);
  particleSensor.setPulseAmplitudeGreen(0);
  Serial.println("Place finger on the sensor.");
}

bool readMAX30102(int32_t &heartRate, int8_t &validHeartRate, int32_t &spo2, int8_t &validSPO2) {
  for (byte i = 0; i < 100; i++) {
    while (!particleSensor.available()) {
      particleSensor.check();
    }

    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample();
  }

  maxim_heart_rate_and_oxygen_saturation(
    irBuffer, 100,
    redBuffer,
    &spo2, &validSPO2,
    &heartRate, &validHeartRate
  );

  return (validHeartRate && validSPO2);
}
