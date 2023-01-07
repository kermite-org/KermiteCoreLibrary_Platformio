#include <Arduino.h>
#include <BoardLED.hpp>
#include <BoardLED_NeoPixel.hpp>
#include <KermiteCore.h>
#include <SimpleButton.hpp>

static BoardLED boardLED(20, 19, 18, true); //tiny2040
//static BoardLED boardLED(25, -1, -1, false); //pico
//static BoardLED_NeoPixel boardLED(17, 0x40); //kb2040
//static BoardLED_NeoPixel boardLED(16, 0x10); //rp2040-zero
//static BoardLED_NeoPixel boardLED(12, 0x40, 11); //xiao rp2040

static SimpleButton button(23); //set an input pin for your board

static KermiteCore kermite;
static bool pressed;

static void updateButton() {
  button.update();
  pressed = button.hold;
  kermite.feedKeyState(0, pressed);
}

void appEntry() {
  boardLED.initialize();

  kermite.setKeyboardName("mykeeb");
  kermite.enableDebugLogging();

  boardLED.write(0, true);
  kermite.begin();
  boardLED.write(0, false);

  int cnt = 0;
  while (1) {
    bool heartbeat = (cnt % 1000 == 0);
    boardLED.write(0, heartbeat || pressed);
    if (cnt % 10 == 0) {
      updateButton();
    }
    kermite.processUpdate();
    cnt++;
    yield();
    delay(1);
  }
}
