#pragma once
#include <stdint.h>

void usbIoCore_initialize();

void usbIoCore_hidKeyboard_writeReport(uint8_t *pReportBytes8);
uint8_t usbIoCore_hidKeyboard_getStatusLedFlags();
bool usbIoCore_hidMouse_writeReport(uint8_t *pReportBytes7);
bool usbIoCore_hidConsumerControl_writeReport(uint8_t *pReportBytes2);
bool usbIoCore_rawHid_writeData(uint8_t *pDataBytes64);
bool usbIoCore_rawHid_readDataIfExists(uint8_t *pDataBytes64);

bool usbIoCore_isConnectedToHost();
void usbIoCore_setProductName(char *productNameText);
void usbIoCore_setSerialNumber(char *serialNumberText);
