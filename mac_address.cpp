#include <WiFiNINA.h>
#include "mac_address.h"

const char* getMacAddress() {
  static char macStr[18]; // 17 chars + null terminator

  byte mac[6];
  WiFi.macAddress(mac);
  snprintf(macStr, sizeof(macStr),
           "%02X:%02X:%02X:%02X:%02X:%02X",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

  return macStr;
}
