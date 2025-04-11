#include "libraries/OLED.h"

OLED myOLED;

void setup() {
  Serial.begin(115200);
  myOLED.setup();
}

void loop() {
  int bpm = 78 + random(-3, 3);
  int spo2 = 97 + random(-1, 2);

  myOLED.update(bpm, spo2);

  delay(150);
}
