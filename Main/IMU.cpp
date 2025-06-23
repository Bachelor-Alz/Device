#include "IMU.h"
#include <math.h> // for sqrt

static int steps = 0;
static bool wasAbove = false;
const float stepThreshold = 1.2;

void initIMU() {
  if (!IMU.begin()) {
    Serial.println("Failed to initialise LSM6DS3");
  } else {
    Serial.println("IMU initialised");
  }
}

bool readIMU(IMUReading& reading) {
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(reading.ax, reading.ay, reading.az);

    // Step detection using acceleration magnitude
    float magnitude = sqrt(reading.ax * reading.ax +
                           reading.ay * reading.ay +
                           reading.az * reading.az);

  if (!wasAbove && magnitude > stepThreshold) {
    steps++;
    wasAbove = true;
  } else if (wasAbove && magnitude < 0.95) {
    wasAbove = false;
  }
}

  if (IMU.gyroscopeAvailable()) {
    IMU.readGyroscope(reading.gx, reading.gy, reading.gz);
  }

  return true;
}

int getStepCount() {
  return steps;
}

void resetStepCount() {
  steps = 0;
  wasAbove = false;
}
