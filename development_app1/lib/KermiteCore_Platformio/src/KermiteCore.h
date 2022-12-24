#pragma once

#include <stdint.h>

class KermiteCore {
public:
  void begin();
  void issueInputKeyStateChange(uint8_t keyIndex, bool isDown);
};
