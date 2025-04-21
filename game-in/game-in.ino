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
ClientConnection client;
Packet packetArr[16];

struct PowerupCoolDownStatus {
  int active = 0;
  unsigned long end;
  PowerUps powerup;
};

PowerupCoolDownStatus CD_status[5];

PowerupCoolDownStatus* getStatus(PowerUps powerup){
  switch (powerup){
    case BallSpeedUp:
      return CD_status;
    case BallInvisible:
      return CD_status + 1;
    case PaddleSpeedUp:
      return CD_status + 2;
    case BallSize: 
      return CD_status + 3 ;
    case BonusPoints:
      return CD_status + 4 ;
  }
}


unsigned long getCooldownLength(PowerUps powerup){
  switch (powerup){
    case BallSpeedUp:
      return 2000;
    case BallInvisible:
        return 1000;
    case PaddleSpeedUp:
        return 5000;
    case BallSize: 
        return 3000;
    case BonusPoints:
      return 0;
  }
}

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
  PongButton inputs[] = { STATE.player1.input.up, STATE.player1.input.down, STATE.player2.input.up, STATE.player2.input.down };

  bool inputRecieved = false;
  while (!inputRecieved) {
    for (int i = 0; i < 4; i++) {
      if (inputs[i].checkPress()) {
        inputRecieved = true;
        break;
      }
    }
  }
}


void setup() {
  initState();


  Serial.begin(9600);
  Serial.println("Started serial");
  IPAddress addr = getIPSerial();
  client = ClientConnection(GAME_IN, addr);


  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);  // Don't proceed, loop forever
  }

  STATE.update(millis());

  drawStartScreen("POWER PONG!");
}

void initState() {
  STATE.powerUpStat = 0;
  CD_status[0].powerup = BallSpeedUp;
  CD_status[1].powerup = BallInvisible;
  CD_status[2].powerup = PaddleSpeedUp;
  CD_status[3].powerup = BallSize;
  CD_status[4].powerup = BonusPoints;

}

void drawPaddle(const Paddle& p) {
  // position refers to the center of the rectangle, so we need to adjust since we draw at the upper lefthand corner
  display.drawRoundRect(p.position.x, p.position.y - p.halfheight, p.halfwidth * 2, p.halfheight * 2, 2, WHITE);
}

void drawBall(const Ball& b) {
  if (STATE.ball.isVisible()){
    display.drawCircle(b.postion.x, b.postion.y, b.radius, WHITE);
  }
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

void updateDisplay() {
  display.clearDisplay();
  drawPaddle(STATE.player1.getPaddle());
  drawPaddle(STATE.player2.getPaddle());
  drawBall(STATE.ball);
  drawScoreBoard(STATE.player1.score, STATE.player2.score);
  display.display();
}

void handlePackets(){
    int recieved = client.readPackets(packetArr);
    if (recieved > 0 ){
      Serial.print("got nPackets: ");
      Serial.println(recieved);
    }

    for (int i = 0; i < recieved; i++){
      packetArr[i].print();

      unsigned long time = millis();
      PowerupCoolDownStatus* stat;
      int powerUps = packetArr[i].getData();
      switch (packetArr[i].getType()){
      case PacketType::PowerupActivatePacket: 
        if (powerUps & PowerUps::BallSpeedUp){
          STATE.ball.velocity.increase(20);
          stat = getStatus(PowerUps::BallSpeedUp);
          stat->end = time + getCooldownLength(PowerUps::BallSpeedUp);
          stat->active = 1;
        } else if (powerUps & PowerUps::PaddleSpeedUp){
          STATE.player1.paddle.paddlePixelPerMove += 10;
          stat = getStatus(PowerUps::PaddleSpeedUp);
          stat->end = time + getCooldownLength(PowerUps::PaddleSpeedUp);
          stat->active = 1;
        } else if (powerUps & PowerUps::BallSize){
          stat = getStatus(PowerUps::BallSize);
          stat->end = time + getCooldownLength(PowerUps::BallSize);
          stat->active = 1;
          STATE.ball.radius = 4;
        } else if (powerUps & PowerUps::BallInvisible){
          stat = getStatus(PowerUps::BallInvisible);
          stat->end = time + getCooldownLength(PowerUps::BallInvisible);
          stat->active = 1;
          STATE.ball.setVisibility(false);
        } else if (powerUps & PowerUps::BonusPoints){
          if (powerUps & (0b1<< 6)){
            STATE.player2.increaseScore();
          } else {
            STATE.player1.increaseScore();
          }
        }
        break;
    }

  }
}

void updatePowerups(unsigned long time){
  for (int i = 0; i < 5; i++){
    if (CD_status[i].active && time > CD_status[i].end){
      switch (CD_status[i].powerup){
        case BallSpeedUp:
          CD_status[i].active = 0;
          STATE.ball.velocity.decrease(20);
          break;
        case BallInvisible:
          CD_status[i].active = 0;
          STATE.ball.setVisibility(true);
          break;
        case PaddleSpeedUp:
          CD_status[i].active = 0;
          STATE.player1.paddle.paddlePixelPerMove -= 10;
          break;
        case BallSize: 
          CD_status[i].active = 0;
          STATE.ball.radius = 2;
          break;
        case BonusPoints:
          CD_status[i].active = 0;
          break;
      }
    }
  }
}

void loop() {
  handlePackets();
  updatePowerups(millis());

  if (STATE.update(millis())) {
    updateDisplay();
  }
}
