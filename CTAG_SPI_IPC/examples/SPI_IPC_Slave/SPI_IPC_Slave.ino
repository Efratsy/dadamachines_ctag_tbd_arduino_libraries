/**
 * @file Slave_IPC_Demo_ESP32.ino
 * @brief A demonstration sketch for the CTAG_SPI_IPC library in Slave mode on an ESP32.
 *
 * This sketch configures an ESP32 as an SPI slave device that listens for packets
 * from an SPI master. It uses a callback function to process incoming data
 * asynchronously as soon as it arrives.
 */
#include <CTAG_SPI_IPC.h>
#include "pins_arduino.h" // Required to use standardized pin names

// --- Pin Definitions ---
// Use the standardized pin names from your ESP32's "pins_arduino.h" file.
// Note the crossover wiring required for SPI communication.
#define PIN_MISO PIN_SPI0_MISO // Connects to Master's MOSI
#define PIN_MOSI PIN_SPI0_MOSI // Connects to Master's MISO
#define PIN_SCK  PIN_SPI0_SCK
#define PIN_CS   PIN_SPI0_SS

/**
 * @brief Callback function that is automatically invoked when a valid SPI packet is received.
 * @note This function is called from an interrupt context. Keep the code inside short
 * and fast. Avoid using delay() or other long-running operations here.
 * @param data A pointer to the received payload data.
 * @param len The length of the payload data in bytes.
 */
void onPacketReceived(const uint8_t* data, size_t len) {
  Serial.print(">> Packet Received! Length=");
  Serial.print(len);
  Serial.print(", Content: \"");
  for (size_t i = 0; i < len; i++) {
    Serial.print((char)data[i]);
  }
  Serial.println("\"");
}

/**
 * @brief Runs once at startup to initialize the SPI Slave.
 */
void setup() {
  Serial.begin(115200);
  while(!Serial); // Wait for the serial port to connect
  Serial.println("\n--- SPI Slave IPC Demo ---");

  // Calling SPI.begin() is not necessary for the ESP32 slave mode,
  // as the library passes the pins directly to the underlying ESP-IDF driver.

  // Initialize the IPC library in slave mode, providing all necessary pins
  // and the function to call when a packet arrives.
  if (CTAG_SPI_IPC::beginSlave(PIN_SCK, PIN_MISO, PIN_MOSI, PIN_CS, onPacketReceived)) {
    Serial.println("Slave ready and waiting for packets...");
  } else {
    Serial.println("Slave initialization FAILED!");
  }
}

/**
 * @brief Runs continuously after setup.
 */
void loop() {
  // Nothing to do here. The magic happens in the background via interrupts.
  // A small delay can be used to prevent the watchdog timer from triggering
  // in an otherwise empty loop on some platforms.
  delay(100);
}