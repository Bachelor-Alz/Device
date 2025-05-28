#ifndef IMU_H
#define IMU_H

#include <Arduino_LSM6DS3.h>

struct IMUReading {
  float ax, ay, az;
  float gx, gy, gz;
};

void initIMU();
bool readIMU(IMUReading& reading);
int getStepCount();
void resetStepCount();

#endif
