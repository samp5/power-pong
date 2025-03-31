#include <Arduino.h>
/**
 * Button struct which handles debounce.
 */
struct Button {
private:
  static const unsigned long debounceDelay = 50;

public:
  int pin;
  int state = LOW;
  int previous_state = HIGH;
  unsigned long last_change = 0;

  /**
   * Construct a button to read input on `pin`
   *
   */
  Button(int pin) {
    this->pin = pin;
    pinMode(this->pin, INPUT);
  }

  /**
   * Check if this button has been pressed (with debounce handling)
   */
  bool checkPress() {
    bool press = false;
    int reading = digitalRead(this->pin);

    // debounce code
    if (reading != this->previous_state) {
      this->last_change = millis();
    }

    if ((millis() - this->last_change) > debounceDelay) {
      if (reading != this->state) {
        this->state = reading;
        if (this->state == HIGH) {
          press = true;
        }
      }
    }
    this->previous_state = reading;
    return press;
  }
};
