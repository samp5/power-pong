#include <WiFi.h>
#include "Packet.h"
#include "Network.h"

struct WifiServer {
public:
  static const int MAX_PACKETS = 16;

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

  /**
    * @param packetsRecieved a packet double pointer which will be given an array
    *   of packets recieved. will contain a maximum of 16 packets
    * @return the number of packets recieved.
    */
  int readPackets(Packet** packetsRecieved) {
    int packetsRead = 0;
    while (this->listener.available() > 0) {
     // parse the header, byte by byte because you can't any other way
     char typeArr[PACKET_TYPE_SIZE];
     for (int i = 0; i < PACKET_TYPE_SIZE; ++i) {
       typeArr[i] = this->listener.read();
     }
     char sizeArr[PACKET_SIZE_TYPE_SIZE];
     for (int i = 0; i < PACKET_SIZE_TYPE_SIZE; ++i) {
       sizeArr[i] = this->listener.read();
     }

     // convert the bytes to their correct types
     PacketType type;
     PACKET_SIZE_TYPE size;
     memcpy(&type, typeArr, PACKET_TYPE_SIZE);
     memcpy(&size, sizeArr, PACKET_SIZE_TYPE_SIZE);

     // read the amount of bytes for this packet
     char packetData[size];
     for (int i = 0; i < size; ++i) {
       packetData[size] = this->listener.read();
     }

     // convert the bytes into a packet
     Packet p = Packet(type).fromBytes(packetData, size);

     // place into array
     this->packets[packetsRead] = p;
     ++packetsRead;
    } 

    // set the packetsRecieved to packets read
    *packetsRecieved = this->packets;
    return packetsRead;
  }

private:
  WiFiServer server;
  WiFiClient listener;
  IPAddress ipAddr;

  Packet packets[MAX_PACKETS];
};
