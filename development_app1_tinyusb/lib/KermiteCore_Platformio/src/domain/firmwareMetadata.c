#include "firmwareMetadata.h"
#include "base/utils.h"
#include "kmTypes.h"

#ifndef KERMITE_FIRMWARE_ID
#error KERMITE_FIRMWARE_ID is not defined
#endif

#ifndef KERMITE_KEYBOARD_NAME
#error KERMITE_KEYBOARD_NAME is not defined
#endif

CommonFirmwareMetadata commonFirmwareMetadata = {
  .dataHeader = { '$', 'K', 'M', 'M', 'D' },
  .projectId = "000000",
  .variationId = "00",
  .deviceInstanceCode = "0000",
  .keyboardName = KERMITE_KEYBOARD_NAME,
};
