
#include <Arduino.h>

extern void app0Entry();
extern void app1Entry();
extern void app2Entry();

void setup() {
  app0Entry();
  // app1Entry();
  //app2Entry();
}

void loop() {}