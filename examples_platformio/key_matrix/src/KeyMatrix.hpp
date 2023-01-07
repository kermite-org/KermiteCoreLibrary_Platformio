#pragma once
#include <Arduino.h>

typedef void (*KeyStateListenerFn)(int keyIndex, bool pressed);

class KeyMatrix {
private:
  const int *columnPins;
  const int *rowPins;
  int numColumns;
  int numRows;
  KeyStateListenerFn keyStateListener;
  bool *inputKeyStates;

public:
  bool *keyStates;

  KeyMatrix(const int *_columnPins, const int *_rowPins, int _numColumns, int _numRows) {
    columnPins = _columnPins;
    rowPins = _rowPins;
    numColumns = _numColumns;
    numRows = _numRows;
    keyStateListener = nullptr;
    inputKeyStates = new bool[numColumns * numRows];
    keyStates = new bool[numColumns * numRows];
  }

  void setKeyStateListener(KeyStateListenerFn fn) {
    keyStateListener = fn;
  }

  void initialize() {
    for (int i = 0; i < numColumns; i++) {
      pinMode(columnPins[i], INPUT_PULLUP);
    }
    for (int i = 0; i < numRows; i++) {
      pinMode(rowPins[i], INPUT_PULLUP);
    }
  }

  void updateInput() {
    for (int row = 0; row < numRows; row++) {
      int rowPin = rowPins[row];
      pinMode(rowPin, OUTPUT);
      digitalWrite(rowPin, LOW);
      for (int col = 0; col < numColumns; col++) {
        int keyIndex = row * numColumns + col;
        inputKeyStates[keyIndex] = digitalRead(columnPins[col]) == LOW;
      }
      pinMode(rowPin, INPUT_PULLUP);
    }
    int numKeySlots = numColumns * numRows;
    for (int i = 0; i < numKeySlots; i++) {
      if (keyStateListener) {
        bool curr = keyStates[i];
        bool next = inputKeyStates[i];
        if (!curr && next) {
          keyStateListener(i, true);
        }
        if (curr && !next) {
          keyStateListener(i, false);
        }
      }
      keyStates[i] = inputKeyStates[i];
    }
  }
};
