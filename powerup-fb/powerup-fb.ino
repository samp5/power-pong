#include <WiFi.h>

// -----------------------
//       TYPE DEFS
// -----------------------
typedef enum Powerup {
  Joystick,
  Distance,
  Photoresistor,
  Numbers,
  Motor,
};

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



// -----------------------
//       CD FUNCS
// -----------------------
void invokeCD() {

}

void updateCooldowns() {
  bool status;
  // check each powerups cooldown
  for (int i = 0; i < 5; ++i) {
    int cd = powerupStatus[i].cd;
    int lastInvoked = powerupStatus[i].lastInvoked;

    // set cooldown expired status
    status = powerupStatus[i].cdExpired = millis() >= (lastInvoked + cd);

    // set the LED to light based on status
    digitalWrite(LED_PINS[i], status);
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
    switch ((Powerup) i) {
      case Joystick:
        powerupStatus[i].cd = 5000;
        break;

      case Distance:
        powerupStatus[i].cd = 5000;
        break;

      case Photoresistor:
        powerupStatus[i].cd = 5000;
        break;

      case Numbers:
        powerupStatus[i].cd = 5000;
        break;

      case Motor:
        powerupStatus[i].cd = 5000;
        break;
    }

    // set the last invoked to negative cooldown, to allow cooldown expired
    powerupStatus[i].lastInvoked = -powerupStatus[i].cd;
  }
}

void loop() {
  invokeCD();
  updateCooldowns();
}
