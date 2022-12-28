#ifdef USE_TINYUSB

#include "../usbIoCore.h"
#include <Adafruit_TinyUSB.h>
#include <Arduino.h>

enum {
  RID_KEYBOARD = 1,
  RID_MOUSE,
  RID_CONSUMER_CONTROL,
};

static const uint8_t descHidReportShared[] = {
  TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(RID_KEYBOARD)),
  TUD_HID_REPORT_DESC_MOUSE(HID_REPORT_ID(RID_MOUSE)),
  TUD_HID_REPORT_DESC_CONSUMER(HID_REPORT_ID(RID_CONSUMER_CONTROL)),
};

static const uint8_t descHidReportGeneric[] = {
  TUD_HID_REPORT_DESC_GENERIC_INOUT(64),
};

static Adafruit_USBD_HID hidShared(descHidReportShared, sizeof(descHidReportShared), HID_ITF_PROTOCOL_NONE, 2, false);

static Adafruit_USBD_HID hidGeneric(descHidReportGeneric, sizeof(descHidReportGeneric), HID_ITF_PROTOCOL_NONE, 2, true);

static uint8_t rawHidRxBuf[4][64];
static uint32_t rawHidRxPageCount = 0;

static void hidGeneric_setReportCallback(uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize) {
  if (rawHidRxPageCount < 4) {
    memcpy(rawHidRxBuf[rawHidRxPageCount], buffer, bufsize);
    rawHidRxPageCount++;
  }
}

void usbIoCore_initialize() {
  // USBDevice.setID(0xF055, 0xA579);
  USBDevice.setID(0xF055, 0xA57A); //for debugging
  USBDevice.setManufacturerDescriptor("Kermite");
  // USBDevice.setProductDescriptor("KermiteCore_Arduino");

  hidShared.begin();

  hidGeneric.setReportCallback(NULL, hidGeneric_setReportCallback);
  hidGeneric.begin();

  while (!USBDevice.mounted()) {
    delay(1);
  }
}

void usbIoCore_hidKeyboard_writeReport(uint8_t *pReportBytes8) {
  if (hidShared.ready()) {
    hidShared.sendReport(RID_KEYBOARD, pReportBytes8, 8);
  }
}

void usbIoCore_hidMouse_writeReport(uint8_t *pReportBytes7) {
  if (hidShared.ready()) {
    hidShared.sendReport(RID_MOUSE, pReportBytes7, 7);
  }
}

void usbIoCore_hidConsumerControl_writeReport(uint8_t *pReportBytes2) {
  if (hidShared.ready()) {
    hidShared.sendReport(RID_MOUSE, pReportBytes2, 2);
  }
}

uint8_t usbIoCore_hidKeyboard_getStatusLedFlags() {
  //todo: support keyboard status LEDs
  return 0;
}

bool usbIoCore_rawHid_writeData(uint8_t *pDataBytes64) {
  if (hidGeneric.ready()) {
    hidGeneric.sendReport(0, pDataBytes64, 64);
    return true;
  }
  return false;
}

bool usbIoCore_rawHid_readDataIfExists(uint8_t *pDataBytes64) {
  if (rawHidRxPageCount > 0) {
    memcpy(pDataBytes64, rawHidRxBuf[--rawHidRxPageCount], 64);
    return true;
  }
  return false;
}

bool usbIoCore_isConnectedToHost() {
  return USBDevice.ready();
}

void usbIoCore_setProductName(char *productNameText) {
  USBDevice.setProductDescriptor(productNameText);
}

void usbIoCore_setSerialNumber(char *serialNumberText) {
  USBDevice.setSerialDescriptor(serialNumberText);
}

#endif
