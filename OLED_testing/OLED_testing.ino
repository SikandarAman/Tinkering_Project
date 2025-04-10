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

// Simulated waveform data
int waveform[SCREEN_WIDTH];
int tick = 0;

void setup() {
  Serial.begin(115200);
  if (!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 init failed"));
    while (true);
  }

  display.clearDisplay();
  drawUI();
}

void loop() {
  drawWaveform();
  delay(50); // controls speed of waveform animation
}

// Draw static parts of the UI
void drawUI() {
  display.clearDisplay();

  // Labels
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 1);
  display.print("%SpO2");

  display.setCursor(90, 2);
  display.print("PRbpm");

  // Placeholder values
  display.setTextSize(2);
  display.setCursor(10, 12);
  display.print("97");

  display.setCursor(100, 12);
  display.print("89");

  // Optional: draw dividing lines
  //display.drawLine(0, 30, 127, 30, SSD1306_WHITE);

  display.display();
}

// Simulate and draw waveform
void drawWaveform() {
  // Shift waveform left
  for (int i = 0; i < SCREEN_WIDTH - 1; i++) {
    waveform[i] = waveform[i + 1];
  }

  // Add new simulated value
  int y = 50 + (int)(10 * sin(tick * 0.2));  // sine wave style
  waveform[SCREEN_WIDTH - 1] = y;
  tick++;

  // Draw waveform
  for (int x = 1; x < SCREEN_WIDTH; x++) {
    display.drawPixel(x, waveform[x], SSD1306_WHITE);
  }

  display.display();

  // Clear old waveform below line to refresh it
  display.fillRect(0, 31, 128, 33, SSD1306_BLACK);
  display.drawLine(0, 30, 127, 30, SSD1306_WHITE);  // redraw line
}
