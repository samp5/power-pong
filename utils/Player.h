#pragma once
#include "GameState.h"
#include "Packet.h"
#include "PowerUps.h"
#ifndef PLAYER_H
#define PLAYER_H
#define P1_PONG_BUTTON_UP 0
#define P1_PONG_BUTTON_DOWN 1
#define P2_PONG_BUTTON_UP 2
#define P2_PONG_BUTTON_DOWN 3

#include "button.h"

unsigned long millis();

enum Direction { Up, Down };

struct PlayerMoveData : public Packet {
  Direction dir;
  PlayerID id;
  unsigned long time;

  PlayerMoveData(Direction dir, PlayerID id, unsigned long time) {
    this->dir = dir;
    this->id = id;
    this->time = time;
  }
  PacketType packet_type() override {
    return PacketType::PlayerMove;
  }
};

class PongButton : public Button {
public:
  Direction direction;
  PongButton(PlayerID id, Direction dir) : Button(PongButton::getPin(id, dir)) {
    this->direction = dir;
    this->setPin(PongButton::getPin(id, dir), INPUT);
  }

private:
  static int getPin(PlayerID id, Direction dir) {
    static int pinArr[2][2] {
      {
        P1_PONG_BUTTON_UP,
        P1_PONG_BUTTON_DOWN,
      }, {
        P2_PONG_BUTTON_UP,
        P2_PONG_BUTTON_DOWN,
      }
    };

    return pinArr[id][dir];
  }
};

class PlayerInput {
public:
  PongButton up;
  PongButton down;

  PlayerInput(PlayerID id) : up(id, Direction::Up), down(id, Direction::Down) {}
};

class Player {
  PlayerInput input;
  PlayerID id;
  PlayerState state;

public:
  Player(PlayerID id) : id(id), input(id), state(id) {}

  void send_input() {
    if (input.up.checkPress()) {
      PlayerMoveData(Direction::Up, this->id, millis()).send_packet();
    } else if (input.down.checkPress()) {
      PlayerMoveData(Direction::Down, this->id, millis()).send_packet();
    }
  }
};

#endif // !PLAYER_H
