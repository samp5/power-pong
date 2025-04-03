#ifndef SERVER
#define SERVER
#include <WiFi.h>
#include "Packet.h"
#include "Network.h"

struct WifiServer {
public:
  WifiServer() {
    WiFiServer wifiServer(PORT);

    int status = WiFi.begin(SSID, PWD);

    if (status != WL_CONNECTED) {
      return;
    } else {
      wifiServer.begin();

      this->server = wifiServer;
      this->listener = wifiServer.available();
      this->ipAddr = WiFi.localIP();
    }
  }

private:
  WiFiServer server;
  WiFiClient listener;
  IPAddress ipAddr;
};


#endif
