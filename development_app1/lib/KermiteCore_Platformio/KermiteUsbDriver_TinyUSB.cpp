#ifdef USE_TINYUSB
#include "KermiteUsbDriver.h"
#include <Arduino.h>

#include <Adafruit_TinyUSB.h>

static const uint8_t desc_hid_report[] = {TUD_HID_REPORT_DESC_KEYBOARD()};

static Adafruit_USBD_HID usb_hid;

void kermiteUsbDriver_initialize() {
  usb_hid.setPollInterval(2);
  usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  usb_hid.begin();
  while (!USBDevice.mounted())
    delay(1);
}

void kermiteUsbDriver_sendKeyboardReport(uint8_t modifier,
                                         uint8_t *hidKeycodes) {
  if (usb_hid.ready()) {
    usb_hid.keyboardReport(0, modifier, hidKeycodes);
  }
}

#endif
