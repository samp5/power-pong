#include <Adafruit_Keypad.h>
#include <WiFiS3.h>
#include "utils/PowerUps.h"
#include "utils/Client.h"
#include <LiquidCrystal_I2C.h>

ClientConnection client;

void sendCDTriggerPacket(int powerUpType){
  if(!client.isConnected()) return;

  Packet p = Packet(PowerupActivatePacket);
  p.setData(powerUpType);
  p.print();
  client.sendPacket(&p);
}

LiquidCrystal_I2C lcd(0x27, 16, 2);

class Joystick{
  private:
    int LEFT_THRESHOLD = 400;
    int RIGHT_THRESHOLD = 800;
    int UP_THRESHOLD = 400;
    int DOWN_THRESHOLD = 800;
    int TIME_TO_COMPLETE = 1000;
    int previousActivation = 0;

    int initial_verification = 0;
    int quadrants_verified = 0;

    void checkOneQuadrant(int quadrant){
      if (verifiedQuadrants[quadrant]){
        return;
      }
      quadrants_verified++;
      verifiedQuadrants[quadrant] = true;
      if (quadrants_verified == 1){
        initial_verification = millis();
      }else if (quadrants_verified == 4){
        if (millis() - initial_verification <= TIME_TO_COMPLETE && millis() - previousActivation >= 1000){
          Serial.println("Joystick PowerUp activated!");
          sendCDTriggerPacket(BallSpeedUp);
          previousActivation = millis();
        }
        quadrants_verified = 0;
        bool temp[4] = {false, false, false, false};
        memcpy(verifiedQuadrants, temp, 4);
        initial_verification = 0;
      }
    }

  public:

    int VRX_PIN, VRY_PIN;
    bool verifiedQuadrants[4];

    Joystick(int VRX_PIN, int VRY_PIN){
      this->VRX_PIN = VRX_PIN;
      this->VRY_PIN = VRY_PIN;
      bool temp[4] = {false, false, false, false};
      memcpy(verifiedQuadrants, temp, 4);
    }

    void checkQuadrants(){
      if (millis() - initial_verification > TIME_TO_COMPLETE && quadrants_verified != 0){
        Serial.println("Joystick PowerUp activation TOO SLOW!");
        quadrants_verified = 0;
        bool temp[4] = {false, false, false, false};
        memcpy(verifiedQuadrants, temp, 4);
        initial_verification = 0;
        return;
      }
      int xVal = analogRead(VRX_PIN);
      int yVal = analogRead(VRY_PIN);
      if (xVal < LEFT_THRESHOLD){
        if (yVal < UP_THRESHOLD){
          checkOneQuadrant(0);
        }else if (yVal > DOWN_THRESHOLD){
          checkOneQuadrant(1);
        }
      }else if (xVal > RIGHT_THRESHOLD){
        if (yVal < UP_THRESHOLD){
          checkOneQuadrant(2);
        }else if (yVal > DOWN_THRESHOLD){
          checkOneQuadrant(3);
        }
      }
    }

};

class Motor{
  private:
    int DELAY_TIME = 50;
    int IDLE_VALUE = 359;
    int SPIN_INTENSITY = 9;

    int prev_time = 0;
    int previousActivation = 0;

  public:

    int pin;

    Motor(int pin){
      this->pin = pin;
      pinMode(pin, INPUT); // this may not be needed but the tutorial had it
    }

    void checkMotorMotion(){
      int reading = analogRead(pin);
      if (millis() - prev_time >= DELAY_TIME){
        prev_time = millis();
        // Gotta check this way because when the motor is idle
        // the reading may fluctuate by a few points every once in a while
        if (reading < IDLE_VALUE - SPIN_INTENSITY || reading > IDLE_VALUE + SPIN_INTENSITY){
          if (millis() - previousActivation >= 1000){
            Serial.println("Motor PowerUp Activated!");
            sendCDTriggerPacket(PaddleSpeedUp);
            previousActivation = millis();
          }
        }
      }
    }
};

class UltraSonicSensor{
  private:
    int DISTANCE_TO_ACTIVATE = 5; // dunno what to make this yet(it's in cm)
    int DELAY_TIME = 100;
    int prev_time = 0;
    float duration, distance;
    int previousActivation = 0;

  public:

    int trigPin, echoPin;

    UltraSonicSensor(int trigPin, int echoPin){
      this->trigPin = trigPin;
      this->echoPin = echoPin;
      pinMode(trigPin, OUTPUT);
      pinMode(echoPin, INPUT);
    }

    void checkUltraSonicSensor(){
      if (millis() - prev_time >= DELAY_TIME){
        prev_time = millis();
        // UMMM am I allowed 12 microseconds of delay using delay() here lol?
        digitalWrite(trigPin, LOW);
        delayMicroseconds(2);
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);

        duration = pulseIn(echoPin, HIGH);
        distance = (duration * .0343) / 2;
        if (distance <= DISTANCE_TO_ACTIVATE && millis() - previousActivation >= 1000){
          Serial.println("UltraSonicSensor PowerUp activated!");
          sendCDTriggerPacket(BallSize);
          previousActivation = millis();
        }
      }
    }
};

class Photoresistor{
  private:
    int LIGHT_THRESHOLD = 900; // we can adjust this to whatever
    int DELAY_TIME = 1000;
    int prev_time = 0;
    int previousActivation = 0;

  public:
    
    int pin;

    Photoresistor(int pin){
      this->pin = pin;
    }

    void checkPhotoresistor(){
      if (millis() - prev_time >= DELAY_TIME){
        prev_time = millis();
        int reading = analogRead(pin);
        if (reading >= LIGHT_THRESHOLD && millis() - previousActivation >= 1000){
          Serial.println("Photoresistor PowerUp activated!");
          sendCDTriggerPacket(BallInvisible);
          previousActivation = millis();
        }
      }
    }
};

class NumPad{
  private:
    int result = 0;
    int sum[2];

    void generateEquation(){
      sum[0] = random(1, 101);
      sum[1] = random(1, 101);
    }

  public:
  
    NumPad(){
      // we are randomizing with empty analog pin noise
      generateEquation();
    }

    void printEquation(){
      char equation[20];
      char temp[10];
      (result == 0) ? snprintf(temp, 10, "?") : snprintf(temp, 10, "%d", result);
      sprintf(equation, "%d + %d = %s", sum[0], sum[1], temp);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(equation);
    }

    void checkKey(char customKey){
      switch (customKey) {
        case 'A':
          if (result == sum[0] + sum[1]){
            Serial.println("NumPad PowerUp activated!");
            sendCDTriggerPacket(BonusPoints);
            result = 0;
            generateEquation();
            printEquation();
          }
        case 'B':
          if (result == sum[0] + sum[1]){
            Serial.println("NumPad PowerUp activated!");
            sendCDTriggerPacket(32 | BonusPoints);
            result = 0;
            generateEquation();
            printEquation();
          }
        case 'C':
        case 'D':
        case '#':
          result = 0;
          break;
        case '*':
          result /= 10;
          break;
        default:
          result = result * 10 + (customKey - '0');
      }
      printEquation();
      
    }
};

Joystick joystick = Joystick(A0, A1);
Motor motor = Motor(A2);
UltraSonicSensor ultraSonicSensor = UltraSonicSensor(3, 5);
Photoresistor photoresistor = Photoresistor(A3);
NumPad numpad;

const byte ROWS = 4;
const byte COLS = 4;

// Define the characters mapped to each button on the 4x4 keypad
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

// Define the Arduino pins connected to the row pinouts of the keypad
byte rowPins[ROWS] = { 12, 11, 10, 9 };
// Define the Arduino pins connected to the column pinouts of the keypad
byte colPins[COLS] = { 8, 7, 6, 4 };

// Initialize a custom keypad instance
Adafruit_Keypad myKeypad = Adafruit_Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  Serial.begin(9600);
  Serial.println("Beggining serial power-in!");
  randomSeed(analogRead(A5));
  lcd.init();
  lcd.clear();
  lcd.backlight();
  myKeypad.begin();
  numpad.printEquation();
  IPAddress ip = getIPSerial();
  client = ClientConnection(POWERUP_IN, ip);
}

void loop() {
  // numpad.checkKey();
  myKeypad.tick();

  // Check if there are new keypad events
  while (myKeypad.available()) {
    // Read the keypad event
    keypadEvent e = myKeypad.read();
    // Print the type of event: pressed or released
    if (e.bit.EVENT == KEY_JUST_PRESSED) numpad.checkKey((char)e.bit.KEY);
  }
  joystick.checkQuadrants();
  motor.checkMotorMotion();
  photoresistor.checkPhotoresistor();
  ultraSonicSensor.checkUltraSonicSensor();
}
