#ifndef PACKET_H
#define PACKET_H

enum PacketType {
  PlayerMove,
  GameStatePacket,
  PowerupCDPacket,
};

struct Header {
  PacketType pt;
  int size; // in bytes
  Header(PacketType pt, int size) : pt(pt), size(size) {}
};

struct Packet {
  void send_packet() {
    int bytes = sizeof(*this) + sizeof(Header);
    Header h = Header(packet_type(), sizeof(*this) + sizeof(Header));
    // TODO:
  };
  virtual PacketType packet_type();
};

#endif // !PACKET_H
