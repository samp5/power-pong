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

    // get the status of the WiFi
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
    // write our idenfication to the server as our first packet.
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
  void sendPacket(Packet *p) { this->client.write(p->data); }

  // fill packetsRecieved with the recieved packets
  // return the number of packets recieved
  int readPackets(Packet *packetsRecieved) {
    int packetsRead = 0;

    // while we can read bytes
    while (this->client.available() > 0) {

      // read a byte
      byte b = client.read();

      Serial.print("Client::readPackets read: ");
      Serial.println(b);
      Packet* this_packet = packetsRecieved + packetsRead;

      // setting the raw value of the packet as the value we read
      this_packet->setRaw(b);

      // DEBUG:
      this_packet->print();

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
