#include "utils/Server.h"
#include "utils/PowerUps.h"
#include "utils/Client.h"
#include "utils/Packet.h"
#define NUM_POWERUPS 5

WifiServer server;
Packet packetArr[48];
int cooldownsExpired = 0b11111;

void managePacket(Packet* packet) {
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
    int toActivate = 0;
    for (int i = 0; i < NUM_POWERUPS; i++){
      if ((data.packetsTriggered >> i) & (cooldownsExpired >> 1)) {
        toActivate = toActivate | (0b1 << i);
        cooldownsExpired = cooldownsExpired ^ (0b1 << i);
      }
    }

    if (toActivate > 0){
      CooldownsTriggeredData data;
      data.packetsTriggered = toActivate;
      Packet p = Packet(PowerupActivatePacket).withData(&data).sendable();

      server.sendPacket(GAME_IN, &p);
      server.sendPacket(POWERUP_FB, &p);
    }
}

void handleCooldownExpired(const CooldownsExpiredData& data) {
    for (int i = 0; i < NUM_POWERUPS; i++){
      if ((data.cooldownsExpired >> i) & 0b1) {
          cooldownsExpired = cooldownsExpired | (0b1 << i);
      }
    }
}


void setup() {
    Serial.begin(9600);
    server.initialize();
}

void loop() {
    int packetsRecived = server.readPackets(&packetArr);
    for (int i = 1; i < packetsRecived; i++) {
        managePacket(&packetArr[i]);
    }
}

