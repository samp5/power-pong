#include "utils/Server.h"
#include "utils/PowerUps.h"
#include "utils/Client.h"
#include "utils/Packet.h"
#define NUM_POWERUPS 5

WifiServer server;
Packet packetArr[48];
int cooldownsExpired = 0b11111;

void managePacket(Packet* packet) {
  Serial.println("managing packet");
    switch (packet->getType()) {
        case PowerupActivatePacket: {
            handlePowerupActivation(packet->getData());
            break;
        }
        case PowerupCDPacket:
            handleCooldownExpired(packet->getData());
            break;
        default:
          Serial.println("no type matched!!");
    }
}

void handlePowerupActivation(int activated) {
    Serial.println("handlig powerup activation");
    int toActivate = 0;
    for (int i = 0; i < NUM_POWERUPS; i++){

        Serial.print("checking powerup: ");
        Serial.println(i);

      if (1 & ((activated >> i) & (cooldownsExpired >> 1))) {

        Serial.print(i);
        Serial.println(" should be activated! ");

        toActivate = toActivate | (0b1 << i);

        Serial.print("toActivate is now:");
        Serial.println(toActivate);
        Serial.println("removing from cooldownsExpired");

        cooldownsExpired = cooldownsExpired ^ (0b1 << i);

        Serial.print("cooldowns expired is now: ");
        Serial.println(cooldownsExpired);
      }
    }

    if (toActivate > 0){
      Packet p = Packet(PowerupActivatePacket);
      p.setData(toActivate);

      Serial.println("sending packet to game input");
      server.sendPacket(GAME_IN, &p);
      Serial.println("sending packet to powerup feedback");
      server.sendPacket(POWERUP_FB, &p);
    }
}

void handleCooldownExpired(int cooldowns) {
    Serial.println("handling cooldown expired");
    for (int i = 0; i < NUM_POWERUPS; i++){
      Serial.print("checking powerup: ");
      Serial.println(i);
      if ((cooldowns >> i) & 0b1) {
          Serial.print(i);
          Serial.println(" is expired");
          Serial.print("cooldowns expired is now: ");
          Serial.println(cooldownsExpired);
          cooldownsExpired = cooldownsExpired | (0b1 << i);
      }
    }
}


void setup() {
    Serial.begin(9600);
    server.initialize();
}

void loop() {
    int packetsRecived = server.readPackets(packetArr);
    if (packetsRecived > 0){
      Serial.print("packets recieved:");
      Serial.println(packetsRecived);
    }
    for (int i = 0; i < packetsRecived; i++) {
        Serial.println(packetArr[i].toBinaryString());
        managePacket(&packetArr[i]);
    }
}

