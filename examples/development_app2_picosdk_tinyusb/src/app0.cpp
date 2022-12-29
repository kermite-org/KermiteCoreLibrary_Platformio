#include "BoardLED.hpp"
#include "Button.hpp"
#include "usbIoCore.h"
#include <Arduino.h>

// tested on Tiny2040

static BoardLED boardLed(20, 19, 18, true);
static Button buttons[] = { Button(7), Button(5), Button(2), Button(26) };

static void updateButton() {
  {
    static uint8_t report[8];
    Button &button = buttons[0];
    button.update();
    if (button.pressed) {
      report[2] = 4;
      usbIoCore_hidKeyboard_writeReport(report);
    }
    if (button.released) {
      report[2] = 0;
      usbIoCore_hidKeyboard_writeReport(report);
    }
  }

  {
    static uint8_t report[7];
    Button &button = buttons[1];
    button.update();
    if (button.pressed) {
      report[1] = 5;
      report[2] = 5;
      usbIoCore_hidMouse_writeReport(report);
    }
  }

  {
    static uint8_t report[64];
    Button &button = buttons[2];
    button.update();
    if (button.pressed) {
      report[0] = 10;
      report[1] = 12;
      report[2] = 14;
      usbIoCore_rawHid_writeData(report);
    }
  }

  boardLed.write(1, buttons[0].hold);
}

static void echoBackRawHid() {
  static uint8_t rxbuf[64];
  bool hasData = usbIoCore_rawHid_readDataIfExists(rxbuf);
  if (hasData) {
    for (int i = 0; i < 64; i++) {
      rxbuf[i]++;
    }
    usbIoCore_rawHid_writeData(rxbuf);
  }
}

void app0Entry() {
  boardLed.initialize();

  usbIoCore_initialize();

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
    echoBackRawHid();
    cnt++;
    delay(1);
  }
}
