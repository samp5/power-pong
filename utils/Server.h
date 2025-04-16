#ifndef SERVER
#define SERVER
#define NUM_CLIENTS 3
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
    while (this->connectedClients != wantedClients) {
      Serial.println("watned clients is:");
      Serial.println(wantedClients);
      WiFiClient client = server.available();
      if (client) {
        Serial.println("client is non null");
        while (client.connected()) {
          Serial.println("client is connected");
          if (client.available()) {
            ClientID c = (ClientID) client.read();
            Serial.println("read");
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
            Serial.println("Its all good now");

            connectedClients |= 1 << c;
            this->clients[c] = ClientConnection(client);
            break;
          }
        }
      }else {
      Serial.println("didn't enter if");
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

  /**
   * @param packetsRecieved a packet double pointer which will be given an array
   *   of packets recieved. will contain a maximum of 48 packets
   * @return the number of packets recieved.
   */
  int readPackets(Packet **packetsRecieved) {
    int numPackets = 0;
    for (int i = 0; i < NUM_CLIENTS; ++i) {
      numPackets += clients[i].readPackets(packetsRecieved + numPackets);
    }
    return numPackets;
  }

  private:
  WiFiServer server;
  IPAddress ipAddr;
  ClientConnection clients[3];

  static const int wantedClients = (1 << NUM_CLIENTS) - 1;
  int connectedClients;
};

#endif
