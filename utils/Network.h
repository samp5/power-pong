#ifndef NETWORK
#define NETWORK
#include <WiFiS3.h>
#include <cstdio>
#define SSID "sa"      // my phone hotspot
#define PWD "abcdefgh" // my phone hotspot password for this
#define PORT 8000

/**
 * gets IP address via serial
 * input IP address must be in format of w.x.y.z, where the variables are ints
 * this function is blocking.
 */
IPAddress getIPSerial() {
  char input[64];

  // wait for input
  while (Serial.available() == 0) {
  };
  //
  // clear from prev input
  memset(input, '\0', 64);
  Serial.readBytesUntil('\n', input, 64);

  // parse the input
  IPAddress ip(input);
}

#endif // !NETWORK
