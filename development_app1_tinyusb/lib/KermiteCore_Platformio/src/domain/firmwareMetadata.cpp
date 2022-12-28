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
  { '$', 'K', 'M', 'M', 'D' }, //dataHeader
  "000000",                    //projectId
  "00",                        //variationId
  "0000",                      //deviceInstanceCode
  KERMITE_KEYBOARD_NAME,       //keyboardName
};
