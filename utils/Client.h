#ifndef CLIENT
#define CLIENT
#include <WiFi.h>
#include "Network.h"
#include "Packet.h"

struct ClientConnection {
public:
  /**
   * Don't invoke this. this exists purely for arrays or declarations.
   */
  ClientConnection() {}
  /**
   * create a new client connection connected to the given ip address
   */
  ClientConnection(IPAddress ipaddr) {
   int status = WiFi.begin(SSID, PWD);
   if (status != WL_CONNECTED) {
     return;
   }

   this->client.connect(ipaddr, PORT);
  }

  void sendPacket(Packet* p) {
    client.print(p->asBytes());
  }

  bool isConnected() {
    return client.connected();
  }

private:
  WiFiClient client;
};



#endif
