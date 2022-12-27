#include "../debugUtils.h"
#include <Arduino.h>

void debugUtils_printBytes(uint8_t *buf, int len) {
  for (int i = 0; i < len; i++) {
    Serial.printf("%02X ", buf[i]);
  }
  Serial.println();
}

void debugUtils_printBytesDec(uint8_t *buf, int len) {
  for (int i = 0; i < len; i++) {
    Serial.printf("%d ", buf[i]);
  }
  Serial.println();
}
