#include <Arduino.h>
#include <KermiteCore.h>
#include <kpm/BoardLED.h>
#include <kpm/BoardLED_NeoPixel.h>
#include <kpm/SimpleButton.h>

static BoardLED_NeoPixel boardLED(11, 0x40); //qt py m0
static SimpleButton button(6);

static KermiteCore kermite;
static bool pressed;

static void updateButton() {
  button.update();
  pressed = button.hold;
  kermite.feedKeyState(0, pressed);
}

void appEntry() {
  boardLED.initialize();

  kermite.setFlashSavingWaitTimeSec(20);
  kermite.setKeyboardName("mykeeb");
  kermite.enableDebugLogging();

  boardLED.write(0, true);
  kermite.begin();
  boardLED.write(0, false);

  int count = 0;
  while (1) {
    bool heartbeat = (count % 1000 == 0);
    boardLED.write(0, heartbeat || pressed);
    if (count % 10 == 0) {
      updateButton();
    }
    kermite.processUpdate();
    count++;
    yield();
    delay(1);
  }
}
