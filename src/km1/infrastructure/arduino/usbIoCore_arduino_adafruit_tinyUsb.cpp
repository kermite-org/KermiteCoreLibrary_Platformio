#include "../buildCondition.h"
#if defined(KEMRITECORE_USE_USBIOCORE_ADAFRUIT_TINYUSB)

#include "../kprintf.h"
#include "../system.h"
#include "../usbIoCore.h"
#include "km1/domain/debugUtils.h"
#include <Adafruit_TinyUSB.h>


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
  // kprintf("received %d %d %d\n", reportId, reportType, bufsize);
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
    system_delayMs(1);
  }
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
  hidShared.sendReport(RID_KEYBOARD, pReportBytes8, 8);
}

static void sendMouseReport(uint8_t *pReportBytes7) {
  hidShared.sendReport(RID_MOUSE, pReportBytes7, 7);
}

static void sendConsumerControlReport(uint8_t *pReportBytes2) {
  hidShared.sendReport(RID_CONSUMER_CONTROL, pReportBytes2, 2);
}

static void sendRawHidReport(uint8_t *pDataBytes63) {
  hidShared.sendReport(RID_GENERIC_HID, pDataBytes63, RAWHID_DATA_LENGTH);
}

static void emitOneReportIfReady() {
  if (hidShared.ready()) {
    reportEmitterQueue_emitOne();
  }
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

uint8_t usbIoCore_hidKeyboard_getStatusLedFlags() {
  return keyboardLedStatus;
}

bool usbIoCore_rawHid_writeData(uint8_t *pDataBytes63) {
  reportEmitterQueue_push(sendRawHidReport, pDataBytes63, 63);
  emitOneReportIfReady();
  return true;
}

bool usbIoCore_rawHid_readDataIfExists(uint8_t *pDataBytes63) {
  if (rawHidRxPageCount > 0) {
    memcpy(pDataBytes63, rawHidRxBuf[--rawHidRxPageCount], RAWHID_DATA_LENGTH);
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

void usbIoCore_processUpdate() {
  emitOneReportIfReady();
}

#endif
