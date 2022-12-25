#include "KermiteCore.h"
#include "device/debugUtils.h"
#include "device/usbIoCore.h"

void KermiteCore::begin() {
  usbIoCore_initialize();
}

void KermiteCore::issueKeyState(uint8_t keyIndex, bool isDown) {
  static uint8_t hidKeycodes[6];
  uint8_t outputHidKeyCode = 4;
  hidKeycodes[0] = isDown ? outputHidKeyCode : 0;
  usbIoCore_hidKeyboard_writeReport(0, hidKeycodes);
}

static uint8_t rawHidTempBuf[64];

void KermiteCore::processUpdate() {
  bool hasData = usbIoCore_rawHid_readDataIfExists(rawHidTempBuf);
  //todo: handle rawHid commands
  if (hasData) {
    debugUtils_printBytes(rawHidTempBuf, 64);
  }
}
