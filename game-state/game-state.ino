#include "utils/Server.h"
#include "utils/PowerUps.h"
#include "utils/Client.h"
#include "utils/Packet.h"

#define NUM_POWERUPS 5

WifiServer server;
Packet packetArr[NUM_CLIENTS * ClientConnection::MAX_INCOMING_PACKETS];

struct PowerupState {
    bool available = true;
    unsigned long cooldownEndTime = 0;
    int cooldownDuration = 10000; // 10 seconds for cool-downs(?)
};

struct {
    PowerupState powerups[NUM_POWERUPS];
} gameState;

unsigned long previousTime = 0;
const long interval = 15;


void setup() {
    server.initialize();
}

void loop() {
    unsigned long currentTime = millis();
    if (currentTime - previousTime < interval){
      return;
    }
    
    previousTime = currentTime;

    for (int clientId = 0; clientId < NUM_CLIENTS; clientId++) {
        if (!server.clients[clientId].isConnected()){
          continue;
        }

        Packet* clientPackets = &packetArr[clientId * ClientConnection::MAX_INCOMING_PACKETS];
        int packets = server.getClient(clientId).readPackets(clientPackets);

        for (int i = 0; i < packets; i++) {
            managePacket(clientId, &clientPackets[i]);
        }
    }

    updatePowerupStates(currentTime);
}

void managePacket(int clientId, Packet* packet) {
    switch (packet->getType()) {
        case PowerupActivatePacket: {
            PowerupActivatePacket powerUpData;
            packet->toStruct(&powerUpData);

            int cooldown = gameState.powerups[powerUpData.powerupType].cooldownDuration;
          
            handlePowerupActivation(clientId, powerUpData.powerupType, cooldown);
            break;
        }
        case PowerupCDPacket:
            sendPowerupStatus(clientId);
            break;
    }
}

void handlePowerupActivation(int clientId, int powerupType, int cooldownDuration) {
    if (!gameState.powerups[powerupType].available) {
        sendCooldownStatus(powerupType);
        return;
    }

    gameState.powerups[powerupType].available = false;
    gameState.powerups[powerupType].cooldownEndTime = millis() + cooldownDuration;

    PowerupActivatePacket activation = {(uint8_t)powerupType};
    server.sendPacket(GAME_IN, &activation); // Tell the game input controller to activate the power-up

    PowerupCDPacket cooldown = {(uint8_t)powerupType, (unsigned long)cooldownDuration};
    server.sendPacket(POWERUP_FB, &cooldown); // Notify the feedback arduino that this power-up just went on cooldown
}

void sendCooldownStatus(int powerupType) {
    long remaining = max(0, (long)(gameState.powerups[powerupType].cooldownEndTime - millis()));
    PowerupCDPacket cooldown = { (uint8_t)powerupType, (unsigned long)remaining};
    server.sendPacket(POWERUP_FB, &cooldown); // Notify the feedback arduino that the power-up is still cooling down
}

void updatePowerupStates(unsigned long currentTime) {
    for (int i = 0; i < NUM_POWERUPS; i++) {
        if (gameState.powerups[i].available || currentTime < gameState.powerups[i].cooldownEndTime){
          continue;
        }
        
        gameState.powerups[i].available = true;
        PowerupAvailablePacket available = { (uint8_t)i};
        server.sendPacket(POWERUP_FB, &available); // Notify the feedback arduinothat the power-up is available again
    }
}

void sendPowerupStatus(int clientId) {
    PowerupStatusPacket status;
    for (int i = 0; i < NUM_POWERUPS; i++) {
        status.available[i] = gameState.powerups[i].available;
        status.remaining[i] = gameState.powerups[i].available ? 0 : max(0, (long)(gameState.powerups[i].cooldownEndTime - millis()));
    }
    server.sendPacket(clientId, &status); // Respond to the client that requested the powerup status(?)
}
