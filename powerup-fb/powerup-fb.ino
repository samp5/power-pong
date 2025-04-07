#include "utils/PowerUps.h"
#include <WiFiS3.h>
#include "utils/Client.h"
#include "utils/Server.h"

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
ClientConnection client;
PowerupStatus powerupStatus[5];
int updatedCDStatus;



// -----------------------
//       CD FUNCS
// -----------------------
void tmpReadResponse() {
  uint32_t received_data_num = 0;
  while (client.client.available()) {
    /* actual data reception */
    char c = client.client.read();
    /* print data to serial port */
    Serial.print(c);
    /* wrap data to 80 columns*/
    received_data_num++;
    if(received_data_num % 80 == 0) { 
      Serial.println();
    }
  }
}

void invokeCD() {
  // get all packets recieved
  Packet** packetsRecieved;
  int packets = /*client.readPackets(packetsRecieved);*/ 0;

  // for each packet recieved
  for (int i = 0; i < packets; ++i) {
    // take the packet
    Packet* packet = packetsRecieved[i];

    // only process powerup activation packets
    if (packet->getType() == PowerupActivatePacket) {
      // parse the packet appropriately
      CooldownsTriggeredData triggered;
      packet->toStruct(&triggered);

      // for each powerup
      for (int i = 0; i < 5; ++i) {
        // if the triggered but was set, set invoked to now
        if (triggered.packetsTriggered >> i) {
          powerupStatus[i].lastInvoked = millis();
        }
      }
    }
  }
}

void sendCDPacket() {
  if (!client.isConnected()) return;

  CooldownsExpiredData data;
  data.cooldownsExpired = updatedCDStatus;
  Packet p = Packet(PowerupCDPacket).withData(&data).sendable();
  client.sendPacket(&p);
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
    sendCDPacket();
    updatedCDStatus = 0;
  }
}



// -----------------------
//        INO FUNCS
// -----------------------
void setup() {
  Serial.begin(9600);

  // for each powerup
  for (int i = 0; i < 5; ++i) {
    // set its LED to output
    pinMode(LED_PINS[i], OUTPUT);
    digitalWrite(LED_PINS[i], HIGH);

    // always set cooldown as expired
    powerupStatus[i].cdExpired = true;

    // then set up its cooldown duration.
    // exists in a switch case to allow for individual cd tweaking.
    switch ((PowerUps)i) {
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

  IPAddress ip = getIPSerial();
  client = ClientConnection(ip);
  client.write((char)0);
  Serial.println("Sent 0");
}

void loop() {
  tmpReadResponse();
  invokeCD();
  updateCooldowns();
}
