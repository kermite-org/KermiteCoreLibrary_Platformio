#include "BoardLED.hpp"
#include "Button.hpp"
#include <Adafruit_TinyUSB.h>
#include <Arduino.h>
#include <KermiteCore.h>

// tested on Tiny2040

static BoardLED boardLed(20, 19, 18, true);
static Button button(7);

static const uint8_t desc_hid_report[] = {
  TUD_HID_REPORT_DESC_GENERIC_INOUT(64)
};

static Adafruit_USBD_HID usb_hid(desc_hid_report, sizeof(desc_hid_report), HID_ITF_PROTOCOL_NONE, 2, true);

static uint16_t get_report_callback(uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen) {
  return 0;
}

static void set_report_callback(uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize) {
  usb_hid.sendReport(0, buffer, bufsize);
}

static void sendTestData() {
  static uint8_t rawHidTxBuf[64];
  if (usb_hid.ready()) {
    rawHidTxBuf[0] = 20;
    rawHidTxBuf[1] = 21;
    rawHidTxBuf[2] = 22;
    usb_hid.sendReport(0, rawHidTxBuf, sizeof(rawHidTxBuf));
  }
}

static void updateButton() {
  button.update();
  if (button.pressed) {
    Serial.printf("pressed\n");
    sendTestData();
  }
  boardLed.write(1, button.hold);
}

void app2Entry() {
  boardLed.initialize();

  boardLed.write(0, true);

  usb_hid.setReportCallback(get_report_callback, set_report_callback);
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
