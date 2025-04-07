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
    int status = WiFi.begin(SSID, PWD);
    if (status != WL_CONNECTED) {
      Serial.println("Not connected :(");
      return;
    } else {
      server.begin();
      this->ipAddr = WiFi.localIP();

      Serial.println("Connecting clients");
      int connectedClients = 0;
      while (connectedClients < NUM_CLIENTS) {
        WiFiClient thisClient = server.available();
        if (thisClient) {
          while (thisClient.connected()) {
            if (thisClient.available()) {
              Serial.println("WifiServer::initialize client was available");
              char c = thisClient.read();
              Serial.print("\tgot: ");
              Serial.println((int)c);
              this->clients[c] = thisClient;
              connectedClients++;
              break;
            }
          }
        }
      }
    }
  }

  const IPAddress &getIP() { return this->ipAddr; }
  /**
   * send the given packet to all clients
   */
  // void sendPacket(ClientID id, Packet *p) { this->listener.sendPacket(p); }

  /**
   * @param packetsRecieved a packet double pointer which will be given an array
   *   of packets recieved. will contain a maximum of 16 packets
   * @return the number of packets recieved.
   */
  // int readPackets(Packet **packetsRecieved) {
  //   return this->listener.readPackets(packetsRecieved);
  // }

  // private:
  WiFiServer server;
  IPAddress ipAddr;
  WiFiClient clients[3];
};

#endif
