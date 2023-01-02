#include <Arduino.h>

//blink, Tiny2040

const int pinLED = 20;

void app4Entry() {
  pinMode(pinLED, OUTPUT);
  pinMode(19, OUTPUT);
  digitalWrite(19, HIGH);
  pinMode(18, OUTPUT);
  digitalWrite(18, HIGH);

  while (1) {
    digitalWrite(pinLED, !digitalRead(pinLED));
    delay(1000);
    yield();
  }
}
