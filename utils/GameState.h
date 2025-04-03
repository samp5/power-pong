#ifndef GAME_STATE
#define GAME_STATE
#include "Common.h"
#include "Packet.h"
#include "PowerUps.h"
#include "button.h"
#define BALL_START_SPEED 10
#define TICK_RATE 10
#define P1_PONG_BUTTON_UP 0
#define P1_PONG_BUTTON_DOWN 1
#define P2_PONG_BUTTON_UP 2
#define P2_PONG_BUTTON_DOWN 3

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
  int x_comp;
  int y_comp;
  int magnitude() { return sqrt(x_comp * x_comp + y_comp * y_comp); }
};

enum Direction { Up, Down };
enum PlayerID { One, Two };

struct DisplayBounds {
  int height;
  int width;
};

class PongButton : public Button {
public:
  Direction direction;
  PongButton(PlayerID id, Direction dir) : Button(PongButton::getPin(id, dir)) {
    this->direction = dir;
  }

private:
  static int getPin(PlayerID id, Direction dir) {
    static int pinArr[2][2]{{
                                P1_PONG_BUTTON_UP,
                                P1_PONG_BUTTON_DOWN,
                            },
                            {
                                P2_PONG_BUTTON_UP,
                                P2_PONG_BUTTON_DOWN,
                            }};

    return pinArr[id][dir];
  }
};

class PlayerInput {
public:
  PongButton up;
  PongButton down;

  PlayerInput(PlayerID id) : up(id, Direction::Up), down(id, Direction::Down) {}
};

struct Paddle {
  Position paddlePosition;          // represents the center of the paddle
  static const int halfheight = 10; // in pixels
  static const int halfwidth = 10;  // in pixels
  static const int paddlePixelPerMove = 10;

  Paddle() : paddlePosition({0, 0}) {};
  bool inBounds(DisplayBounds bounds, Position newPosition) {
    return ((newPosition.y + this->halfheight) <= bounds.height) &&
           (newPosition.y - this->halfheight >= 0);
  }
  void move(DisplayBounds bounds, Direction move) {
    Position newPostion;
    newPostion.x = this->paddlePosition.x;
    newPostion.y = this->paddlePosition.y;

    switch (move) {
    case Up:
      newPostion.y -= this->paddlePixelPerMove;
      break;
    case Down:
      newPostion.y += this->paddlePixelPerMove;
      break;
    }

    if (inBounds(bounds, newPostion)) {
      this->paddlePosition.x = newPostion.x;
      this->paddlePosition.y = newPostion.y;
    }
  }
};

struct Player {
  int score;
  PowerUpStatus powerUps;
  PlayerInput input;
  Paddle paddle;
  PlayerID id;
  Direction lastMove;
  Player(PlayerID id) : id(id), input(id), score(0), paddle(), powerUps(0) {}

  void increaseScore() { this->score++; }
  const Paddle &getPaddle() { return paddle; }

  void processInput() {
    if (input.up.checkPress()) {
      lastMove = Up;
    }
    if (input.down.checkPress()) {
      lastMove = Down;
    }
  }

  void movePaddle(DisplayBounds bounds) { paddle.move(bounds, lastMove); }
};

struct Ball {
  const int MAX_SPEED = 40;
  const int START_BALL_RADIUS = 2;
  const int INITIAL_SPEED = 10;
  const int INITIAL_V_X = 5;
  const int INITIAL_V_Y = 5;
  int radius;
  Velocity velocity;
  Position postion;
  Ball(Position start) : postion(start), radius(START_BALL_RADIUS) {
    setInitialVelocity();
  }
  void setInitialVelocity() {
    int initial_x = random(1) == 0 ? random(3, 6) : -random(3, 6);
    int remaining_magnitude = INITIAL_SPEED - sqrt(initial_x * initial_x);
    int initial_y = random(1) == 0 ? remaining_magnitude : -remaining_magnitude;
  }

  bool onLeft(DisplayBounds bounds) {
    return this->postion.x < (bounds.width / 2);
  }
  bool onRight(DisplayBounds bounds) {
    return this->postion.x > (bounds.width / 2);
  }

  bool scored(DisplayBounds bounds, PlayerID &id) {
    if (this->postion.x - radius < 0) {
      id = One;
      return true;
    } else if (this->postion.x + radius > bounds.width) {
      id = Two;
      return true;
    } else {
      return false;
    }
  }

  /**
   *
   * Move the position of the ball according to its current position and
   * velocity. This function does not check the tick and only should be called
   * when all game state is being updated
   *
   */
  void move(DisplayBounds bounds, const Paddle &player1Paddle,
            const Paddle &player2Paddle) {
    Position newPosition;
    newPosition.x = this->postion.x + this->velocity.x_comp;
    newPosition.y = this->postion.y + this->velocity.y_comp;

    // check points scored
    // if there are points scored then we can early return
    if (newPosition.x - radius < 0 || newPosition.x + radius > bounds.width) {
      this->postion.x = newPosition.x;
      this->postion.y = newPosition.y;
      return;
    }

    // check collision with top and bottom
    if ((newPosition.y - radius) < 0) {

      // adjust for how out of bounds we are
      int overlap = 0 - (newPosition.y - radius);
      newPosition.y += overlap;

      // flip our y component
      this->velocity.y_comp = -this->velocity.y_comp;

    } else if ((newPosition.y + radius) > bounds.height) {

      // adjust for how out of bounds we are
      int overlap = bounds.height - (newPosition.y + radius);
      newPosition.y -= overlap;

      // flip our y component
      this->velocity.y_comp = -this->velocity.y_comp;
    }

    // check collision with our paddles

    int speed = this->velocity.magnitude();
    // check collision with left paddle
    if ((newPosition.x - (player1Paddle.paddlePosition.x +
                          player1Paddle.halfwidth)) < this->radius &&
        abs(newPosition.y - player1Paddle.paddlePosition.y) <
            (radius + player1Paddle.halfheight)) {

      /**
       *  _____________________
       * |                     |
       * | 🏓🏀                |
       * |                     |
       * |                  🏓 |
       * |_____________________|
       */

      /**
       *
       * For a vector <x,y> with some magnitude m.
       * We can obtain a vector <x', y'> that is in the same direction
       * with a magnitude m' via
       *
       * x' = x * (m' / m)
       * y' = y * (m' / m)
       *
       */
      float dy = newPosition.y - player1Paddle.paddlePosition.y;
      float dx = newPosition.x - player1Paddle.paddlePosition.x +
                 player1Paddle.halfwidth;

      float magnitude = sqrt(dx * dx + dy * dy);

      this->velocity.x_comp = dx * (speed / magnitude);
      this->velocity.y_comp = dy * (speed / magnitude);

    } else if (((player2Paddle.paddlePosition.x - player2Paddle.halfwidth) -
                newPosition.x) < radius &&
               abs(newPosition.y - player2Paddle.paddlePosition.y) <
                   (radius + player2Paddle.halfheight)) {

      /**
       *  _____________________
       * |                     |
       * | 🏓                  |
       * |                     |
       * |                🏀🏓 |
       * |_____________________|
       */

      float dy = newPosition.y - player2Paddle.paddlePosition.y;
      float dx = newPosition.x - player2Paddle.paddlePosition.x +
                 player2Paddle.halfwidth;

      float magnitude = sqrt(dx * dx + dy * dy);

      this->velocity.x_comp = dx * (speed / magnitude);
      this->velocity.y_comp = dy * (speed / magnitude);
    }

    this->postion.x = newPosition.x;
    this->postion.y = newPosition.y;
  }
};

struct GameState {
  DisplayBounds displayBounds;
  Player player1, player2;
  Ball ball;
  PowerUpStatus powerUpStat;
  static const unsigned long tick = TICK_RATE;
  unsigned long last_update;
  GameState(DisplayBounds bounds)
      : displayBounds({bounds.height, bounds.width}), player1(PlayerID::One),
        player2(PlayerID::Two), ball({0, 0}), last_update(0) {}
  void update(unsigned long time);
  void scored(PlayerID);
};

inline void GameState::update(unsigned long time) {
  player1.processInput();
  player2.processInput();

  if (time - last_update < this->tick) {
    return;
  }

  this->last_update = time;
  player1.movePaddle(this->displayBounds);
  player2.movePaddle(this->displayBounds);

  ball.move(this->displayBounds, player1.getPaddle(), player2.getPaddle());

  PlayerID scoringPlayer;
  if (ball.scored(displayBounds, scoringPlayer)) {
    this->scored(scoringPlayer);
  }
}

inline void GameState::scored(PlayerID id) {
  switch (id) {
  case One:
    player1.increaseScore();
    break;
  case Two:
    player2.increaseScore();
    break;
  }

  this->ball.postion.x = this->displayBounds.width / 2;
  this->ball.postion.y = this->displayBounds.height / 2;

  this->ball.postion.x = this->displayBounds.width / 2;
  this->ball.postion.y = this->displayBounds.height / 2;
  this->ball.setInitialVelocity();
}

#endif // !GAME_STATE
