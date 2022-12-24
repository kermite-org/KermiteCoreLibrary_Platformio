#include <Adafruit_TinyUSB.h>
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

extern Adafruit_USBD_Device USBDevice;

static const uint8_t desc_hid_report[] = {TUD_HID_REPORT_DESC_KEYBOARD()};

static Adafruit_USBD_HID usb_hid;

void setup() {
  initBoardLEDs();
  initBoardButton();

  usb_hid.setPollInterval(2);
  usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));

  usb_hid.begin();
  writeBoardLED(0, true);
  while (!USBDevice.mounted())
    delay(1);
  writeBoardLED(0, false);
}

static void sendHidKey(char chr, bool pressed) {
  if (usb_hid.ready()) {
    if (pressed) {
      usb_hid.keyboardPress(0, chr);
    } else {
      usb_hid.keyboardRelease(0);
    }
  }
}

static void updateButton() {
  static bool pressed = false;
  bool nextPressed = readBoardButton();
  if (!pressed && nextPressed) {
    Serial.printf("pressed\n");
    sendHidKey('a', true);
  }
  if (pressed && !nextPressed) {
    Serial.printf("released\n");
    sendHidKey('a', false);
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
