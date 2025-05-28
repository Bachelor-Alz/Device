## HealthDevice: Arduino Setup and Configuration

A compact Arduino-based data acquisition system designed to stream IMU, GPS, and biometric sensor data via HTTP. Built on the **Arduino Nano 33 IoT**, this project interfaces with multiple onboard and external sensors and supports network transmission over Wi-Fi.

---

## Overview

This project collects sensor data from the following modules:

- **LSM6DS3** (built into the Nano 33 IoT): Inertial Measurement Unit (IMU) for accelerometer and gyroscope readings.
- **NEO-6M**: GPS module for geolocation data (latitude and longitude).
- **MAX30102**: Pulse oximeter and heart-rate sensor.

It formats the readings into JSON and transmits them to clients over a Wi-Fi network using a simple HTTP-based communication protocol.

---

## Hardware Used

| Sensor / Board         | Purpose                          | Connection Notes                |
|------------------------|----------------------------------|---------------------------------|
| Arduino Nano 33 IoT    | Main microcontroller + Wi-Fi     | USB and Wi-Fi                   |
| LSM6DS3 (onboard)      | Accelerometer + Gyroscope        | I²C (internal to Nano)          |
| NEO-6M GPS             | Geolocation                      | UART (RX/TX to Serial1)         |
| MAX30102               | Heart rate + SpO₂                | I²C (SDA, SCL)                  |

---

## Wiring Guide

### NEO-6M GPS
| GPS Pin  | Arduino Pin |
|----------|-------------|
| VCC      | 3.3V        |
| GND      | GND         |
| TX       | RX1 (pin 0) |
| RX       | TX1 (pin 1) |

### MAX30102
| Sensor Pin | Arduino Pin |
|------------|-------------|
| VIN        | 3.3V        |
| GND        | GND         |
| SDA        | SDA (A4)    |
| SCL        | SCL (A5)    |

> **Note**: Ensure all sensors operate at 3.3V logic. Do **not** supply 5V to the Nano 33 IoT or connected peripherals.

---

## Getting Started

### 1. **Dependencies**
Install the following libraries via the Arduino Library Manager:

- `Arduino_LSM6DS3`
- `Adafruit_GPS`
- `MAX30105`
- `WiFiNINA`
- `ArduinoJson`

### 2. Wi-Fi and Server Configuration
Edit the `secrets.h` file to configure your network and server settings:

```cpp
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

const char HOST[] = ""; //Update HOST with your IPv4 Address
const int AI_API_PORT = "9999";
const int API_PORT = "5171";
```


