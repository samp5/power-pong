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
  ClientConnection(IPAddress ipaddr) {
    Serial.println("Connecting to WiFi...");
    int status = WiFi.begin(SSID, PWD);
    while (status != WL_CONNECTED) {
      Serial.println("Unable to connect to wifi, trying again...");
      delay(10000); // wait 10s before trying again
      status = WiFi.begin(SSID, PWD);
    }

    Serial.println("Connecting to server...");
    while (!this->client.connect(ipaddr, PORT)) {
      Serial.println("Unable to connect to server, trying again...");
    }

    Serial.println("Connected to server");
  }

  /**
   * send the given packet to the server
   */
  void sendPacket(Packet *p) { this->client.print(p->asBytes()); }

  /**
   * @param packetsRecieved a packet double pointer which will be given an array
   *   of packets recieved. will contain a maximum of 16 packets
   * @return the number of packets recieved.
   */
  int readPackets(Packet **packetsRecieved) {
    int packetsRead = 0;
    while (this->client.available() > 0) {
      // parse the header, byte by byte because you can't any other way
      char typeArr[PACKET_TYPE_SIZE];
      for (int i = 0; i < PACKET_TYPE_SIZE; ++i) {
        typeArr[i] = this->client.read();
      }
      char sizeArr[PACKET_SIZE_TYPE_SIZE];
      for (int i = 0; i < PACKET_SIZE_TYPE_SIZE; ++i) {
        sizeArr[i] = this->client.read();
      }

      // convert the bytes to their correct types
      PacketType type;
      PACKET_SIZE_TYPE size;
      memcpy(&type, typeArr, PACKET_TYPE_SIZE);
      memcpy(&size, sizeArr, PACKET_SIZE_TYPE_SIZE);

      // read the amount of bytes for this packet
      char packetData[size];
      for (int i = 0; i < size; ++i) {
        packetData[size] = this->client.read();
      }

      // convert the bytes into a packet
      Packet p = Packet(type).fromBytes(packetData, size);

      // place into array
      this->incomingPackets[packetsRead] = p;
      ++packetsRead;
    }

    // set the packetsRecieved to packets read
    *packetsRecieved = this->incomingPackets;
    return packetsRead;
  }

  bool isConnected() { return client.connected(); }

// private:
  WiFiClient client;
  Packet incomingPackets[MAX_INCOMING_PACKETS];
};

#endif
