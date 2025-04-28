#include "libraries/OLED.h"
#include <Wire.h>
#include <MAX30100_PulseOximeter.h>

// Create OLED object
OLED myOLED;

// Create PulseOximeter object
PulseOximeter pox;

// For sampling updates
const uint32_t UPDATE_INTERVAL_MS = 1000;  // Update every 1 second
uint32_t tsLastReport = 0;

// Variables to store sensor readings
float bpm = 0;
float spo2 = 0;

// Callback when a beat is detected
void onBeatDetected() {
  Serial.println("Beat detected!");
}

void setup() {
  Serial.begin(115200);

  // Initialize OLED
  myOLED.setup();

  // Initialize MAX30100
  Serial.println("Initializing MAX30100 sensor...");
  if (!pox.begin()) {
    Serial.println("FAILED to initialize MAX30100 sensor!");
    while (1);
  } else {
    Serial.println("MAX30100 sensor initialized successfully!");
  }

  // Setup beat detection callback
  pox.setOnBeatDetectedCallback(onBeatDetected);
}

void loop() {
  // Continuously update the sensor readings
  pox.update();

  // Every UPDATE_INTERVAL_MS milliseconds, fetch BPM and SpO2
  if (millis() - tsLastReport > UPDATE_INTERVAL_MS) {
    tsLastReport = millis();
    bpm = pox.getHeartRate();
    spo2 = pox.getSpO2();

    // Only update if we have valid readings
    if (bpm != 0 && spo2 != 0) {
      Serial.print("BPM: ");
      Serial.print(bpm);
      Serial.print(" | SpO2: ");
      Serial.println(spo2);

      // Send data to OLED
      myOLED.update((int)bpm, (int)spo2);
    }
  }
}
