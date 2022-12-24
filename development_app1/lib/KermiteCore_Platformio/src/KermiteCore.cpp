#include "KermiteCore.h"
#include "device/usbIoCore.h"

void KermiteCore::begin() {
  usbIoCore_initialize();
}

void KermiteCore::issueInputKeyStateChange(uint8_t keyIndex, bool isDown) {
  static uint8_t hidKeycodes[6];
  uint8_t outputHidKeyCode = 4;
  hidKeycodes[0] = isDown ? outputHidKeyCode : 0;
  usbIoCore_hidKeyboard_writeReport(0, hidKeycodes);
}
