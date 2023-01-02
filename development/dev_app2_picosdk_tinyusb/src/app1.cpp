#include "BoardLED.hpp"
#include "Button.hpp"
#include <Arduino.h>
#include <KermiteCore.h>
#include <SPI.h>
#include <SdFat.h>

// tested on Tiny2040

static BoardLED boardLed(20, 19, 18, true);
static Button button(7);

static KermiteCore kermite;

static void updateButton() {
  button.update();
  if (button.pressed) {
    Serial.printf("pressed\n");
    kermite.feedKeyState(0, true);
  }
  if (button.released) {
    Serial.printf("released\n");
    kermite.feedKeyState(0, false);
  }
  boardLed.write(1, button.hold);
}

void app1Entry() {
  boardLed.initialize();
  boardLed.write(0, true);

  kermite.setKeyboardName("mykbd");
  kermite.setFirmwareId("Y1NFrC");
  kermite.enableDebugLogging();
  kermite.begin();
  boardLed.write(0, false);

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
    kermite.processUpdate();
    cnt++;
    yield();
    delay(1);
  }
}
