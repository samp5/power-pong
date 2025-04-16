#ifndef PACKET_H
#define PACKET_H
#include <cstring>
#include <string.h>
#define PACKET_MAX_SZ 60
#define PACKET_SIZE_TYPE unsigned int
#define PACKET_SIZE_TYPE_SIZE sizeof(PACKET_SIZE_TYPE)
#define PACKET_TYPE_SIZE sizeof(PacketType)
#define NUM_POWERUPS 5

enum PacketType {
  PlayerMove,
  GameStatePacket,
  PowerupCDPacket,
  PowerupActivatePacket,
  PowerupAvailablePacket,
  PowerupStatusPacket,
  PACKET_COUNT, // always have last to keep size
};


struct PacketData {};

struct Packet {
public:
  /**
   * Don't invoke this. this exists purely for arrays, sending a packet from
   * this constructor has undefined behavior
   */
  Packet() {
    this->type = (PacketType) -1;
  }

  Packet(PacketType type) {
    this->type = type;
  }

  PacketType getType() {
    return this->type;
  }

  /**
   * part of the packet factory.
   * sets the `data` of the packet, along with the size.
   * to give the data, make a struct that extends `PacketData` and pass by
   *   pointer.
   */
  virtual Packet withData(PacketData* data) {
    this->size = sizeof(*data);
    memcpy(this->data, data, this->size);
    return *this;
  }

  /**
   * part of the packet factory, assembles the packet into its proper format
   */
  Packet sendable() {
    // not really necessary, but I dont want to hardcode them in case
    // that they get changed to different types
    size_t typeSz = sizeof(this->type);
    size_t sizeSz = sizeof(this->size);

    // write all the data
    memcpy(formattedData, &this->type, typeSz);
    memcpy(formattedData+typeSz, &this->size, sizeSz);
    memcpy(formattedData+typeSz+sizeSz, this->data, this->size);
    
    return *this;
  }

  /**
   * functions after the packet factory, converts the packet into bytes to be
   *   sent accross a connection
   */
  char* asBytes() {
    return formattedData;
  }

  /**
   * part of the packet decoding factory.
   * takes bytes and places them into the `data` field
   */
  Packet fromBytes(char* data, int size) {
    memcpy(this->data, data, size);
    this->size = size;
    return *this;
  }

  /**
   * part of the packet decoding factory.
   * given a struct, places current data into the struct
   */
  void toStruct(PacketData* destination) {
    memcpy(destination, this->data, this->size);
  }

protected:
  char data[PACKET_MAX_SZ];
  PACKET_SIZE_TYPE size;

private:
  PacketType type;
  char formattedData[PACKET_MAX_SZ + (2 * sizeof(int))];
};

#endif // !PACKET_H
