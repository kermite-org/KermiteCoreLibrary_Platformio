#include "BoardLED.hpp"
#include "Button.hpp"
#include "domain/dataMemory.h"
#include "infrastructure/flashPersistSector.h"
#include <Arduino.h>

static BoardLED boardLed(20, 19, 18, true);
static Button buttons[] = { Button(7), Button(5) };

static int cnt = 0;

static void updateButton() {

  {
    Button &button = buttons[0];
    button.update();
    if (button.pressed) {
      dataMemory_writeByte(100, 123 + (cnt++));
      Serial.printf("written\n");
    }
    boardLed.write(1, button.hold);
  }

  {
    Button &button = buttons[1];
    button.update();
    if (button.pressed) {
      uint8_t val = dataMemory_readByte(100);
      Serial.printf("val: %d\n", val);
    }
    boardLed.write(2, button.hold);
  }
}

void app3Entry() {
  boardLed.initialize();

  flashPersistSector_initialize();
  dataMemory_initialize();

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
    dataMemory_processTick();
    yield();
    delay(1);
  }
}
