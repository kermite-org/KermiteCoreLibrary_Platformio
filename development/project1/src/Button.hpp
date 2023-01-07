#include <Arduino.h>

class Button {
private:
  int pin;

public:
  bool hold;
  bool pressed;
  bool released;

  Button(int _pin) {
    pin = _pin;
    pinMode(pin, INPUT_PULLUP);
  }

  void update() {
    bool hold_next = digitalRead(pin) == LOW;
    pressed = !hold && hold_next;
    released = hold && !hold_next;
    hold = hold_next;
  }
};
