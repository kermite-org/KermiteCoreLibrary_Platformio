#include "../buildCondition.h"
#if defined(KEMRITECORE_USE_USBIOCORE_RP2040_ARDUINO_PICO)

#include "../kprintf.h"
#include "RP2040USB.h"
#include "class/hid/hid_device.h"
#include "km1/domain/debugUtils.h"
#include "tusb.h"

void __USBInstallKeyboard() {}
void __USBInstallMouse() {}
void __USBInstallConsumerControl() {}
void __USBInstallGenericHID() {}

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

static uint8_t rawHidRxBuf[4][rawHidDataLength];
static uint32_t rawHidRxPageCount = 0;

static void hidSetReportCallbackFn(uint8_t reportId, uint8_t reportType, uint8_t const *buffer, uint16_t bufsize) {
  int reportIdHidKeyboard = __USBGetKeyboardReportID();
  int reportIdRawHid = __USBGetGenericHIDReportID();
  if (reportId == reportIdHidKeyboard && bufsize == 1) {
    hidKeyboardStatusLedFlags = buffer[0];
  }
  if (reportId == reportIdRawHid && bufsize == rawHidDataLength) {
    if (rawHidRxPageCount < 4) {
      memcpy(rawHidRxBuf[rawHidRxPageCount], buffer, bufsize);
      rawHidRxPageCount++;
    }
  }
}

void usbIoCore_initialize() {
  __USBSetDeviceAttributes(usbDeviceAttrs);
  __USBSubscribeHIDSetReportCallback(hidSetReportCallbackFn);
}

//----------
//report emitter queue

typedef void (*ReportEmitterFn)(uint8_t *report);
typedef struct {
  ReportEmitterFn reportEmitterFn;
  uint8_t *reportBytes;
} ReportQueueItem;

static ReportQueueItem *reportQueue[8];
static int reportQueue_wi = 0;
static int reportQueue_ri = 0;

static void reportEmitterQueue_push(ReportEmitterFn fn, uint8_t *report, int len) {
  if ((reportQueue_wi + 1) & 7 == reportQueue_ri) {
    kprintf("cannot enqueue hid report (8/8)\n");
  } else {
    uint8_t *reportBytes = new uint8_t[len];
    memcpy(reportBytes, report, len);
    ReportQueueItem *item = new ReportQueueItem();
    item->reportEmitterFn = fn;
    item->reportBytes = reportBytes;
    reportQueue[reportQueue_wi] = item;
    reportQueue_wi = (reportQueue_wi + 1) & 7;
  }
}

static ReportQueueItem *reportEmitterQueue_pop() {
  if (reportQueue_wi != reportQueue_ri) {
    ReportQueueItem *item = reportQueue[reportQueue_ri];
    reportQueue[reportQueue_ri] = nullptr;
    reportQueue_ri = (reportQueue_ri + 1) & 7;
    return item;
  }
  return nullptr;
}

static void reportEmitterQueue_emitOne() {
  ReportQueueItem *item = reportEmitterQueue_pop();
  if (item) {
    item->reportEmitterFn(item->reportBytes);
    delete[] item->reportBytes;
    delete item;
  }
}

//----------

static void sendKeyboardReport(uint8_t *pReportBytes8) {
  int reportId = __USBGetKeyboardReportID();
  tud_hid_report(reportId, pReportBytes8, 8);
}

static void sendMouseReport(uint8_t *pReportBytes7) {
  int reportId = __USBGetMouseReportID();
  tud_hid_report(reportId, pReportBytes7, 7);
}

static void sendConsumerControlReport(uint8_t *pReportBytes2) {
  const int reportId = __USBGetConsumerControlReportID();
  tud_hid_report(reportId, pReportBytes2, 2);
}

static void sendRawHidReport(uint8_t *pDataBytes63) {
  const int reportId = __USBGetGenericHIDReportID();
  tud_hid_report(reportId, pDataBytes63, rawHidDataLength);
}

static void emitOneReportIfReady() {
  if (tud_hid_ready()) {
    reportEmitterQueue_emitOne();
  }
}

uint8_t usbIoCore_hidKeyboard_getStatusLedFlags() {
  return hidKeyboardStatusLedFlags;
}

void usbIoCore_hidKeyboard_writeReport(uint8_t *pReportBytes8) {
  reportEmitterQueue_push(sendKeyboardReport, pReportBytes8, 8);
  emitOneReportIfReady();
}

void usbIoCore_hidMouse_writeReport(uint8_t *pReportBytes7) {
  reportEmitterQueue_push(sendMouseReport, pReportBytes7, 7);
  emitOneReportIfReady();
}

void usbIoCore_hidConsumerControl_writeReport(uint8_t *pReportBytes2) {
  reportEmitterQueue_push(sendConsumerControlReport, pReportBytes2, 2);
  emitOneReportIfReady();
}

void usbIoCore_rawHid_writeData(uint8_t *pDataBytes63) {
  reportEmitterQueue_push(sendRawHidReport, pDataBytes63, 63);
  emitOneReportIfReady();
}

bool usbIoCore_rawHid_readDataIfExists(uint8_t *pDataBytes63) {
  if (rawHidRxPageCount > 0) {
    memcpy(pDataBytes63, rawHidRxBuf[--rawHidRxPageCount], rawHidDataLength);
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

void usbIoCore_processUpdate() {
  emitOneReportIfReady();
}

#endif
