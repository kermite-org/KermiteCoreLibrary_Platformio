#include "BoardLED.hpp"
#include "Button.hpp"
#include <Arduino.h>
#include <Keyboard.h>
#include <Mouse.h>

// tested on Tiny2040

static BoardLED boardLed(20, 19, 18, true);
static Button buttons[] = { Button(7), Button(5), Button(2), Button(26) };

static void updateButton() {
  {
    Button &button = buttons[0];
    button.update();
    if (button.pressed) {
      Mouse.move(5, 5, 0);
    }
  }

  {
    Button &button = buttons[1];
    button.update();
    if (button.pressed) {
      Keyboard.print("hello");
    }
  }

  boardLed.write(1, buttons[0].hold);
}

void app0Entry() {
  boardLed.initialize();
  Mouse.begin();
  Keyboard.begin();

  while (1) {
    static int cnt = 0;
    if (cnt % 1000 == 0) {
      boardLed.write(1, true);
    }
    if (cnt % 1000 == 1) {
      boardLed.write(1, false);
    }
    if (cnt % 10 == 0) {
      updateButton();
    }
    cnt++;
    yield();
    delay(1);
  }
}
