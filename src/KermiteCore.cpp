#include "KermiteCore.h"
#include "km1/domain/firmwareMetadata.h"
#include "km1/domain/keyboardMain.h"

KermiteCore::KermiteCore() {
  firmwareMetadata_initialize();
}

void KermiteCore::begin() {
  keyboardMain_initialize();
}

void KermiteCore::setKeyboardName(const char *keyboardName) {
  firmwareMetaData_setKeyboardName(keyboardName);
}

void KermiteCore::setFirmwareId(const char *firmwareId) {
  firmwareMetaData_setFirmwareId(firmwareId);
}

void KermiteCore::feedKeyState(int keyIndex, bool pressed) {
  keyboardMain_feedKeyState(keyIndex, pressed);
}

void KermiteCore::processUpdate() {
  keyboardMain_processUpdate();
}
