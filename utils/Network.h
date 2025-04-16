#ifndef NETWORK
#define NETWORK
#include "Packet.h"
#include <WiFiS3.h>
#include <cstdio>
#define SSID "sa"      // my phone hotspot
#define PWD "abcdefgh" // my phone hotspot password for this
#define PORT 8000

// IDs for a game client to assume. there should only be one of each connecting
// to the game server
enum ClientID {
  GAME_IN,
  POWERUP_IN,
  POWERUP_FB,
};

/**
 * gets IP address via serial
 * input IP address must be in format of w.x.y.z, where the variables are ints
 * this function is blocking.
 */
IPAddress getIPSerial() {
  char input[64];

  Serial.println("Connected to Serial. Input IP Address");
  // wait for input
  while (Serial.available() == 0) {
  };

  // clear from prev input
  memset(input, '\0', 64);
  Serial.readBytesUntil('\n', input, 64);

  // parse the input
  IPAddress ip(input);

  return ip;
}

#endif // !NETWORK
