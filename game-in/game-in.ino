#include "utils/Player.h"
#include "utils/GameState.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1  // Reset pin # (or -1 if sharing Arduino reset pin)

GameState STATE;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


void drawStartScreen(String intro) {
  display.clearDisplay();
  display.drawRoundRect(0, 0, display.width(), display.height(), display.height() / 4, WHITE);
  display.setTextSize(1);
  display.setTextColor(WHITE);

  int16_t radius = 3;
  display.drawCircle(display.width() / 2, display.height() / 2 - radius - 4, radius, WHITE);

  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(intro, 0, 0, &x1, &y1, &w, &h);
  display.setCursor(display.width() / 2 - (w / 2), display.height() / 2);
  display.println(intro);

  int16_t x2, y2;
  uint16_t w2, h2;
  String secondLine = "Press any key...";
  display.getTextBounds(secondLine, 0, 0, &x2, &y2, &w2, &h2);
  display.setCursor(display.width() / 2 - (w2 / 2), display.height() / 2 + h + 4);
  display.println(secondLine);

  display.display();

  // for (;;) {
  //   for (int i = 0; i < num_buttons; i++) {
  //     Button *curr = &buttons[i];
  //     curr->state = digitalRead(curr->pin);
  //     if (curr->prev_state == LOW && curr->state == HIGH) {
  //       curr->prev_state = curr->state;
  //       return;
  //     }
  //     curr->prev_state = curr->state;
  //   }
  // }
}

void initState();

void setup() {
  initState();
  drawStartScreen("POWER PONG!");
}

void initState() {
  STATE.ball.postion = { display.width() / 2, display.height() / 2 };
  STATE.powerUpStat = 0;
}

void drawPaddle(Position p) {
  // TODO:
  display.drawRoundRect(p.x, p.y, 10, 10, 5, WHITE);
}

void drawBall(Position p) {
  // TODO:
}

void drawScoreBoard(int player1Score, int player2Score) {
  // TODO:
}

void drawPowerUpUpdates(PowerUpStatus newStatus) {
  // TODO:
}

void updateDisplay() {
  display.clearDisplay();
  drawPaddle(STATE.player1.paddlePosition);
  drawPaddle(STATE.player2.paddlePosition);
  drawBall(STATE.ball.postion);
  drawScoreBoard(STATE.player1.score, STATE.player2.score);
  drawPowerUpUpdates(STATE.powerUpStat);
  display.display();
}


void loop() {
  updateDisplay();
  STATE.ball.update();
}
