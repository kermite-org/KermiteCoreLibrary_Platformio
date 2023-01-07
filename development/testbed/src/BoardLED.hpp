#pragma once
#include <Arduino.h>

class BoardLED {
private:
  bool invert;
  int pins[3] = { -1, -1, -1 };

public:
  BoardLED(int _pin0, int _pin1, int _pin2 = -1, bool _invert = false) {
    pins[0] = _pin0;
    pins[1] = _pin1;
    pins[2] = _pin2;
    invert = _invert;
  }
  void initialize() {
    for (int i = 0; i < 3; i++) {
      pinMode(pins[i], OUTPUT);
      digitalWrite(pins[i], invert ? HIGH : LOW);
    }
  }
  void write(int index, bool value) {
    int pin = pins[index];
    if (pin != -1) {
      digitalWrite(pin, value ^ invert);
    }
  }
  void toggle(int index) {
    int pin = pins[index];
    if (pin != -1) {
      digitalWrite(pin, !digitalRead(pin));
    }
  }
};
