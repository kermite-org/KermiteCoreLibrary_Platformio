#include "flashPersistSector.h"

#include <Adafruit_FlashTransport.h>
#include <Adafruit_SPIFlash.h>

//Use last 4k bytes of flash data area to store keymapping and configurations for Kermite
//A user application can still use the space until (filesystem_size - 4kB)

static Adafruit_FlashTransport_RP2040 flashTransport;
static Adafruit_SPIFlash flash(&flashTransport);

extern uint8_t _FS_start;
extern uint8_t _FS_end;

static const uint32_t flashDataAreaAddr = ((uint32_t)&_FS_start);
static const uint32_t flashDataAreaSize = ((uint32_t)(&_FS_end - &_FS_start));

static const int persistDataSize = flashPersistSector_DataSize;

static const uint32_t persistDataLocationOffset = flashDataAreaSize - persistDataSize;

void flashPersistSector_initialize() {
  if (flashDataAreaSize > 0) {
    flash.begin();
  }
}

void flashPersistSector_read(uint8_t *bytes4096) {
  if (flashDataAreaSize > 0) {
    flash.readBuffer(persistDataLocationOffset, bytes4096, persistDataSize);
  }
}
void flashPersistSector_write(uint8_t *bytes4096) {
  if (flashDataAreaSize > 0) {
    flash.eraseSector(persistDataLocationOffset / 4096);
    flash.writeBuffer(persistDataLocationOffset, bytes4096, persistDataSize);
  }
}
