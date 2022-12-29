#include "dataStorage.h"
#include "../base/utils.h"
#include "../infrastructure/system.h"
#include "../infrastructure/xprintf.h"
#include "commandDefinitions.h"
#include "dataMemory.h"
#include "firmwareMetadata.h"
#include "kmTypes.h"
#include "versionDefinitions.h"
#include <stdio.h>

#ifndef KM0_STORAGE__USER_STORAGE_SIZE
#define KM0_STORAGE__USER_STORAGE_SIZE 0
#endif
const uint16_t UserStorageDataSize = KM0_STORAGE__USER_STORAGE_SIZE;

#ifndef KM0_KEYBOARD__NUM_CUSTOM_PARAMETERS
#define KM0_KEYBOARD__NUM_CUSTOM_PARAMETERS 0
#endif
const uint16_t CustomParametersDataSize = KM0_KEYBOARD__NUM_CUSTOM_PARAMETERS;
typedef struct {
  uint8_t projectId[6];
  uint8_t __reserved1[2];
  uint8_t __reserved2[8];
  uint8_t parametersDataInitializationFlag;
  uint8_t softwareStorageFormatRevision;
} T_SystemData;

const uint16_t SystemDataSize = sizeof(T_SystemData);
const uint16_t SystemParametersDataSize = sizeof(T_SystemParametersSet);
const uint16_t StorageHeadMagicNumber = 0xa37e;

//----------------------------------------------------------------------

enum {
  ChunkSig_UserData = 0xaa10,
  ChunkSig_SystemData = 0xaa20,
  ChunkSig_SystemParameters = 0xaa30,
  ChunkSig_CustomParameters = 0xaa40,
  ChunkSig_ProfileData = 0xaa70,
};

enum {
  SubChunkSig_ProfileDataHeader = 0xbb71,
  SubChunkSig_ProfileSettings = 0xbb72,
  SubChunkSig_ProfileLayerList = 0xbb74,
  SubChunkSig_ProfileMappingEntries = 0xbb76,
  SubChunkSig_ProfileKeyAssigns = 0xbb78,
};

static void seekChunk(uint16_t chunkSig, uint16_t posStart, uint16_t posEnd, uint16_t *out_addr, uint16_t *out_size) {
  uint16_t pos = posStart;
  // xprintf("--seek chunk %x\n", chunkSig);
  while (pos < posEnd) {
    uint16_t head = dataMemory_readWord(pos);
    uint16_t size = dataMemory_readWord(pos + 2);
    if (head == chunkSig) {
      pos += 4;
      *out_addr = pos;
      *out_size = size;
      // xprintf("chunk %x found, pos:%d size:%d\n", chunkSig, pos, size);
      return;
    } else {
      if (size == 0) {
        break;
      }
      pos += 4 + size;
    }
  }
  *out_addr = 0;
  *out_size = 0;
}

static void getChunkRegion(uint16_t chunkSig, uint16_t *out_addr, uint16_t *out_size) {
  seekChunk(chunkSig, 2, dataMemory_getCapacity(), out_addr, out_size);
}

static void getSubChunkRegion(uint16_t chunkSig, uint16_t subChunkSig, uint16_t *out_addr, uint16_t *out_size) {
  uint16_t parent_addr, parent_size;
  getChunkRegion(chunkSig, &parent_addr, &parent_size);
  if (parent_addr > 0) {
    seekChunk(subChunkSig, parent_addr, parent_addr + parent_size, out_addr, out_size);
  } else {
    *out_addr = 0;
    *out_size = 0;
  }
}

static uint16_t getChunkBodyAddress(uint16_t chunkSig) {
  uint16_t addr, size;
  getChunkRegion(chunkSig, &addr, &size);
  return addr;
}

static uint16_t getChunkBodySize(uint16_t chunkSig) {
  uint16_t addr, size;
  getChunkRegion(chunkSig, &addr, &size);
  return size;
}

static uint16_t getSubChunkBodyAddress(uint16_t chunkSig, uint16_t subChunkSig) {
  uint16_t addr, size;
  getSubChunkRegion(chunkSig, subChunkSig, &addr, &size);
  return addr;
}

static uint16_t getSubChunkBodySize(uint16_t chunkSig, uint16_t subChunkSig) {
  uint16_t addr, size;
  getSubChunkRegion(chunkSig, subChunkSig, &addr, &size);
  return size;
}

static uint16_t putBlankChunk(uint16_t addr, uint16_t chunkSig, uint16_t bodySize) {
  dataMemory_writeWord(addr, chunkSig);
  dataMemory_writeWord(addr + 2, bodySize);
  return addr + 4 + bodySize;
}

static uint16_t getKeyMappingDataBodyLengthMax() {
  uint16_t addr = getChunkBodyAddress(ChunkSig_ProfileData);
  if (addr) {
    return dataMemory_getCapacity() - addr;
  }
  return 0;
}

static uint8_t tempDataBuf[6];

static bool validateStorageDataFormat() {
  uint16_t first = dataMemory_readWord(0);
  if (first != StorageHeadMagicNumber) {
    return false;
  }
  uint16_t szUserData = getChunkBodySize(ChunkSig_UserData);
  uint16_t szSystemData = getChunkBodySize(ChunkSig_SystemData);
  uint16_t szSystemParameters = getChunkBodySize(ChunkSig_SystemParameters);
  uint16_t szCustomParameters = getChunkBodySize(ChunkSig_CustomParameters);
  uint16_t posKeyMappingDataBody = getChunkBodyAddress(ChunkSig_ProfileData);
  bool dataLayoutValid = (szUserData == UserStorageDataSize) &&
                         (szSystemData == SystemDataSize) &&
                         (szSystemParameters == SystemParametersDataSize) &&
                         (szCustomParameters == CustomParametersDataSize) &&
                         (posKeyMappingDataBody > 0);
  if (!dataLayoutValid) {
    return false;
  }
  //check project id stored in system data body
  uint16_t posSystemDataBody = getChunkBodyAddress(ChunkSig_SystemData);
  dataMemory_readBytes(posSystemDataBody, tempDataBuf, 6);
  bool projectIdValid = utils_compareBytes(
      tempDataBuf, (uint8_t *)commonFirmwareMetadataSecond.firmwareId, 6);
  if (!projectIdValid) {
    return false;
  }
  return true;
}

static void resetDataStorage() {
  xprintf("reset data storage for new project\n");
  dataMemory_clearAllZero();
  dataMemory_writeWord(0, StorageHeadMagicNumber);
  uint16_t pos = 2;
  //make regions
  if (UserStorageDataSize > 0) {
    pos = putBlankChunk(pos, ChunkSig_UserData, UserStorageDataSize);
  }
  pos = putBlankChunk(pos, ChunkSig_SystemData, SystemDataSize);
  pos = putBlankChunk(pos, ChunkSig_SystemParameters, SystemParametersDataSize);
  if (CustomParametersDataSize > 0) {
    pos = putBlankChunk(pos, ChunkSig_CustomParameters, CustomParametersDataSize);
  }
  pos = putBlankChunk(pos, ChunkSig_ProfileData, 0);

  uint16_t posSystemDataBody = getChunkBodyAddress(ChunkSig_SystemData);
  if (posSystemDataBody) {
    //write project id
    dataMemory_writeBytes(
        posSystemDataBody,
        (uint8_t *)commonFirmwareMetadataSecond.firmwareId, 6);
    //write default instance number
    dataMemory_writeBytes(posSystemDataBody + 8, (uint8_t *)"00000000", 8);
  }
}

//----------------------------------------------------------------------
//exports

void dataStorage_initialize() {
  xprintf("data storage initialize\n");
  bool storageValid = validateStorageDataFormat();
  if (!storageValid) {
    resetDataStorage();
  } else {
    xprintf("storage data valid\n");
  }
}

uint16_t dataStorage_getKeyMappingDataCapacity() {
  return getKeyMappingDataBodyLengthMax();
}

uint16_t dataStorage_getDataAddress_storageSystemParametersRevision() {
  uint16_t base = getChunkBodyAddress(ChunkSig_SystemData);
  return base + 16;
}

uint16_t dataStorage_getDataAddress_systemParameters() {
  return getChunkBodyAddress(ChunkSig_SystemParameters);
}

uint16_t dataStorage_getDataSize_systemParameters() {
  return SystemParametersDataSize;
}

uint16_t dataStorage_getDataAddress_profileData() {
  return getChunkBodyAddress(ChunkSig_ProfileData);
}

uint16_t dataStorage_getDataAddress_profileData_profileHeader() {
  return getSubChunkBodyAddress(ChunkSig_ProfileData, SubChunkSig_ProfileDataHeader);
}

uint16_t dataStorage_getDataAddress_profileData_profileSettings() {
  return getSubChunkBodyAddress(ChunkSig_ProfileData, SubChunkSig_ProfileSettings);
}

uint16_t dataStorage_getDataAddress_profileData_layerList() {
  return getSubChunkBodyAddress(ChunkSig_ProfileData, SubChunkSig_ProfileLayerList);
}

uint16_t dataStorage_getDataAddress_profileData_mappingEntries() {
  return getSubChunkBodyAddress(ChunkSig_ProfileData, SubChunkSig_ProfileMappingEntries);
}

uint16_t dataStorage_getDataAddress_profileData_keyAssigns() {
  return getSubChunkBodyAddress(ChunkSig_ProfileData, SubChunkSig_ProfileKeyAssigns);
}

uint16_t dataStorage_getDataSize_profileData_keyAssigns() {
  return getSubChunkBodySize(ChunkSig_ProfileData, SubChunkSig_ProfileKeyAssigns);
}
