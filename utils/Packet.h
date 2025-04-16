#ifndef PACKET_H
#define PACKET_H

enum PacketType {
  PowerupCDPacket,
  PowerupActivatePacket,
};

#define TYPE_BIT 7

struct Packet {
  char data = 0;

  Packet (){
  }

  Packet (PacketType t){
    switch (t) {
    case PowerupCDPacket:
      data |= (0b1 << TYPE_BIT);
    }
  }
  void setData(int data){
    this->data |= data;
  }
  PacketType getType(){
    if (this->data & (0b1 << TYPE_BIT)){
      return PowerupCDPacket;
    } else {
      return PowerupActivatePacket;
    }
  }
  int getData() {
    return data & 0b01111111;
  }
};

#endif // !PACKET_H
