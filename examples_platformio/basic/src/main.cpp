#include <Arduino.h>
#include <KermiteCore.h>

KermiteCore kermite;
int pinButton = 2;

void setup() {
  pinMode(pinButton, INPUT_PULLUP);
  kermite.setKeyboardName("mykeeb");
  kermite.begin();
}

int cnt = 0;
void loop() {
  if (cnt % 10 == 0) {
    bool pressed = digitalRead(pinButton) == LOW;
    kermite.feedKeyState(0, pressed);
  }
  kermite.processUpdate();
  cnt++;
  delay(1);
}
