#pragma once
#include <stdint.h>

void usbIoCore_initialize();
void usbIoCore_hidKeyboard_writeReport(uint8_t modifiers, uint8_t keycode[6]);
