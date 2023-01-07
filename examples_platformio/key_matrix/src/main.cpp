#include "BoardLED_NeoPixel.hpp"
#include "KeyMatrix.hpp"
#include <Arduino.h>
#include <KermiteCore.h>

KermiteCore kermite;

BoardLED_NeoPixel boardLED(25); //promicro rp2040

const int numColumns = 7;
const int numRows = 4;
const int pinColumns[numColumns] = { 28, 27, 26, 22, 20, 23, 21 };
const int pinRows[numRows] = { 6, 7, 8, 9 };

KeyMatrix keyMatrix(pinColumns, pinRows, numColumns, numRows);

int pressedKeyCount = 0;

void handleKeyStateChange(int keyIndex, bool pressed) {
  kermite.feedKeyState(keyIndex, pressed);
  pressedKeyCount += (pressed ? 1 : -1);
}

void setup() {
  boardLED.initialize();
  boardLED.setBrightness(0x40);

  keyMatrix.setKeyStateListener(handleKeyStateChange);
  keyMatrix.initialize();

  kermite.setKeyboardName("mykeeb");
  kermite.begin();
}

void loop() {
  static int count = 0;
  boardLED.write(0, count % 2000 == 0);
  if (count % 10 == 0) {
    keyMatrix.updateInput();
    boardLED.write(2, pressedKeyCount > 0);
  }
  kermite.processUpdate();
  count++;
  delay(1);
}
