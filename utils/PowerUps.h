#ifndef POWER_UPS_H
#define POWER_UPS_H
#include "Packet.h"

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
  BallSpeedUp   = 0, // Joystick
  BallInvisible = 1, // Photores
  PaddleSpeedUp = 2, // Motor
  BallSize      = 4, // Distance Sensor
  BonusPoints   = 8, // Num Pad
};

/**
 * Packet that is send from the Powerup Feedback.
 * 
 * `cooldownsExpired` is an int with each bit corresponding to each powerup.
 * if that powerup's bit is 1, the powerup is now available. if it is 0, ignore.
 *
 * Example: BallSize's cooldown expired
 *
 * cooldownsExpired = 0b01000;
 *
 * this means that just this cooldown expired. it does not mean that others are
 * not available.
 */
struct PowerupPacket : Packet {
  int cooldownsExpired;
  PacketType packet_type() override {
    return PacketType::PowerupCDPacket;
  }
};

#endif // !POWER_UPS_H
