#include <Arduino.h>

//Tiny2040, blink

const int pinLED = 19;

void app4Entry() {
  pinMode(pinLED, OUTPUT);
  pinMode(18, OUTPUT);
  digitalWrite(18, HIGH);
  pinMode(20, OUTPUT);
  digitalWrite(20, HIGH);

  while (1) {
    digitalWrite(pinLED, !digitalRead(pinLED));
    delay(1000);
    yield();
  }
}
