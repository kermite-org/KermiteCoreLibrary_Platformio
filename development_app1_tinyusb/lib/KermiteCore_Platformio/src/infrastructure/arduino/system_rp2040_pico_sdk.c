#include "../system.h"
#include "pico/bootrom.h"

void system_jumpToDfuBootloader() {
  reset_usb_boot(0, 0);
}
