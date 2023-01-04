#include "../buildCondition.h"
#if defined(KEMRITECORE_USE_USBIOCORE_ADAFRUIT_TINYUSB_SEPARATED_INTERFACES)

#include "../system.h"
#include "../usbIoCore.h"
#include <Adafruit_TinyUSB.h>


enum {
  RID_KEYBOARD = 1,
  RID_MOUSE,
  RID_CONSUMER_CONTROL,
};

static const int rawHidDataLength = 63;

static const uint8_t descHidReportShared[] = {
  TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(RID_KEYBOARD)),
  TUD_HID_REPORT_DESC_MOUSE(HID_REPORT_ID(RID_MOUSE)),
  TUD_HID_REPORT_DESC_CONSUMER(HID_REPORT_ID(RID_CONSUMER_CONTROL)),
};

static const uint8_t descHidReportGeneric[] = {
  TUD_HID_REPORT_DESC_GENERIC_INOUT(rawHidDataLength),
};

static Adafruit_USBD_HID hidShared(descHidReportShared, sizeof(descHidReportShared), HID_ITF_PROTOCOL_NONE, 2, true);

static Adafruit_USBD_HID hidGeneric(descHidReportGeneric, sizeof(descHidReportGeneric), HID_ITF_PROTOCOL_NONE, 2, true);

static uint8_t rawHidRxBuf[4][rawHidDataLength];
static uint32_t rawHidRxPageCount = 0;

static uint8_t keyboardLedStatus = 0;

static void hidShared_setReportCallback(uint8_t reportId, hid_report_type_t reportType, uint8_t const *buffer, uint16_t bufsize) {
  if (reportId == RID_KEYBOARD && bufsize == 1) {
    keyboardLedStatus = buffer[0];
  }
}

static void hidGeneric_setReportCallback(uint8_t reportId, hid_report_type_t reportType, uint8_t const *buffer, uint16_t bufsize) {
  if (rawHidRxPageCount < 4) {
    memcpy(rawHidRxBuf[rawHidRxPageCount], buffer, bufsize);
    rawHidRxPageCount++;
  }
}

void usbIoCore_initialize() {
  USBDevice.setID(0xF055, 0xA579);
  // USBDevice.setID(0xF055, 0xA57A); //for debugging
  USBDevice.setManufacturerDescriptor("Kermite");

  hidShared.setReportCallback(NULL, hidShared_setReportCallback);
  hidShared.begin();

  hidGeneric.setReportCallback(NULL, hidGeneric_setReportCallback);
  hidGeneric.begin();

  while (!USBDevice.mounted()) {
    system_delayMs(1);
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
  return keyboardLedStatus;
}

bool usbIoCore_rawHid_writeData(uint8_t *pDataBytes63) {
  if (hidGeneric.ready()) {
    hidGeneric.sendReport(0, pDataBytes63, rawHidDataLength);
    return true;
  }
  return false;
}

bool usbIoCore_rawHid_readDataIfExists(uint8_t *pDataBytes63) {
  if (rawHidRxPageCount > 0) {
    memcpy(pDataBytes63, rawHidRxBuf[--rawHidRxPageCount], rawHidDataLength);
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

#endif
