#pragma once
#include <stdint.h>

void usbIoCore_initialize();
void usbIoCore_hidKeyboard_writeReport(uint8_t modifiers, uint8_t hidKeycodes[6]);

void usbIoCore_rawHid_writeData(uint8_t *pDataBytes64);
bool usbIoCore_rawHid_readDataIfExists(uint8_t *pDataBytes64);
