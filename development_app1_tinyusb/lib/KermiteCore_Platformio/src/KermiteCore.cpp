#include "KermiteCore.h"
#include "km1/domain/keyboardMain.h"

void KermiteCore::begin() {
  keyboardMain_initialize();
}

void KermiteCore::feedKeyState(int keyIndex, bool pressed) {
  keyboardMain_feedKeyState(keyIndex, pressed);
}

void KermiteCore::processUpdate() {
  keyboardMain_processUpdate();
}
