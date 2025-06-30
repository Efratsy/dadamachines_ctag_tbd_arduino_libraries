/**
 * @file CTAG_LED_Demo.ino
 * @brief Demonstration sketch for the CTAG_LED library.
 *
 * This example initializes a single NeoPixel LED and cycles it through
 * Red, Green, Blue, and Off states in a non-blocking manner.
 */

#include <CTAG_LED.h>
#include "pins_arduino.h" // Required to use standardized pin names

// --- Configuration ---
// Define which pin is used for the NeoPixel data line.
#define PIXEL_PIN     PIN_NEOPIXELS
#define NUMPIXELS     1

/**
 * @brief Global instance of our CTAG_LED library.
 */
CTAG_LED leds(NUMPIXELS, PIXEL_PIN);

/**
 * @brief Variables for the non-blocking state machine.
 */
int colorState = 0;                 // Current state (0=Red, 1=Green, 2=Blue, 3=Off)
unsigned long lastChangeTime = 0;   // Timestamp of the last color change.
const long interval = 1000;         // Interval to stay on each color (in milliseconds).


/**
 * @brief Runs once at startup to initialize hardware.
 */
void setup() {
  Serial.begin(115200);
  Serial.println("\n--- CTAG_LED Non-Blocking Demo ---");

  // Initialize the LED library.
  // The begin() function will also turn the LED off initially.
  leds.begin();
  
  // Set a moderate brightness for the LED.
  leds.setBrightness(50);
}

/**
 * @brief Runs continuously. Uses a non-blocking timer to cycle colors.
 */
void loop() {
  // Check if the desired interval has passed since the last color change.
  if (millis() - lastChangeTime >= interval) {
    
    // Update the timestamp for the next change.
    lastChangeTime = millis();

    // Change the LED color based on the current state.
    switch (colorState) {
      case 0: // State 0: Red
        Serial.println("LED state: Red");
        leds.setPixelColor(0, 255, 0, 0); // Set the first pixel (index 0) to Red
        break;
      case 1: // State 1: Green
        Serial.println("LED state: Green");
        leds.setPixelColor(0, 0, 255, 0); // Set the first pixel to Green
        break;
      case 2: // State 2: Blue
        Serial.println("LED state: Blue");
        leds.setPixelColor(0, 0, 0, 255); // Set the first pixel to Blue
        break;
      case 3: // State 3: Off
        Serial.println("LED state: Off");
        leds.setPixelColor(0, 0, 0, 0);   // Set the first pixel to Off
        break;
    }

    // Send the new color data to the physical LED.
    leds.show();

    // Advance to the next state for the next cycle.
    colorState++;
    if (colorState > 3) {
      colorState = 0; // Reset state machine to loop back to Red.
    }
  }

  // You can add other non-blocking code here.
  // It will run continuously without being stopped by delays.
}