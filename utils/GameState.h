#ifndef GAME_STATE
#define GAME_STATE
#include "Packet.h"
#include "PowerUps.h"
#include <cstdlib>
struct PlayerState;
#define BALL_START_SPEED 10
#define TICK_RATE 10

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
  Velocity() : angle(rand() > 2.5 ? 180 : 0), magnitude(BALL_START_SPEED) {}
};

struct Ball {
  const int MAX_SPEED = 40;
  Velocity velocity;
  Position postion;
  Ball(Position start) : velocity(), postion(start) {}
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
  static const unsigned long tick = TICK_RATE;
  unsigned long last_update;
  GameState()
      : player1(PlayerID::One), player2(PlayerID::Two), ball({0, 0}),
        last_update(0) {}
};

#endif // !GAME_STATE
