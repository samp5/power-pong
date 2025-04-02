#ifndef GAME_STATE
#define GAME_STATE
#include "Packet.h"
#include "PowerUps.h"
struct PlayerState;

/**
 * The gameboard looks something like this
 *
 * (0,0)
 *  _____________________
 * |                     |
 * | 🏓                  |
 * |            🏀       |
 * |                  🏓 |
 * |                     |
 * |                     |
 * |_____________________|
 *                    (max_w,max_h)
 */

struct Position {
  int x;
  int y;
};

struct Velocity {
  int angle;     // value between 0 and 360 (inclusive) indicating direction
  int magnitude; // value indicating the speed in pixels per tick
};

struct Ball {
  const int MAX_SPEED = 40;
  Velocity velocity;
  Position postion;
};

enum PlayerID { One, Two };

struct PlayerState {
  PlayerID id;
  int score;
  PowerUpStatus powerUps;
  Position paddlePosition; // represents the top left corner of the paddle

  PlayerState(PlayerID id)
      : id(id), score(0), paddlePosition({0, 0}), powerUps(0) {}
};

struct GameState {
  PlayerState player1, player2;
  Ball ball;
  PowerUpStatus powerUpStat;
  GameState() : player1(PlayerID::One), player2(PlayerID::Two) {}
};

struct GameStateData : public Packet {
  GameState state;
  PacketType packet_type() override { return PacketType::GameStatePacket; }
};

#endif // !GAME_STATE
