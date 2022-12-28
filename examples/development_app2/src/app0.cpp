#include "BoardLED.hpp"
#include "Button.hpp"
#include <Arduino.h>

// tested on Tiny2040

static BoardLED boardLed(20, 19, 18, true);
static Button button(7);

static void updateButton() {
  button.update();
  if (button.pressed) {
    Serial.printf("pressed\n");
  }
  if (button.released) {
    Serial.printf("released\n");
  }
  boardLed.write(1, button.hold);
}

void app0Entry() {
  boardLed.initialize();

  while (1) {
    static int cnt = 0;
    if (cnt % 1000 == 0) {
      boardLed.write(0, true);
    }
    if (cnt % 1000 == 1) {
      boardLed.write(0, false);
    }
    if (cnt % 10 == 0) {
      updateButton();
    }
    cnt++;
    yield();
    delay(1);
  }
}
