/**
 * @file CTAG_Audio_Demo.ino
 * @brief Demonstration sketch for the CTAG Audio Library.
 *
 * This example shows how to:
 * 1. Initialize the audio codec.
 * 2. Start the audio engine.
 * 3. Create a sine wave oscillator (VCO) and set it as the audio source.
 * 4. Control the oscillator's frequency in the main loop to create a siren effect.
 */

// Include the custom board pin definitions. This is crucial!
#include "pins_arduino.h"

// Include the main library header.
#include "CTAG_Audio.h"

// --- Global Objects ---

/**
 * @brief Global instance of the audio codec driver.
 */
CTAG_AudioCodec codec;

/**
 * @brief Global instance of our sine wave oscillator "plugin".
 */
CTAG_VCO_Sine myVCO;


/**
 * @brief Runs once at startup to initialize the hardware and software.
 */
void setup() {
  // Start serial communication for debugging output.
  Serial.begin(115200);
  delay(1000); // Wait for the serial monitor to connect.
  Serial.println("\n--- CTAG Audio Framework Demo ---");

  // 1. Initialize the audio codec via I2C.
  Serial.println("Initializing Audio Codec...");
  // Use the standardized pin names from our "pins_arduino.h".
  if (!codec.begin(PIN_WIRE1_SDA, PIN_WIRE1_SCL)) {
    Serial.println("Codec initialization failed! Halting.");
    while(1); // Stop execution if the codec can't be found.
  }
  codec.setHeadphoneVolume(70); // Set a comfortable headphone volume.
  Serial.println("Codec initialized successfully.");

  // 2. Start the audio engine and connect our VCO as the sound source.
  Serial.println("Starting Audio Engine...");
  CTAG_AudioEngine::setSource(&myVCO);
  CTAG_AudioEngine::begin(); // Starts the I2S communication.
  Serial.println("Audio Engine running.");

  // 3. Configure the initial state of our oscillator.
  myVCO.setFrequency(220.0f); // Set initial frequency to A2 (220 Hz).
  myVCO.setAmplitude(0.5f);   // Set amplitude to 50% to avoid clipping.

  Serial.println("Setup complete. Sound should now be audible.");
}


/**
 * @brief Runs continuously after setup().
 * The audio is generated in a background task, so this loop is free for controls.
 */
void loop() {
  // We can now control the sound parameters in the main loop.
  
  // Example: Create a "siren" effect by modulating the frequency with a sine wave.
  // The frequency will oscillate smoothly between 0 Hz and 440 Hz.
  float frequency = 220.0f + 220.0f * sin(millis() * 0.001f);
  myVCO.setFrequency(frequency);
  
  // Print the current frequency to the serial monitor for feedback.
  Serial.printf("Frequency: %.2f Hz\n", frequency);

  // A short delay to prevent flooding the serial monitor.
  delay(20);
}