#ifndef SERVER
#define SERVER
#include "Client.h"
#include "Network.h"
#include "Packet.h"
#include <WiFi.h>

struct WifiServer {
public:
  WifiServer() : server(PORT) {
    int status = WiFi.begin(SSID, PWD);

    if (status != WL_CONNECTED) {
      return;
    } else {
      this->server.begin();
      this->listener = server.available();
      this->ipAddr = WiFi.localIP();
    }
  }

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

private:
  WiFiServer server;
  ClientConnection listener;
  IPAddress ipAddr;
};

#endif
