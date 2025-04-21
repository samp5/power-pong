#ifndef PACKET_H
#define PACKET_H

#include "PowerUps.h"
enum PacketType {
  PowerupCDPacket,
  PowerupActivatePacket,
};

String packetTypeToString(PacketType type){
  if (type == PowerupCDPacket){
    return "PowerupCDPacket";
  } else {
    return "PowerupActivatePacket";
  }
}

#define TYPE_BIT 7

struct Packet {
  byte data = 0;

  Packet (){}

  Packet (PacketType t){
    switch (t) {
    case PowerupCDPacket:
      data |= (0b1 << TYPE_BIT);
    case PowerupActivatePacket:
      break;
    }
  }

  void setData(byte data){
    // remove that MSB
    data &= 0b01111111;
    this->data |= data;
  }

  void setType(PacketType t){
    switch (t) {
    case PowerupCDPacket:
      this->data |= (0b1 << TYPE_BIT);
    case PowerupActivatePacket:
      this->data &= 0b01111111;
      break;
    }
  }

  PacketType getType(){
    if (this->data & (0b1 << TYPE_BIT)){
      return PowerupCDPacket;
    } else {
      return PowerupActivatePacket;
    }
  }
  void print(){
      Serial.print("Type is: " + packetTypeToString(this->getType()));
      Serial.print(String(" Raw Data is:") +  String((int)this->data)  + String(" (") +   this->toBinaryString() + String(")") + " Powerup is ");
      Serial.println(powerUpAsString());
  }
  String powerUpAsString(){
        if (getData() & PowerUps::BallSpeedUp){
          return String("BallSpeedUp");
        } else if (getData() & PowerUps::PaddleSpeedUp){
          return String("PaddleSpeedUp");
        } else if (getData() & PowerUps::BallSize){
          return String("BallSize");
        } else if (getData() & PowerUps::BallInvisible){
          return String("BallInvisible");
        } else if (getData() & PowerUps::BonusPoints){
          return String("BonusPoints");
        }
  }

  String toBinaryString(){
    // base of string
    String s = "0b";
    // loop from the MSB to LSB
    for (int i = 7; i >= 0; i--){
      if (this->data & (0b1 << i)){
        s += "1";
      }else {
        s += "0";
      }
    }
    return s;
  }
  byte getData() {
    return data & 0b01111111;
  }

  byte getRaw(){
    return data;
  }
  void setRaw(byte raw){
    this->data = raw;
  }
};

#endif // !PACKET_H
