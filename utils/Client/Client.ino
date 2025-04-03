#include <WiFi.h>
#include "Network.h"
#include "Packet.h"

struct ClientConnection {
public:
  ClientConnection(IPAddress ipaddr);
  void sendPacket(Packet* p);
  bool isConnected();

private:
  WiFiClient client;
};

ClientConnection::ClientConnection(IPAddress ipaddr) {
  int status = WiFi.begin(SSID, PWD);
  if (status != WL_CONNECTED) {
    return;
  }

  this->client.connect(ipaddr, PORT);
}

void ClientConnection::sendPacket(Packet* p) {
  client.print(p->asBytes());
}

bool ClientConnection::isConnected() {
  return client.connected();
}
