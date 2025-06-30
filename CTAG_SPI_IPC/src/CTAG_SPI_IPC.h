/**
 * @file CTAG_SPI_IPC.h
 * @brief A simple, packet-based SPI library for inter-processor communication.
 *
 * This library provides a master/slave communication protocol over SPI,
 * ensuring data integrity with a custom packet structure including magic bytes,
 * length, and a CRC8 checksum. The master implementation is platform-independent,
 * while the slave implementation is currently specific to the ESP32.
 */
#pragma once
#ifndef CTAG_SPI_IPC_H
#define CTAG_SPI_IPC_H

#include <Arduino.h>
#include <SPI.h>

namespace CTAG_SPI_IPC {

/**
 * @brief Callback function type for the slave mode.
 * @note On ESP32, this function is called from an interrupt service routine (ISR).
 * Keep the code within the callback short and efficient.
 */
using Callback = void(*)(const uint8_t* data, size_t len);

// --- Master Functions (Platform-Independent) ---

/**
 * @brief Initializes the module as a platform-independent SPI Master.
 * @param csPin The Chip Select (CS) pin.
 * @param spi A reference to the SPIClass object to use (default: SPI).
 * @param speed The desired SPI clock speed in Hz (default: 8MHz).
 * @param mode The SPI mode (default: SPI_MODE3).
 * @return True on success.
 */
bool beginMaster(uint8_t csPin,
                 SPIClass& spi = SPI,
                 uint32_t speed = 8000000,
                 uint8_t mode = SPI_MODE3);

/**
 * @brief Sends a data packet as the SPI Master.
 * @note The maximum payload size is 61 bytes due to the packet overhead.
 * @param data Pointer to the data buffer to send.
 * @param len The length of the data in bytes.
 * @return True if the packet was sent, false on error (e.g., payload too large).
 */
bool send(const uint8_t* data, size_t len);


// --- Slave Functions (ESP32 Specific Implementation) ---

/**
 * @brief Initializes the module as an SPI Slave (ESP32 only).
 * @param sckPin The SPI Clock pin.
 * @param misoPin The SPI MISO pin (Slave Out).
 * @param mosiPin The SPI MOSI pin (Slave In).
 * @param csPin The SPI Chip Select pin.
 * @param cb Callback function that will be invoked when a new, valid packet arrives.
 * @return True on success, false otherwise.
 */
bool beginSlave(uint8_t sckPin, uint8_t misoPin, uint8_t mosiPin, uint8_t csPin, Callback cb);

/**
 * @brief Checks if a new packet is available to be read in slave mode (polling method).
 * @return True if a new packet has been received and validated.
 */
bool available();

/**
 * @brief Reads the last received packet into a user-provided buffer.
 * @param buffer Pointer to the buffer where the data will be copied.
 * @param maxLen The maximum size of the buffer.
 * @return The number of bytes received, or 0 if no new packet was available.
 */
size_t receive(uint8_t* buffer, size_t maxLen);

} // namespace CTAG_SPI_IPC

#endif // CTAG_SPI_IPC_H