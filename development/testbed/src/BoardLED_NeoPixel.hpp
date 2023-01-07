#pragma once
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>

class BoardLED_NeoPixel {
private:
  Adafruit_NeoPixel neoPixel;
  uint8_t brightness;
  bool states[3] = { false, false, false };
  int powerPin;

  void updateLedColor() {
    int col = 0;
    if (states[0]) {
      col |= 0xFF0000;
    }
    if (states[1]) {
      col |= 0x00FF00;
    }
    if (states[2]) {
      col |= 0x0000FF;
    }

    neoPixel.clear();
    neoPixel.setPixelColor(0, col);
    neoPixel.show();
  }

public:
  BoardLED_NeoPixel(int pin, int _brightness = 0xFF, int _powerPin = -1)
      : neoPixel(1, pin, NEO_BGR), brightness(_brightness), powerPin(_powerPin) {}

  void initialize() {
    if (powerPin != -1) {
      pinMode(powerPin, OUTPUT);
      digitalWrite(powerPin, HIGH);
    }
    neoPixel.setBrightness(brightness);
  }

  void write(int index, bool value) {
    states[index] = value;
    updateLedColor();
  }
  void toggle(int index) {
    states[index] = !states[index];
    updateLedColor();
  }
};
