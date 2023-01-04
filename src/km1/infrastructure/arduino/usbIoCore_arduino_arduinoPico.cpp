#include "../buildCondition.h"
#if defined(KEMRITECORE_USE_USBIOCORE_RP2040_ARDUINO_PICO)

#include "RP2040USB.h"
#include "class/hid/hid_device.h"
#include "tusb.h"

void __USBInstallKeyboard() {}
void __USBInstallMouse() {}
void __USBInstallConsumerControl() {}
void __USBInstallSecondHID_RawHID() {}

static __USBDeviceAttributes usbDeviceAttrs = {
  .vendorId = 0xF055,
  .productId = 0xA579,
  // .productId = 0xA57A, //debug
  .manufacturerName = "Kermite",
  .productName = "unnamed keyboard",
  .serialNumberText = "00000000"
};

static const int rawHidDataLength = 63;

static uint8_t hidKeyboardStatusLedFlags = 0;

static uint8_t rawHidRxBuf[rawHidDataLength];
static bool rawHidRxHasData = false;

void hidSetReportCallbackFn(uint8_t instance, uint8_t reportId, uint8_t reportType, uint8_t const *buffer, uint16_t bufsize) {
  const int instanceShared = __USBGetHIDInstanceIndexForSharedHID();
  const int instanceRawHid = __USBGetHIDInstanceIndexForRawHID();
  if (instance == instanceShared) {
    const int reportIdHidKeyboard = __USBGetKeyboardReportID();
    if (reportId == reportIdHidKeyboard && bufsize == 1) {
      hidKeyboardStatusLedFlags = buffer[0];
    }
  }
  if (instance == instanceRawHid && bufsize == rawHidDataLength) {
    memcpy(rawHidRxBuf, buffer, rawHidDataLength);
    rawHidRxHasData = true;
  }
}

void usbIoCore_initialize() {
  __USBSetDeviceAttributes(usbDeviceAttrs);
  __USBSubscribeHIDSetReportCallback(hidSetReportCallbackFn);
}

void usbIoCore_hidKeyboard_writeReport(uint8_t *pReportBytes8) {
  const int instance = __USBGetHIDInstanceIndexForSharedHID();
  const int reportId = __USBGetKeyboardReportID();
  if (tud_hid_n_ready(instance)) {
    tud_hid_n_report(instance, reportId, pReportBytes8, 8);
  }
}

uint8_t usbIoCore_hidKeyboard_getStatusLedFlags() {
  return hidKeyboardStatusLedFlags;
}

void usbIoCore_hidMouse_writeReport(uint8_t *pReportBytes7) {
  const int instance = __USBGetHIDInstanceIndexForSharedHID();
  const int reportId = __USBGetMouseReportID();
  if (tud_hid_n_ready(instance)) {
    tud_hid_n_report(instance, reportId, pReportBytes7, 7);
  }
}

void usbIoCore_hidConsumerControl_writeReport(uint8_t *pReportBytes2) {
  const int instance = __USBGetHIDInstanceIndexForSharedHID();
  const int reportId = __USBGetConsumerControlReportID();
  if (tud_hid_n_ready(instance)) {
    tud_hid_n_report(instance, reportId, pReportBytes2, 2);
  }
}

void usbIoCore_rawHid_writeData(uint8_t *pDataBytes63) {
  const int instance = __USBGetHIDInstanceIndexForRawHID();
  if (tud_hid_n_ready(instance)) {
    tud_hid_n_report(instance, 0, pDataBytes63, rawHidDataLength);
  }
}

bool usbIoCore_rawHid_readDataIfExists(uint8_t *pDataBytes63) {
  if (rawHidRxHasData) {
    memcpy(pDataBytes63, rawHidRxBuf, rawHidDataLength);
    rawHidRxHasData = false;
    return true;
  }
  return false;
}

bool usbIoCore_isConnectedToHost() {
  return tud_hid_ready();
}

void usbIoCore_setProductName(const char *productNameText) {
  usbDeviceAttrs.productName = productNameText;
}

void usbIoCore_setSerialNumber(const char *serialNumberText) {
  usbDeviceAttrs.serialNumberText = serialNumberText;
}

#endif
