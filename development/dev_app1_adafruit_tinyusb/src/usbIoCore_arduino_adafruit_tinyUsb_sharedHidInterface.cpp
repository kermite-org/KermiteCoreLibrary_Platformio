#include "km1/domain/debugUtils.h"
#include "km1/infrastructure/usbIoCore.h"
#include <Adafruit_TinyUSB.h>
#include <Arduino.h>

enum {
  RID_KEYBOARD = 1,
  RID_MOUSE,
  RID_CONSUMER_CONTROL,
  RID_GENERIC_HID
};

#define RAWHID_DATA_LENGTH 63

static const uint8_t descHidReportShared[] = {
  TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(RID_KEYBOARD)),
  TUD_HID_REPORT_DESC_MOUSE(HID_REPORT_ID(RID_MOUSE)),
  TUD_HID_REPORT_DESC_CONSUMER(HID_REPORT_ID(RID_CONSUMER_CONTROL)),
  TUD_HID_REPORT_DESC_GENERIC_INOUT(RAWHID_DATA_LENGTH, HID_REPORT_ID(RID_GENERIC_HID)),
};

static const uint8_t descHidReportGeneric[] = {};

static Adafruit_USBD_HID hidShared(descHidReportShared, sizeof(descHidReportShared), HID_ITF_PROTOCOL_NONE, 2, true);

static uint8_t rawHidRxBuf[4][RAWHID_DATA_LENGTH];
static uint32_t rawHidRxPageCount = 0;

static uint8_t keyboardLedStatus = 0;

static void hidShared_setReportCallback(uint8_t reportId, hid_report_type_t reportType, uint8_t const *buffer, uint16_t bufsize) {
  if (bufsize == RAWHID_DATA_LENGTH + 1) {
    //retrieve report id from first byte
    reportId = buffer[0];
    buffer = buffer + 1;
    bufsize -= 1;
  }
  Serial.printf("received %d %d %d\n", reportId, reportType, bufsize);
  debugUtils_printBytes((uint8_t *)buffer, bufsize);
  if (reportId == RID_KEYBOARD && bufsize == 1) {
    keyboardLedStatus = buffer[0];
  }
  if (reportId == RID_GENERIC_HID) {
    if (rawHidRxPageCount < 4) {
      memcpy(rawHidRxBuf[rawHidRxPageCount], buffer, bufsize);
      rawHidRxPageCount++;
    }
  }
}

void usbIoCore_initialize() {
  USBDevice.setID(0xF055, 0xA579);
  // USBDevice.setID(0xF055, 0xA57A); //for debugging
  USBDevice.setManufacturerDescriptor("Kermite");

  hidShared.setReportCallback(NULL, hidShared_setReportCallback);
  hidShared.begin();

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
    hidShared.sendReport(RID_CONSUMER_CONTROL, pReportBytes2, 2);
  }
}

uint8_t usbIoCore_hidKeyboard_getStatusLedFlags() {
  return keyboardLedStatus;
}

bool usbIoCore_rawHid_writeData(uint8_t *pDataBytes64) {
  if (hidShared.ready()) {
    hidShared.sendReport(RID_GENERIC_HID, pDataBytes64, RAWHID_DATA_LENGTH);
    return true;
  }
  return false;
}

bool usbIoCore_rawHid_readDataIfExists(uint8_t *pDataBytes64) {
  if (rawHidRxPageCount > 0) {
    memcpy(pDataBytes64, rawHidRxBuf[--rawHidRxPageCount], RAWHID_DATA_LENGTH);
    return true;
  }
  return false;
}

bool usbIoCore_isConnectedToHost() {
  return USBDevice.ready();
}

void usbIoCore_setProductName(const char *productNameText) {
  USBDevice.setProductDescriptor(productNameText);
}

void usbIoCore_setSerialNumber(const char *serialNumberText) {
  USBDevice.setSerialDescriptor(serialNumberText);
}
