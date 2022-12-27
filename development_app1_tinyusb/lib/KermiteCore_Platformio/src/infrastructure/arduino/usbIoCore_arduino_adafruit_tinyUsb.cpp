#ifdef USE_TINYUSB

#include "../usbIoCore.h"
#include <Adafruit_TinyUSB.h>
#include <Arduino.h>

enum {
  RID_KEYBOARD = 1,
  RID_MOUSE,
  RID_CONSUMER_CONTROL,
};

static const uint8_t desc_hid_report[] = {
  TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(RID_KEYBOARD)),
  TUD_HID_REPORT_DESC_MOUSE(HID_REPORT_ID(RID_MOUSE)),
  TUD_HID_REPORT_DESC_CONSUMER(HID_REPORT_ID(RID_CONSUMER_CONTROL)),
};

static const uint8_t desc_hid_report_2[] = {
  TUD_HID_REPORT_DESC_GENERIC_INOUT(64),
};

static Adafruit_USBD_HID usb_hid(desc_hid_report, sizeof(desc_hid_report), HID_ITF_PROTOCOL_NONE, 2, false);

static Adafruit_USBD_HID usb_hid_2(desc_hid_report_2, sizeof(desc_hid_report_2), HID_ITF_PROTOCOL_NONE, 2, true);

static uint8_t rawHidRxBuf[4][64];
static uint32_t rawHidRxPageCount = 0;

static void usb_hid_2_set_report_callback(uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize) {
  if (rawHidRxPageCount < 4) {
    memcpy(rawHidRxBuf[rawHidRxPageCount], buffer, bufsize);
    rawHidRxPageCount++;
  }
}

void usbIoCore_initialize() {
  // USBDevice.setID(0xF055, 0xA579);
  USBDevice.setID(0xF055, 0xA57A); //for debugging

  USBDevice.setManufacturerDescriptor("Kermite");
  USBDevice.setProductDescriptor("KermiteCore_Arduino");

  usb_hid.begin();

  usb_hid_2.setReportCallback(NULL, usb_hid_2_set_report_callback);
  usb_hid_2.begin();

  while (!USBDevice.mounted()) {
    delay(1);
  }
}

void usbIoCore_hidKeyboard_writeReport(
    uint8_t modifier,
    uint8_t *hidKeycodes) {
  if (usb_hid.ready()) {
    usb_hid.keyboardReport(RID_KEYBOARD, modifier, hidKeycodes);
  }
}

void usbIoCore_rawHid_writeData(uint8_t *pDataBytes64) {
  if (usb_hid_2.ready()) {
    usb_hid_2.sendReport(0, pDataBytes64, 64);
  }
}

bool usbIoCore_rawHid_readDataIfExists(uint8_t *pDataBytes64) {
  if (rawHidRxPageCount > 0) {
    memcpy(pDataBytes64, rawHidRxBuf[--rawHidRxPageCount], 64);
    return true;
  }
  return false;
}

#endif
