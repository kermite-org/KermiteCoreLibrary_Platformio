#pragma once

#include <stdint.h>

class KermiteCore {
public:
  void begin();
  void feedKeyState(int keyIndex, bool hold);
  void processUpdate();
};
