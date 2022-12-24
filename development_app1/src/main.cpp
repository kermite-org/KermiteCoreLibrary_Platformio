#include <Arduino.h>

// tested on Tiny2040

static const int pinLEDs[3] = {20, 19, 18};

static void initBoardLEDs() {
  for (int i = 0; i < 3; i++) {
    int pin = pinLEDs[i];
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
  }
}

static void writeBoardLED(int index, bool state) {
  int pin = pinLEDs[index];
  digitalWrite(pin, !state);
}

static void initBoardButton() { pinMode(7, INPUT_PULLUP); }

static bool readBoardButton() { return digitalRead(7) == LOW; }

void setup() {
  initBoardLEDs();
  initBoardButton();
}

void updateButton() {
  static bool pressed = false;
  bool nextPressed = readBoardButton();
  if (!pressed && nextPressed) {
    Serial.printf("pressed\n");
  }
  if (pressed && !nextPressed) {
    Serial.printf("released\n");
  }
  pressed = nextPressed;
  writeBoardLED(1, pressed);
}

void loop() {
  static int cnt = 0;
  if (cnt % 1000 == 0) {
    writeBoardLED(0, true);
  }
  if (cnt % 1000 == 1) {
    writeBoardLED(0, false);
  }
  if (cnt % 10 == 0) {
    updateButton();
  }
  cnt++;
  delay(1);
}
