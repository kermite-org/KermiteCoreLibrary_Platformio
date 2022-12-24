#include "KermiteCore.h"
#include "KermiteUsbDriver.h"

void KermiteCore::begin() { kermiteUsbDriver_initialize(); }

void KermiteCore::issueInputKeyStateChange(uint8_t keyIndex, bool isDown) {
  static uint8_t hidKeycodes[6];
  uint8_t outputHidKeyCode = 4;
  hidKeycodes[0] = isDown ? outputHidKeyCode : 0;
  kermiteUsbDriver_sendKeyboardReport(0, hidKeycodes);
}
