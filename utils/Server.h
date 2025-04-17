#ifndef SERVER
#define SERVER
#define NUM_CLIENTS 1
#include "Client.h"
#include "Network.h"
#include "Packet.h"
#include <WiFiS3.h>

struct WifiServer {
public:
  WifiServer() : server(PORT) {}

  void initialize() {
    // connect to Wifi, with retries
    Serial.println("Connecting to WiFi...");
    int status = WiFi.begin(SSID, PWD);
    while (status != WL_CONNECTED) {

      Serial.println("Unable to connect to wifi, trying again...");

      delay(3000); // wait 3s before trying again
      status = WiFi.begin(SSID, PWD);
    }

    // start the server, get the ip
    server.begin();
    this->ipAddr = WiFi.localIP();

    // notify server status
    Serial.print("Waiting for clients on host ");

    Serial.print(this->ipAddr);
    Serial.print(":");
    Serial.println(PORT);

    // wait for clients to connect
    this->connectedClients = 0;
    getClients();
  }

  /**
   * connect to all clients
   */
  void getClients() {
    while (this->connectedClients != 1) {
      WiFiClient client = server.available();
      if (client) {
        while (client.connected()) {
          if (client.available()) {
            ClientID c = (ClientID) client.read();
            Serial.println("Read Client ID: ");
            Serial.println(c);
            
            switch (c) {
              case GAME_IN:
                Serial.println("Connected to Game Input");
                break;
              case POWERUP_IN:
                Serial.println("Connected to Powerup Input");
                break;
              case POWERUP_FB:
                Serial.println("Connected to Powerup Feedback");
                break;
            }

            connectedClients = 1;
            this->clients[c] = ClientConnection(client);
            break;
          }
        }
      }
    }
  }

  /**
   * get the IP of the server
   */
  const IPAddress &getIP() { return this->ipAddr; }

  /**
   * send the given packet to the specified client
   */
  void sendPacket(ClientID id, Packet *p) {
    this->clients[id].sendPacket(p);
  }

  // read packets from all clients into the array
  // return the number of packets placed into that array
  int readPackets(Packet *packetsRecieved) {
    int packetsRead = 0;
    for (int i = 0; i < NUM_CLIENTS; ++i) {
      int numReadClient_i = clients[i].readPackets(packetsRecieved + packetsRead);
      packetsRead += numReadClient_i;
    }
    if (packetsRead > 0){
      Serial.print("Server::readPackets num packets is ");
      Serial.println(packetsRead);
    }
    return packetsRead;
  }

  private:
  WiFiServer server;
  IPAddress ipAddr;
  ClientConnection clients[3];

  static const int wantedClients = (1 << NUM_CLIENTS) - 1;
  int connectedClients;
};

#endif
