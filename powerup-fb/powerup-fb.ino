#include "utils/PowerUps.h"
#include <WiFi.h>

// -----------------------
//       TYPE DEFS
// -----------------------
typedef struct {
  bool cdExpired;
  int lastInvoked;
  int cd;
} PowerupStatus;



// -----------------------
//     CONSTS & VARS
// -----------------------
// pins
byte LED_PINS[5] = {
  12, 11, 10, 9, 8
};

// runtime variables
PowerupStatus powerupStatus[5];
int updatedCDStatus;



// -----------------------
//       CD FUNCS
// -----------------------
void invokeCD() {
  // if a packet is recieved from server, set that powerup's cd
}

void updateCooldowns() {
  // check each powerups cooldown
  for (int i = 0; i < 5; ++i) {
    int cd = powerupStatus[i].cd;
    int lastInvoked = powerupStatus[i].lastInvoked;

    // set cooldown expired status
    int cdStatus = millis() >= (lastInvoked + cd);
    if (cdStatus) {
      updatedCDStatus | (1 << i);
    }

    // set the LED and struct to light based on status
    powerupStatus[i].cdExpired = cdStatus;
    digitalWrite(LED_PINS[i], powerupStatus[i].cdExpired);
  }

  if (updatedCDStatus) {
    PowerupPacket packet;
    packet.cooldownsExpired = updatedCDStatus;
    // send packet to server

    updatedCDStatus = 0;
  }
}



// -----------------------
//        INO FUNCS
// -----------------------
void setup() {
  // for each powerup
  for (int i = 0; i < 5; ++i) {
    // set its LED to output
    pinMode(LED_PINS[i], OUTPUT);

    // always set cooldown as expired
    powerupStatus[i].cdExpired = true;

    // then set up its cooldown duration.
    // exists in a switch case to allow for individual cd tweaking.
    switch ((PowerUps) i) {
      case BallSpeedUp:
        powerupStatus[i].cd = 5000;
        break;

      case BallInvisible:
        powerupStatus[i].cd = 5000;
        break;

      case PaddleSpeedUp:
        powerupStatus[i].cd = 5000;
        break;

      case BallSize:
        powerupStatus[i].cd = 5000;
        break;

      case BonusPoints:
        powerupStatus[i].cd = 5000;
        break;
    }

    // set the last invoked to negative cooldown, to allow cooldown expired
    powerupStatus[i].lastInvoked = -powerupStatus[i].cd;
  }

  updatedCDStatus = 0;
}

void loop() {
  invokeCD();
  updateCooldowns();
}
