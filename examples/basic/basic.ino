#include <KermiteCore.h>

KermiteCore kermite;
int pinButton = D2; //set the pin connected to a key
int count = 0;

void setup() {
  pinMode(pinButton, INPUT_PULLUP);
  kermite.setKeyboardName("mykeeb");
  kermite.begin();
}

void loop() {
  if (count % 10 == 0) {
    bool pressed = digitalRead(pinButton) == LOW;
    kermite.feedKeyState(0, pressed);
  }
  kermite.processUpdate();
  count++;
  delay(1);
}
