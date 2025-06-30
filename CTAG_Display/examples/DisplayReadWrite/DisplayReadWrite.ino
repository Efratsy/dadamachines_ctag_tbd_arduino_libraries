/**
 * @file CTAG_Display_Demo.ino
 * @brief Demonstration sketch for the CTAG_Display library.
 *
 * This example showcases how to initialize the display, write text to specific
 * rows, read the entire display buffer back, and update rows continuously.
 */

// Include the custom board pin definitions to use standardized pin names.
#include "pins_arduino.h"

// Include the main library header.
#include <CTAG_Display.h>

/**
 * @brief Global instance of the display driver.
 */
CTAG_Display display;

/**
 * @brief Runs once at startup to initialize hardware and display content.
 */
void setup() {
  // Start serial communication for debugging output.
  Serial.begin(115200);
  while (!Serial) { delay(10); } // Wait for serial port to connect. Needed for native USB.

  Serial.println("\n--- CTAG_Display Demo ---");

  // Initialize the display using the standardized I2C pins from pins_arduino.h
  Serial.println("Initializing display...");
  if (!display.begin(PIN_WIRE1_SDA, PIN_WIRE1_SCL, 0x3C)) {
    Serial.println("Display init failed! Halting.");
    while (1) delay(100); // Stop execution if display fails.
  }
  Serial.println("Display initialized successfully.");

  // --- Initial Content ---
  // Clear the display and write some initial test messages.
  display.clear();
  display.writeRow(0, "CTAG_Display Demo");
  display.writeRow(1, "writeRow -> row1");
  display.writeRow(2, "writeRow -> row2");
  delay(2000);

  // Read the content of the internal buffer and print it to the Serial Monitor.
  Serial.println("\n--- Current display buffer content: ---");
  Serial.println(display.readDisplay());
  Serial.println("------------------------------------");
}

/**
 * @brief Runs continuously after setup() to update the display content.
 */
void loop() {
  static uint8_t counter = 0;
  char textBuffer[22]; // A buffer to format text into.

  // Continuously update row 4 with a loop counter.
  snprintf(textBuffer, sizeof(textBuffer), "Loop count: %3u", counter++);
  display.writeRow(4, textBuffer);

  // Simulate a timestamp or other changing data on row 5.
  snprintf(textBuffer, sizeof(textBuffer), "Millis: %8lu", millis());
  display.writeRow(5, textBuffer);

  delay(1000); // Update once per second.
}