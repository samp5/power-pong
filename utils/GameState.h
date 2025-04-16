#ifndef GAME_STATE
#define GAME_STATE
#include "Common.h"
#include "Packet.h"
#include "PowerUps.h"
#include "button.h"
#include <cstdio>
#define BALL_START_SPEED 4
#define TICK_RATE 50
#define P1_PONG_BUTTON_UP 2
#define P1_PONG_BUTTON_DOWN 3
#define P2_PONG_BUTTON_UP 12
#define P2_PONG_BUTTON_DOWN 13

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
  float x_comp;
  float y_comp;
  float magnitude() { return sqrt(x_comp * x_comp + y_comp * y_comp); }
};

enum Direction { Up, Down, None };
enum PlayerID { One, Two };

struct DisplayBounds {
  int width;
  int height;
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
  Position position;               // represents the center of the paddle
  static const int halfheight = 8; // in pixels
  static const int halfwidth = 1;  // in pixels
  static const int paddlePixelPerMove = 2;

  Paddle() : position({0, 0}) {};
  bool inBounds(DisplayBounds bounds, Position newPosition) {
    return ((newPosition.y + this->halfheight) <= bounds.height) &&
           (newPosition.y - this->halfheight >= 0);
  }

  void move(DisplayBounds bounds, Direction move) {
    Position newPostion;
    newPostion.x = this->position.x;
    newPostion.y = this->position.y;

    switch (move) {
    case Up:
      newPostion.y -= this->paddlePixelPerMove;
      break;
    case Down:
      newPostion.y += this->paddlePixelPerMove;
      break;
    case None:
      break;
    }

    if (inBounds(bounds, newPostion)) {
      this->position.x = newPostion.x;
      this->position.y = newPostion.y;
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
  Player(PlayerID id)
      : id(id), input(id), score(0), paddle(), powerUps(0), lastMove(None) {}

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
  const int INITIAL_SPEED = BALL_START_SPEED;
  const int INITIAL_V_X = 5;
  const int INITIAL_V_Y = 5;
  int radius;
  Velocity velocity;
  Position postion;

  Ball(Position p) : postion(p), radius(START_BALL_RADIUS) {
    setInitialVelocity();
  }
  void setInitialVelocity() {
    float initial_x =
        random(2) == 0
            ? random(INITIAL_SPEED / 2, INITIAL_SPEED - INITIAL_SPEED / 4)
            : -random(INITIAL_SPEED / 2, INITIAL_SPEED - INITIAL_SPEED / 4);

    float remaining_magnitude = INITIAL_SPEED - abs(initial_x);
    float initial_y =
        random(2) == 0 ? remaining_magnitude : -remaining_magnitude;

    velocity.x_comp = initial_x;
    velocity.y_comp = initial_y;
  }

  bool scored(DisplayBounds bounds, PlayerID &id) {
    if (this->postion.x - radius < 0) {
      id = Two;
      return true;
    } else if (this->postion.x + radius > bounds.width) {
      id = One;
      return true;
    } else {
      return false;
    }
  }

  Position applyVelocity() {
    return {static_cast<int>(this->postion.x + this->velocity.x_comp),
            static_cast<int>(this->postion.y + this->velocity.y_comp)};
  }

  /**
   *
   * Move the position of the ball according to its current position and
   * velocity. This function does not check the tick and only should be called
   * when all game state is being updated
   *
   *
   */
  void move(DisplayBounds bounds, const Paddle &player1Paddle,
            const Paddle &player2Paddle) {
    Position newPosition = applyVelocity();

    char buf[1024];

    // check collision with top and bottom
    if ((newPosition.y - radius) < 0) {
      // adjust for how out of bounds we are
      int overlap = 0 - (newPosition.y - radius);
      newPosition.y += overlap;

      // flip our y component
      this->velocity.y_comp = -this->velocity.y_comp;

    } else if ((newPosition.y + radius) >= bounds.height) {

      // adjust for how out of bounds we are
      int overlap = bounds.height - (newPosition.y + radius);

      snprintf(buf, 1024, "\tAdjusted for overlap: (%d,%d) -> (%d,%d)",
               newPosition.x, newPosition.y, newPosition.x,
               newPosition.y - overlap);
      newPosition.y -= overlap;

      // flip our y component
      this->velocity.y_comp = -1 * this->velocity.y_comp;
    }

    // check collision with our paddles

    /**
     *  _____________________
     * |                     |
     * | 🏓🏀                |
     * |                     |
     * |                  🏓 |
     * |_____________________|
     */
    int p1_distX = abs(newPosition.x - player1Paddle.position.x);
    bool p1_colidesX = p1_distX < radius + player1Paddle.halfwidth;

    int p1_distY = abs(newPosition.y - player1Paddle.position.y);
    int p1_collidesY = p1_distY < radius + player1Paddle.halfheight;

    /**
     *  _____________________
     * |                     |
     * | 🏓                  |
     * |                     |
     * |                🏀🏓 |
     * |_____________________|
     */
    int p2_distX = abs(player2Paddle.position.x - newPosition.x);
    bool p2_collidesX = p2_distX < radius + player2Paddle.halfwidth;

    int p2_distY = abs(newPosition.y - player2Paddle.position.y);
    bool p2_collidesY = p2_distY < (radius + player2Paddle.halfheight);

    // check collision with left paddle
    if (p1_colidesX && p1_collidesY) {

      float speed = this->velocity.magnitude();

      // adjust our x position to not overlap with the paddle
      newPosition.x += p1_distX;

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
      float dy = newPosition.y - player1Paddle.position.y;
      float dx = p1_distX;

      float magnitude = sqrt(dx * dx + dy * dy);

      this->velocity.x_comp = dx * (speed / magnitude);
      this->velocity.y_comp = dy * (speed / magnitude);

    } else if (p2_collidesX && p2_collidesY) {
      float speed = this->velocity.magnitude();

      // adjust our x position to not overlap with the paddle
      newPosition.x -= p2_distX;

      float dy = newPosition.y - player2Paddle.position.y;
      float dx = p2_distX;

      float magnitude = sqrt(dx * dx + dy * dy);

      this->velocity.x_comp = -dx * (speed / magnitude);
      this->velocity.y_comp = dy * (speed / magnitude);
    }

    this->postion = newPosition;
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
      : displayBounds({bounds.width, bounds.height}), player1(PlayerID::One),
        player2(PlayerID::Two), last_update(0),
        ball({bounds.width / 2, bounds.height / 2}) {
    player1.paddle.position = {player1.paddle.halfwidth * 2 + 4,
                               (bounds.height / 2) - player1.paddle.halfheight};
    player2.paddle.position = {bounds.width - player2.paddle.halfwidth * 2 - 4,
                               (bounds.height / 2) - player2.paddle.halfheight};
  }
  bool update(unsigned long time) {

    player1.processInput();
    player2.processInput();

    if (time - last_update < this->tick) {
      return false;
    }

    this->last_update = time;

    player1.movePaddle(this->displayBounds);
    player2.movePaddle(this->displayBounds);

    ball.move(this->displayBounds, player1.getPaddle(), player2.getPaddle());

    PlayerID scoringPlayer;
    if (ball.scored(displayBounds, scoringPlayer)) {
      this->scored(scoringPlayer);
      return true;
    }

    if (abs(ball.velocity.x_comp) < 1) {
      ball.velocity.x_comp = ball.velocity.x_comp / abs(ball.velocity.x_comp);
    }

    return true;
  }
  void scored(PlayerID);
};

inline void GameState::scored(PlayerID id) {
  switch (id) {
  case One:
    player1.increaseScore();
    break;
  case Two:
    player2.increaseScore();
    break;
  }

  this->ball.postion = {displayBounds.width / 2, displayBounds.height / 2};
  this->ball.setInitialVelocity();
}

#endif // !GAME_STATE
