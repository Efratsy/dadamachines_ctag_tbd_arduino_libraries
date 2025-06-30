/**
 * @file CTAG_Display.h
 * @brief Header file for a text-based display wrapper library.
 *
 * This library provides a simple, row-based text interface for SH1106G-based
 * OLED displays, wrapping the more complex Adafruit_GFX and Adafruit_SH110X libraries.
 */
#pragma once
#ifndef CTAG_DISPLAY_H
#define CTAG_DISPLAY_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

// --- Fallback Definitions ---
// These can be overridden in your sketch before including this header.
#ifndef SCREEN_WIDTH
#define SCREEN_WIDTH 128  ///< Display width in pixels.
#endif
#ifndef SCREEN_HEIGHT
#define SCREEN_HEIGHT 64  ///< Display height in pixels.
#endif
#ifndef OLED_RESET
#define OLED_RESET    10  ///< Reset pin (-1 if sharing the Arduino reset pin).
#endif

/**
 * @class CTAG_Display
 * @brief Provides a simple, 8-row text interface for an I2C OLED display.
 */
class CTAG_Display {
public:
  /**
   * @brief Constructs a new CTAG_Display object.
   */
  CTAG_Display();

  /**
   * @brief Destroys the CTAG_Display object, freeing allocated memory.
   */
  ~CTAG_Display();

  /**
   * @brief Initializes the display using the I2C interface.
   * @note On the ESP32, this implementation uses the `Wire1` peripheral.
   * @param sdaPin The GPIO pin for I2C SDA.
   * @param sclPin The GPIO pin for I2C SCL.
   * @param i2cAddr The I2C address of the display (e.g., 0x3C).
   * @return True on successful initialization, false otherwise.
   */
  bool begin(uint8_t sdaPin, uint8_t sclPin, uint8_t i2cAddr);

  /**
   * @brief Clears the internal text buffer and the physical display.
   */
  void clear();

  /**
   * @brief Writes the current content of the text buffer to the display.
   */
  void display();

  /**
   * @brief Writes a line of text to the buffer and immediately updates the display.
   * @param row The row number to write to (0-7).
   * @param text The C-style string to write (max 21 characters).
   */
  void writeRow(uint8_t row, const char* text);

  /**
   * @brief Reads the entire content of the internal text buffer.
   * @return A String object containing the content of all rows, separated by newlines.
   */
  String readDisplay() const;

private:
  ///< Pointer to the underlying Adafruit display object.
  Adafruit_SH1106G* _disp;

  ///< Internal buffer for the text content (8 rows of 21 chars + null terminator).
  char _buffer[8][22];
};

#endif // CTAG_DISPLAY_H