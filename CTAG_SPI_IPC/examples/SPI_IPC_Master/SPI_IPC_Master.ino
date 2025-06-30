/**
 * @file Master_IPC_Demo.ino
 * @brief A demonstration sketch for the CTAG_SPI_IPC library in Master mode.
 *
 * This sketch configures an RP2040 (or similar board) as an SPI master and
 * periodically sends a test message to a connected SPI slave device.
 */
#include <CTAG_SPI_IPC.h>
#include "pins_arduino.h" // Required to use standardized pin names

// --- Pin Definitions ---
// Use the standardized pin names from your board's "pins_arduino.h" file.
#define PIN_MISO PIN_SPI0_MISO
#define PIN_MOSI PIN_SPI0_MOSI
#define PIN_SCK  PIN_SPI0_SCK
#define PIN_CS   PIN_SPI0_SS

/**
 * @brief Runs once at startup to initialize the SPI Master.
 */
void setup() {
  Serial.begin(115200);
  while(!Serial); // Wait for the serial port to connect
  Serial.println("\n--- SPI Master IPC Demo ---");

  // Configure the SPI pins for the RP2040.
  SPI.setMISO(PIN_MISO);
  SPI.setMOSI(PIN_MOSI);
  SPI.setSCK(PIN_SCK);
  
  // Activate the SPI hardware in master mode.
  // `false` means the CS pin will be controlled manually by the library.
  SPI.begin(false);

  // Initialize the IPC library in master mode.
  // We use the SPI peripheral, a 1MHz clock speed, and SPI_MODE0 for compatibility.
  if (CTAG_SPI_IPC::beginMaster(PIN_CS, SPI, 1000000, SPI_MODE0)) {
    Serial.println("Master ready.");
  } else {
    Serial.println("Master initialization failed!");
  }
}

/**
 * @brief Runs continuously after setup to send data periodically.
 */
void loop() {
  static uint8_t counter = 0;
  char message[32];

  // Create a new test message for each iteration.
  snprintf(message, sizeof(message), "Test packet No. %d", counter++);
  
  // Send the message via the IPC library.
  Serial.printf("Sending: \"%s\"...", message);
  bool ok = CTAG_SPI_IPC::send((const uint8_t*)message, strlen(message));
  Serial.println(ok ? " OK" : " FAILED");

  // Wait for one second before sending the next packet.
  delay(1000);
}