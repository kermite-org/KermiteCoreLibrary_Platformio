#include <Arduino.h>
#include <KermiteCore.h>

const int pinKey0 = 2;
KermiteCore kermite;

void setup() {
  pinMode(pinKey0, INPUT_PULLUP);
  kermite.begin();
}

void loop() {
  static int cnt = 0;
  if (cnt % 10 == 0) {
    bool keyState = digitalRead(pinKey0);
    kermite.feedKeyState(0, keyState);
  }
  kermite.processUpdate();
  cnt++;
  delay(1);
}
