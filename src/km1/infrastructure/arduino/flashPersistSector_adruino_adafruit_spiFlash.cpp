#include "../buildCondition.h"
#if defined(KERMITECORE_USE_FLASHSTORAGE_ADAFRUIT_SPIFLASH)

#include "../flashPersistSector.h"
#include <Adafruit_FlashTransport.h>
#include <Adafruit_SPIFlash.h>

//Use last 4k bytes of flash data area to store keymapping and configurations for Kermite
//A user application can still use the space until (filesystem_size - 4kB)

#if defined(ARDUINO_ARCH_RP2040)
static Adafruit_FlashTransport_RP2040 flashTransport;
#endif

#if defined(ARDUINO_ARCH_SAMD)
#if defined(EXTERNAL_FLASH_USE_SPI)
Adafruit_FlashTransport_SPI flashTransport(EXTERNAL_FLASH_USE_CS,
                                           EXTERNAL_FLASH_USE_SPI);
#endif
#endif

static Adafruit_SPIFlash flash(&flashTransport);

static const int persistDataSize = flashPersistSector_DataSize;

void ensureInitialized() {
  static bool initialized = false;
  if (!initialized) {
    flash.begin();
    initialized = true;
  }
}

void flashPersistSector_read(uint8_t *bytes4096) {
  int offset = flash.size() - persistDataSize;
  if (offset > 0) {
    ensureInitialized();
    flash.readBuffer(offset, bytes4096, persistDataSize);
  }
}
void flashPersistSector_write(uint8_t *bytes4096) {
  int offset = flash.size() - persistDataSize;
  if (offset > 0) {
    ensureInitialized();
    flash.eraseSector(offset / 4096);
    flash.writeBuffer(offset, bytes4096, persistDataSize);
  }
}

#endif
