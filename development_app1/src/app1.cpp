#include "BoardLED.hpp"
#include "Button.hpp"
#include <Adafruit_TinyUSB.h>
#include <Arduino.h>

// tested on Tiny2040

enum {
  RID_KEYBOARD = 1,
  RID_MOUSE,
  RID_CONSUMER_CONTROL,
};

static const uint8_t desc_hid_report[] = {
  TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(RID_KEYBOARD)),
  TUD_HID_REPORT_DESC_MOUSE(HID_REPORT_ID(RID_MOUSE)),
  TUD_HID_REPORT_DESC_CONSUMER(HID_REPORT_ID(RID_CONSUMER_CONTROL))
};

Adafruit_USBD_HID usb_hid(desc_hid_report, sizeof(desc_hid_report), HID_ITF_PROTOCOL_NONE, 2, false);

static BoardLED boardLED(20, 19, 18, true);
static Button buttons[] = { Button(7), Button(5), Button(2) };

static void sendHidKey(uint hidKeycode, bool pressed) {
  static uint8_t hidKeycodes[6];
  if (usb_hid.ready()) {
    hidKeycodes[0] = pressed ? hidKeycode : 0;
    usb_hid.keyboardReport(RID_KEYBOARD, 0, hidKeycodes);
  }
}

static void updateButton() {
  {
    Button &button = buttons[0];
    button.update();
    if (button.pressed) {
      sendHidKey(4, true);
    }
    if (button.released) {
      sendHidKey(4, false);
    }
    boardLED.write(1, button.hold);
  }

  {
    Button &button = buttons[1];
    button.update();
    if (button.hold) {
      int8_t delta = 5;
      if (usb_hid.ready()) {
        usb_hid.mouseMove(RID_MOUSE, delta, delta); // right + down
      }
    }
  }

  {
    Button &button = buttons[2];
    button.update();
    if (button.pressed) {
      if (usb_hid.ready()) {
        usb_hid.sendReport16(RID_CONSUMER_CONTROL, HID_USAGE_CONSUMER_VOLUME_DECREMENT);
      }
    }
    if (button.released) {
      if (usb_hid.ready()) {
        usb_hid.sendReport16(RID_CONSUMER_CONTROL, 0);
      }
    }
  }
}

void app1Entry() {
  boardLED.initialize();

  usb_hid.setPollInterval(2);

  usb_hid.begin();
  boardLED.write(0, true);
  while (!USBDevice.mounted())
    delay(1);
  boardLED.write(0, false);

  while (1) {
    static int cnt = 0;
    if (cnt % 1000 == 0) {
      boardLED.write(0, true);
    }
    if (cnt % 1000 == 1) {
      boardLED.write(0, false);
    }
    if (cnt % 10 == 0) {
      updateButton();
    }
    cnt++;
    delay(1);
  }
}
