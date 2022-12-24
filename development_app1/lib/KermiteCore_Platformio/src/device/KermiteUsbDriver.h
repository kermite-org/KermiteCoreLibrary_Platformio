#pragma once
#include <stdint.h>

void kermiteUsbDriver_initialize();

void kermiteUsbDriver_sendKeyboardReport(uint8_t modifiers, uint8_t keycode[6]);
