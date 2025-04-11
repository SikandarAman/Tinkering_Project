#ifndef OLED_H
#define OLED_H

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_MOSI 23
#define OLED_CLK  18
#define OLED_DC   16
#define OLED_CS   5
#define OLED_RST  17

class OLED {
public:
  OLED() : display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RST, OLED_CS) {}

  void setup() {
    if (!display.begin(SSD1306_SWITCHCAPVCC)) {
      Serial.println(F("SSD1306 init failed"));
      while (1);
    }

    drawUI();

    // Initialize graph buffer
    for (int i = 0; i < SCREEN_WIDTH; i++) {
      heartRateBuffer[i] = -1;
    }
  }

  void update(int bpm, int spO2) {
    // Simulate value for graph bpm
    graphbpm = t[random(0, 3)];
    updateInfo(bpm, spO2);
    updateGraph(graphbpm);
  }

private:
  Adafruit_SSD1306 display;

  // Member variables
  int heartRateBuffer[SCREEN_WIDTH];
  const int minBPM = 50;
  const int maxBPM = 150;
  int graphbpm = 0;
  int t[3] = {60, 120, 80};

  void drawUI() {
    display.clearDisplay();

    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 3);
    display.print("BPM");

    display.setCursor(90, 3);
    display.print("%SpO2");

    display.drawLine(0, 0, 127, 0, SSD1306_WHITE);
    display.drawLine(0, 32, 127, 32, SSD1306_WHITE);

    display.display();
  }

  void updateInfo(int heartrate, int spO2) {
    display.fillRect(0, 12, SCREEN_WIDTH, 20, SSD1306_BLACK);

    display.setTextSize(2);
    display.setCursor(10, 13);
    display.print(heartrate);

    display.setCursor(100, 13);
    display.print(spO2);

    display.display();
  }

  void updateGraph(int heartRate) {
    for (int i = 0; i < SCREEN_WIDTH - 1; i++) {
      heartRateBuffer[i] = heartRateBuffer[i + 1];
    }

    int y = map(heartRate, minBPM, maxBPM, SCREEN_HEIGHT - 1, 32);
    y = constrain(y, 0, SCREEN_HEIGHT - 1);
    heartRateBuffer[SCREEN_WIDTH - 1] = y;

    display.fillRect(0, 34, SCREEN_WIDTH, 34, SSD1306_BLACK);

    for (int x = 1; x < SCREEN_WIDTH; x++) {
      if (heartRateBuffer[x - 1] >= 0 && heartRateBuffer[x] >= 0) {
        display.drawLine(x - 1, heartRateBuffer[x - 1], x, heartRateBuffer[x], SSD1306_WHITE);
      }
    }

    display.display();
  }
};

#endif // OLED_H
