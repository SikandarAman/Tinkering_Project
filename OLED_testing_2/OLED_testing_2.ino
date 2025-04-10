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

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RST, OLED_CS);

// Plotting buffer
int heartRateBuffer[SCREEN_WIDTH];  // 1 value per pixel column

// Graph Y range
const int minBPM = 50;
const int maxBPM = 150;

void setup() {
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 init failed"));
    while (1);
  }
  drawUI();

  // Init graph buffer with 0s
  for (int i = 0; i < SCREEN_WIDTH; i++) {
    heartRateBuffer[i] = -1;  // use -1 to indicate uninitialized
  }
}

// Simulated BPM values (replace with sensor later)
int bpm = 70;
int t = 0;

void loop() {
  // Simulate variation in BPM
  bpm = 75 + random(-15,20);
  

  updateGraph(bpm);
  updateGraph(bpm);
  delay(120);
}

void updateGraph(int heartRate) {
  // Scroll buffer left
  for (int i = 0; i < SCREEN_WIDTH - 1; i++) {
    heartRateBuffer[i] = heartRateBuffer[i + 1];
  }

  // Map BPM to Y pixel (invert Y because OLED origin is top-left)
  int y = map(heartRate, minBPM, maxBPM, SCREEN_HEIGHT - 1, 32);  // only upper half used
  y = constrain(y, 0, SCREEN_HEIGHT - 1);

  heartRateBuffer[SCREEN_WIDTH - 1] = y;

  // Clear old graph area
  display.fillRect(0, 34, SCREEN_WIDTH, 34, SSD1306_BLACK);

  // Draw graph
  for (int x = 1; x < SCREEN_WIDTH; x++) {
    if (heartRateBuffer[x - 1] >= 0 && heartRateBuffer[x] >= 0) {
      display.drawLine(x - 1, heartRateBuffer[x - 1], x, heartRateBuffer[x], SSD1306_WHITE);
    }
  }

  display.display();
}

void drawUI() {
  display.clearDisplay();

  // Labels
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 3);
  display.print("%SpO2");

  display.setCursor(90, 3);
  display.print("PRbpm");

  // Placeholder values
  display.setTextSize(2);
  display.setCursor(10, 13);
  display.print("97");

  display.setCursor(100, 13);
  display.print("89");

  // Optional: draw dividing lines
  display.drawLine(0, 0, 127, 0, SSD1306_WHITE);
  display.drawLine(0, 32, 127, 32, SSD1306_WHITE);

  display.display();
}
