/**
 * @file CTAG_ExtensionBoard_Demo.ino
 * @brief A comprehensive demo for the CTAG_ExtensionBoard library.
 *
 * This sketch demonstrates how to:
 * 1. Initialize the board and LEDs.
 * 2. Continuously read all input data (buttons, pots, encoder) from the board.
 * 3. Provide visual feedback for all inputs using the onboard NeoPixel LEDs.
 * 4. Print the state of potentiometers and function buttons to the serial monitor
 * at a fixed interval to avoid flooding the output.
 */

#include <CTAG_ExtensionBoard.h>
#include "pins_arduino.h" // Required to use standardized pin names

// ========== Configuration ==========
// These values must match your hardware setup.

// Use the standardized pin names from your board's "pins_arduino.h" file.
#define LED_PIN       PIN_NEOPIXELS
#define I2C_SDA_PIN   PIN_WIRE1_SDA
#define I2C_SCL_PIN   PIN_WIRE1_SCL
const uint8_t LED_COUNT = 17;

/**
 * @brief Global instance of the extension board library.
 */
CTAG_ExtensionBoard ext(LED_COUNT, LED_PIN);

/**
 * @brief Timer variables for throttling the serial output.
 */
unsigned long lastPrintTime = 0;
const long printInterval = 200; // Interval for serial printing in milliseconds.

/**
 * @brief Runs once at startup to initialize hardware.
 */
void setup() {
  Serial.begin(115200);
  while (!Serial) {
    delay(10); // Wait for the serial console to connect.
  }
  Serial.println("\n--- CTAG_ExtensionBoard: Full Demo with Serial Print ---");

  // Initialize the I2C connection and LEDs via the library.
  ext.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  
  // Set a moderate brightness for the LEDs.
  ext.setBrightness(10);
  
  Serial.println("Board initialized and ready.");
}

/**
 * @brief Runs continuously after setup to handle I/O.
 */
void loop() {
  // --- 1. Always fetch the latest data from the board first. ---
  if (!ext.update()) {
    Serial.println("I2C connection failed!");
    delay(200);
    return;
  }

  // --- 2. Handle the LED logic (runs every loop for maximum responsiveness). ---

  // Logic for the encoder LED (at index 0)
  if (ext.isEncoderPressed()) {
    ext.setLed(0, 0, 255, 0); // Green
  } else if (ext.isEncoderRotatedForward() || ext.isEncoderRotatedBackward()) {
    ext.setLed(0, 255, 0, 0); // Red on any rotation
  } else {
    ext.setLed(0, 0, 0, 0);   // Off
  }

  // Logic for the 16 digital button LEDs (at indices 1-16)
  for (int i = 0; i < 16; i++) {
    uint8_t ledIndex = i + 1;
    if (ext.isButtonLongPressed(i)) {
      ext.setLed(ledIndex, 255, 0, 0);    // Red for long press
    } else if (ext.isButtonPressed(i)) {
      ext.setLed(ledIndex, 0, 255, 0);    // Green for short press
    } else {
      ext.setLed(ledIndex, 64, 64, 64);   // Gray when idle
    }
  }

  // --- 3. Send all updated LED colors to the hardware at once. ---
  ext.show();


  // --- 4. Handle serial output only every 'printInterval' milliseconds. ---
  if (millis() - lastPrintTime >= printInterval) {
    lastPrintTime = millis(); // Update the timestamp for the next print

    // Output for the 4 potentiometers
    Serial.print("Pots [0-3]:\t");
    for (int i = 0; i < 4; i++) {
      Serial.print(ext.getPot(i));
      if (i < 3) {
        Serial.print("\t"); // Tab for clean spacing
      }
    }
    Serial.println(); // Newline for the next output

    // Output for the 4 function buttons
    Serial.print("F-Btns [0-3]:\t");
    for (int i = 0; i < 4; i++) {
      if (ext.isFButtonLongPressed(i)) {
        Serial.print("LONG ");
      } else if (ext.isFButtonPressed(i)) {
        Serial.print("SHORT");
      } else {
        Serial.print(" OFF "); // Add spacing for consistent width
      }
      
      if (i < 3) {
        Serial.print("\t"); // Tab for clean spacing
      }
    }
    Serial.println(); // Final newline
    Serial.println("------------------------------------"); // Separator for readability
  }
}