#include "utils/GameState.h"
#include "utils/Server.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1  // Reset pin # (or -1 if sharing Arduino reset pin)

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
GameState STATE({ SCREEN_WIDTH, SCREEN_HEIGHT });
WifiServer server;

void initState();

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


void setup() {
  delay(2000);

  initState();

  Serial.begin(9600);
  Serial.println("Started serial");

  server.initialize();
  Serial.print("Server status: ");
  Serial.print(WiFi.status());
  Serial.print("\n");
  Serial.println(server.getIP());

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);  // Don't proceed, loop forever
  }
  display.display();
  delay(1000);  // Pause for 2 seconds

  STATE.update(millis());
  display.display();

  // drawStartScreen("POWER PONG!");
}

void initState() {
  STATE.powerUpStat = 0;
}

void drawPaddle(const Paddle& p) {
  // position refers to the center of the rectangle, so we need to adjust since we draw at the upper lefthand corner
  display.drawRoundRect(p.position.x, p.position.y - p.halfheight, p.halfwidth * 2, p.halfheight * 2, 2, WHITE);
}

void drawBall(const Ball& b) {
  display.drawCircle(b.postion.x, b.postion.y, b.radius, WHITE);
}

void drawScoreBoard(int player1Score, int player2Score) {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  int16_t x, y;
  uint16_t w2, h2;
  char secondLine[16];
  sprintf(secondLine, "%d - %d", player1Score, player2Score);
  display.getTextBounds(secondLine, 0, 0, &x, &y, &w2, &h2);
  display.setCursor(display.width() / 2 - (w2 / 2), h2 + 3);
  display.println(secondLine);
}

void drawPowerUpUpdates(PowerUpStatus newStatus) {
  // TODO:
}

void updateDisplay() {
  display.clearDisplay();
  drawPaddle(STATE.player1.getPaddle());
  drawPaddle(STATE.player2.getPaddle());
  drawBall(STATE.ball);
  drawScoreBoard(STATE.player1.score, STATE.player2.score);
  drawPowerUpUpdates(STATE.powerUpStat);
  display.display();
}


void loop() {
  if (STATE.update(millis())) {
    updateDisplay();
  }

  WiFiClient client = server.clients[0];
  if (client && client.connected() && client.available()) {
    client.println("I could be sending you a packet right now");
    // Serial.println("Building packet");
    // CooldownsTriggeredData d;
    // d.packetsTriggered = 1;
    // Packet p = Packet(PowerupActivatePacket).withData(&d).sendable();
    // Serial.println("Sending packet");
    // server.sendPacket(,&p);
    // Serial.println("Packet sent");
    // client.stop();
  }
}
