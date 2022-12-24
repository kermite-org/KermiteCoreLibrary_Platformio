#include <Arduino.h>

// tested on Tiny2040

void setup() {
  pinMode(20, OUTPUT);
  digitalWrite(20, HIGH);
  pinMode(19, OUTPUT);
  digitalWrite(19, HIGH);
  pinMode(18, OUTPUT);
  digitalWrite(18, HIGH);
}

void loop() {
  digitalWrite(20, !digitalRead(20));
  delay(100);
}
