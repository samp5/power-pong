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
            CooldownsTriggeredData data;
            packet->toStruct(&data);
            handlePowerupActivation(data);
            break;
        }
        case PowerupCDPacket:
            CooldownsExpiredData data;
            packet->toStruct(&data);
            handleCooldownExpired(data);
            break;
    }
}

void handlePowerupActivation(const CooldownsTriggeredData& data) {
    Serial.println("handlig powerup activation");
    int toActivate = 0;
    for (int i = 0; i < NUM_POWERUPS; i++){
        Serial.print("checking powerup: ");
        Serial.println(i);
      if ((data.packetsTriggered >> i) & (cooldownsExpired >> 1)) {
        Serial.println("should be activated! ");
        toActivate = toActivate | (0b1 << i);
        Serial.print("toActivate is now:");
        Serial.println(toActivate);
        cooldownsExpired = cooldownsExpired ^ (0b1 << i);
        Serial.print("cooldowns expired is now: ");
        Serial.println(cooldownsExpired);
      }
    }

    if (toActivate > 0){
      CooldownsTriggeredData data;
      data.packetsTriggered = toActivate;
      Packet p = Packet(PowerupActivatePacket).withData(&data).sendable();

      Serial.println("sending packet to game input");
      server.sendPacket(GAME_IN, &p);
      Serial.println("sending packet to powerup feedback");
      server.sendPacket(POWERUP_FB, &p);
    }
}

void handleCooldownExpired(const CooldownsExpiredData& data) {
    Serial.println("handling cooldown expired");
    for (int i = 0; i < NUM_POWERUPS; i++){
      if ((data.cooldownsExpired >> i) & 0b1) {
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
    Packet** ppp = (Packet**) &packetArr;
    int packetsRecived = server.readPackets(ppp);
    for (int i = 1; i < packetsRecived; i++) {
        managePacket(&packetArr[i]);
    }
}

