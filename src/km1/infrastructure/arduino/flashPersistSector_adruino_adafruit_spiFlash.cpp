#include "../buildCondition.h"
#if defined(KERMITECORE_USE_FLASHSTORAGE_ADAFRUIT_SPIFLASH)

#include "../flashPersistSector.h"
#include "../kprintf.h"
#include <Adafruit_FlashTransport.h>
#include <Adafruit_SPIFlash.h>

//Use latter 4k bytes of flash data area to store keymapping and configurations for Kermite
//A user application can still use the space until (filesystem_size - 4kB)

#if defined(ARDUINO_ARCH_RP2040)
static Adafruit_FlashTransport_RP2040 flashTransport;
#endif

static Adafruit_SPIFlash flash(&flashTransport);

static const int persistDataSize = flashPersistSector_DataSize;
static int32_t locationOffset = -1;

void ensureInitialized() {
  static bool initialized = false;
  if (!initialized) {
    flash.begin();
    uint32_t fsSize = flash.size();
    if (fsSize >= persistDataSize) {
      locationOffset = fsSize - persistDataSize;
    }
    initialized = true;
  }
}

void flashPersistSector_read(uint8_t *bytes4096) {
  ensureInitialized();
  if (locationOffset >= 0) {
    flash.readBuffer(locationOffset, bytes4096, persistDataSize);
  }
}
void flashPersistSector_write(uint8_t *bytes4096) {
  ensureInitialized();
  if (locationOffset >= 0) {
    flash.eraseSector(locationOffset / 4096);
    flash.writeBuffer(locationOffset, bytes4096, persistDataSize);
  }
}

#endif
