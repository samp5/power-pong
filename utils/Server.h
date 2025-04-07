#ifndef SERVER
#define SERVER
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

      this->listener.client = server.available();
      this->ipAddr = WiFi.localIP();
      this->connected = 1;
    }
  }

  int isConnected() { return this->connected; }

  const IPAddress &getIP() { return this->ipAddr; }
  /**
   * send the given packet to all clients
   */
  void sendPacket(Packet *p) { this->listener.sendPacket(p); }

  /**
   * @param packetsRecieved a packet double pointer which will be given an array
   *   of packets recieved. will contain a maximum of 16 packets
   * @return the number of packets recieved.
   */
  int readPackets(Packet **packetsRecieved) {
    return this->listener.readPackets(packetsRecieved);
  }

  // private:
  WiFiServer server;
  ClientConnection listener;
  IPAddress ipAddr;
  int connected = 0;
};

#endif
