#ifndef CLIENT
#define CLIENT
#include "Network.h"
#include "Packet.h"
#include <WiFiS3.h>

struct ClientConnection {
  friend class WifiServer;

public:
  static const int MAX_INCOMING_PACKETS = 16;

  /**
   * Don't invoke this. this exists purely for arrays or declarations.
   */
  ClientConnection() {}

  /**
   * create a new client connection connected to the given ip address
   */
  ClientConnection(ClientID clientID, IPAddress ipaddr) {
    // connect to Wifi, with retries
    Serial.println("Connecting to WiFi...");
    int status = WiFi.begin(SSID, PWD);
    while (status != WL_CONNECTED) {
      Serial.println("Unable to connect to wifi, trying again...");
      delay(3000); // wait 3s before trying again
      status = WiFi.begin(SSID, PWD);
    }

    // connect to server, with retries
    Serial.println("Connecting to server...");
    while (!this->client.connect(ipaddr, PORT)) {
      Serial.println("Unable to connect to server, trying again...");
    }

    // feedback of connection
    client.write((char)clientID);
    Serial.println("Connected to server");
  }

  /**
   * create a new client connection object using an existing WiFi client
   */
  ClientConnection(WiFiClient client) {
    this->client = client;
  }

  /**
   * send the given packet to the server
   */
  void sendPacket(Packet *p) { this->client.print(p->data); }

  /**
   * @param packetsRecieved a packet double pointer which will be given an array
   *   of packets recieved. will contain a maximum of 16 packets
   * @return the number of packets recieved.
   */
  int readPackets(Packet **packetsRecieved) {
    int packetsRead = 0;
    while (this->client.available() > 0) {
      Packet p;
      byte b = client.read();
      Serial.print("Client::readPackets read: ");
      Serial.println(b);

      p.setData(b);
      Serial.print("Packet now contains: ");
      Serial.println(p.data);

      *(packetsRecieved[packetsRead]) = p;
      ++packetsRead;
    }

    return packetsRead;
  }

  bool isConnected() { return client.connected(); }

private:
  WiFiClient client;
  Packet incomingPackets[MAX_INCOMING_PACKETS];
};

#endif
