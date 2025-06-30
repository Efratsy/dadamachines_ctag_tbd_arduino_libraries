/**
 * @file CTAG_Button_Demo.ino
 * @brief Demonstration sketch for the CTAG_Button library.
 *
 * This example shows how to use the CTAG_Button library to detect
 * short presses, long presses, and the current pressed state in a
 * non-blocking way.
 */
#include <CTAG_Button.h>

// --- Configuration ---

/**
 * @brief The GPIO pin where the button is connected.
 */
#define BUTTON_PIN PIN_UI_SWITCH

// --- Global Objects ---

/**
 * @brief Create an instance of our CTAG_Button library.
 * @note This uses the default debounce (50ms) and long press (800ms) timings.
 */
CTAG_Button button(BUTTON_PIN);

/**
 * @brief Runs once at startup to initialize the hardware and software.
 */
void setup() {
  Serial.begin(115200);
  Serial.println("\n--- CTAG_Button Demo ---");
  Serial.println("Demonstrating short and long press detection.");
  Serial.println("Hold the button for > 0.8 seconds to trigger a long press.");

  // Initialize the button. This sets the pin mode.
  button.begin();
}

/**
 * @brief Runs continuously after setup().
 */
void loop() {
  // This function must be called in every loop cycle!
  // It reads the button and updates its internal state machine.
  button.update();

  // --- Check for Events ---
  // Events are "one-shot" and only fire once when they occur.

  // Event: Short Press (pressed AND released in < 0.8s)
  if (button.wasShortPressed()) {
    Serial.println("EVENT: Short press detected!");
  }

  // Event: Long Press (held for > 0.8s)
  if (button.wasLongPressed()) {
    Serial.println("EVENT: Long press detected!");
  }


  // --- Check for State ---
  // State checks return the current condition of the button.

  // State: Currently being held down.
  // This code block runs only every 200ms while the button is held
  // to prevent flooding the serial monitor.
  static unsigned long lastPrintTime = 0;
  if (button.isPressed()) {
    if (millis() - lastPrintTime > 200) {
      Serial.println("STATE: Button is being held down...");
      lastPrintTime = millis();
    }
  }
}