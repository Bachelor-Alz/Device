#ifndef IMU_H
#define IMU_H

#include <Arduino_LSM6DS3.h>

// Data structure for IMU readings
struct IMUReading {
  float ax, ay, az;
  float gx, gy, gz;
};

// Initialise IMU
inline void initIMU() {
  if (!IMU.begin()) {
    Serial.println("IMU init failed!");
    while (true);
  }
  Serial.println("IMU ready.");
}

// Read current IMU state into provided struct
inline bool readIMU(IMUReading& reading) {
  float ax, ay, az, gx, gy, gz;

  if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable()) {
    IMU.readAcceleration(ax, ay, az);
    IMU.readGyroscope(gx, gy, gz);
    reading.ax = ax;  reading.ay = ay;  reading.az = az;
    reading.gx = gx;  reading.gy = gy;  reading.gz = gz;
    return true;
  }

  return false;
}

#endif
