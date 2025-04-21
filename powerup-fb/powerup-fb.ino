#include "utils/button.h"
#include "utils/PowerUps.h"
#include "utils/Packet.h"
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
  12, // Photores | Ball Invis
  11, // Motor | Paddle Speed
  10, // Distance | Ball Size
  9,  // Numpad | Points
  8   // 
};

// runtime variables
ClientConnection client;
PowerupStatus powerupStatus[5];
int updatedCDStatus;
Packet recievedPackets[16];



// -----------------------
//       CD FUNCS
// -----------------------
void invokeCD() {
  // get all packets recieved
  int packets = client.readPackets(recievedPackets);
  if (packets > 0) {
    Serial.print("Packets gotten: ");
    Serial.println(packets);
  }

  // for each packet recieved
  for (int i = 0; i < packets; ++i) {
    // take the packet
    Packet* packet = &recievedPackets[i];
    PacketType type = packet->getType();

    Serial.print("Packet content: ");
    Serial.println((byte)packet->data);

    Serial.print("Parsing packet of type ");
    Serial.println(type);

    // only process powerup activation packets
    if (type == PowerupActivatePacket) {
      // for each powerup
      for (int i = 0; i < 5; ++i) {
        int powerupBit = (packet->getData() >> i) & 0b1;
        Serial.print("  Powerup of index ");
        Serial.print(i);
        Serial.print(" has value ");
        Serial.println(powerupBit);

        // if the triggered but was set, set invoked to now
        if (powerupBit) {
          Serial.print("  Powerup activated: ");
          Serial.println(i);
          powerupStatus[i].cdExpired = false;
          powerupStatus[i].lastInvoked = millis();
        }
      }
    }
  }
}

void sendCDPacket() {
  if (!client.isConnected()) return;

  Serial.print("Sending packet with value ");
  Serial.println(updatedCDStatus);

  Packet p = Packet(PowerupCDPacket);
  p.setData(updatedCDStatus);
  client.sendPacket(&p);
}

void updateCooldowns() {
  // check each powerups cooldown
  for (int i = 0; i < 5; ++i) {
    int cd = powerupStatus[i].cd;
    int lastInvoked = powerupStatus[i].lastInvoked;

    // set cooldown expired status
    bool cdStatus = millis() >= (lastInvoked + cd);
    if (cdStatus && !powerupStatus[i].cdExpired) {
      Serial.print("Powerup cooldown at index ");
      Serial.print(i);
      Serial.println(" just expired");
      updatedCDStatus |= (1 << i);
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
    switch (i) {
      case 0: // BallSpeedUp:
        powerupStatus[i].cd = 30000;
        break;

      case 1: // BallInvisible:
        powerupStatus[i].cd = 30000;
        break;

      case 2: // PaddleSpeedUp:
        powerupStatus[i].cd = 30000;
        break;

      case 3: // BallSize:
        powerupStatus[i].cd = 30000;
        break;

      case 4: // BonusPoints:
        powerupStatus[i].cd = 30000;
        break;
    }

    // set the last invoked to negative cooldown, to allow cooldown expired
    powerupStatus[i].lastInvoked = -powerupStatus[i].cd;
  }

  updatedCDStatus = 0;

  IPAddress ip = getIPSerial();
  client = ClientConnection(POWERUP_FB, ip);
}

void loop() {
  invokeCD();
  updateCooldowns();
}
