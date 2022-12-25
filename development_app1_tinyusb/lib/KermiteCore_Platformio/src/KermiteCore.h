#pragma once

#include <stdint.h>

class KermiteCore {
public:
  void begin();
  void issueKeyState(uint8_t keyIndex, bool isDown);
  void processUpdate();
};
