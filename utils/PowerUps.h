#ifndef POWER_UPS_H
#define POWER_UPS_H

typedef int PowerUpStatus;
/**
 * This enum enables us to have multiple power ups
 * represented with a single integer with bitwise operations
 *
 * To add a power up just create a new enum value with the
 * next power of two.
 *
 *
 * Example: Checking if a ball is sped up.
 *
 *
 * Ball ball;
 * if (ball.powerUpStatus & PowerUps::BallSpeedUp){
 *    // ball is sped up!
 * } else if (ball.powerUpStatus & PowerUps::BallInvisible){
 *    // ball is invisible!
 * } else {
 *    // neither power up above applies
 * }
 *
 */
enum PowerUps {
  BallSpeedUp = 0,
  BallInvisible = 1,
  PaddleSpeedUp = 2,
};

#endif // !POWER_UPS_H
