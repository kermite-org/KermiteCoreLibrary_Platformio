#include "BoardLED.hpp"
#include "Button.hpp"
#include <Adafruit_TinyUSB.h>
#include <Arduino.h>
#include <KermiteCore.h>

// tested on Tiny2040

static BoardLED boardLed(20, 19, 18, true);
static Button button(7);

enum {
  RID_KEYBOARD = 1,
};

static const uint8_t desc_hid_report[] = {
  TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(RID_KEYBOARD)),
};

static Adafruit_USBD_HID usb_hid(desc_hid_report, sizeof(desc_hid_report), HID_ITF_PROTOCOL_NONE, 2, false);

static void sendHidKey(uint hidKeycode, bool pressed) {
  static uint8_t hidKeycodes[6];
  if (usb_hid.ready()) {
    hidKeycodes[0] = pressed ? hidKeycode : 0;
    usb_hid.keyboardReport(RID_KEYBOARD, 0, hidKeycodes);
  }
}

static void updateButton() {
  button.update();
  if (button.pressed) {
    Serial.printf("pressed\n");
    sendHidKey(4, true);
  }
  if (button.released) {
    Serial.printf("released\n");
    sendHidKey(4, false);
  }
  boardLed.write(1, button.hold);
}

void app1Entry() {

  USBDevice.setID(0xF055, 0xA57A);
  USBDevice.setManufacturerDescriptor("Kermite");
  USBDevice.setProductDescriptor("KermiteCore_Arduino");

  boardLed.initialize();

  boardLed.write(0, true);
  usb_hid.begin();
  while (!USBDevice.mounted())
    delay(1);
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
    cnt++;
    yield();
    delay(1);
  }
}
