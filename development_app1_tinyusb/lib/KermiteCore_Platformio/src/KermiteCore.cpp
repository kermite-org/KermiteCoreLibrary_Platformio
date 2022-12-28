#include "KermiteCore.h"
#include "domain/dataMemory.h"
#include "domain/keyboardMainInternal.h"
#include "infrastructure/debugUtils.h"
#include "infrastructure/flashPersistSector.h"
#include "infrastructure/usbIoCore.h"

void KermiteCore::begin() {
  keyboardMain_initialize();
}

void KermiteCore::feedKeyState(int keyIndex, bool pressed) {
  keyboardMain_feedKeyState(keyIndex, pressed);
}

void KermiteCore::processUpdate() {
  keyboardMain_processUpdate();
}
