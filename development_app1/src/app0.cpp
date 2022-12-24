#include <Adafruit_TinyUSB.h>
#include <Arduino.h>
#include <KermiteCore.h>

// tested on Tiny2040

static KermiteCore kermite;

static const int pinLEDs[3] = { 20, 19, 18 };

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

static void initBoardButton() {
  pinMode(7, INPUT_PULLUP);
}

static bool readBoardButton() {
  return digitalRead(7) == LOW;
}

static void updateButton() {
  static bool pressed = false;
  bool nextPressed = readBoardButton();
  if (!pressed && nextPressed) {
    Serial.printf("pressed\n");
    kermite.issueInputKeyStateChange(0, true);
  }
  if (pressed && !nextPressed) {
    Serial.printf("released\n");
    kermite.issueInputKeyStateChange(0, false);
  }
  pressed = nextPressed;
  writeBoardLED(1, pressed);
}

void app0Entry() {
  initBoardLEDs();
  initBoardButton();

  writeBoardLED(0, true);
  kermite.begin();
  writeBoardLED(0, false);

  while (1) {
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
    yield();
    delay(1);
  }
}
