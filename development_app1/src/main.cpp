#include <Arduino.h>

// tested on Tiny2040

void setup() {
  pinMode(20, OUTPUT);
  digitalWrite(20, HIGH);
  pinMode(19, OUTPUT);
  digitalWrite(19, HIGH);
  pinMode(18, OUTPUT);
  digitalWrite(18, HIGH);

  pinMode(7, INPUT_PULLUP);
}

void updateButton() {
  static bool pressed = false;
  bool nextPressed = digitalRead(7) == LOW;
  if (!pressed && nextPressed) {
    Serial.printf("pressed\n");
  }
  if (pressed && !nextPressed) {
    Serial.printf("released\n");
  }
  pressed = nextPressed;
  digitalWrite(19, !pressed);
}

void loop() {
  static int cnt = 0;
  if (cnt % 1000 == 0) {
    digitalWrite(20, LOW);
  }
  if (cnt % 1000 == 1) {
    digitalWrite(20, HIGH);
  }
  if (cnt % 10 == 0) {
    updateButton();
  }
  cnt++;
  delay(1);
}
