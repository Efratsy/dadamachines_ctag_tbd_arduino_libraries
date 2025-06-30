/**
 * @file CTAG_LED.h
 * @brief Header file for a simplified NeoPixel LED wrapper library.
 *
 * This library acts as a straightforward wrapper around the Adafruit_NeoPixel
 * library to simplify common operations like setting pixels and brightness.
 */
#pragma once
#ifndef CTAG_LED_H
#define CTAG_LED_H

#include <Adafruit_NeoPixel.h>

/**
 * @class CTAG_LED
 * @brief Provides a simplified interface for controlling a strip of NeoPixel LEDs.
 */
class CTAG_LED {
public:
    /**
     * @brief Constructs a new CTAG_LED object.
     * @param numPixels The number of pixels in the strip.
     * @param pin The GPIO pin the NeoPixel data line is connected to.
     * @param type The pixel type and speed (e.g., NEO_GRB + NEO_KHZ800).
     */
    CTAG_LED(uint16_t numPixels, int16_t pin, neoPixelType type = NEO_GRB + NEO_KHZ800);

    /**
     * @brief Initializes the NeoPixel library and strip.
     * @note Must be called in the setup() function.
     */
    void begin();

    /**
     * @brief Sends the updated color data to the physical LEDs.
     * @note This is required to see any changes made with setPixelColor().
     */
    void show();

    /**
     * @brief Clears the buffer and turns off all LEDs.
     */
    void clear();

    /**
     * @brief Sets the color of a single pixel in the buffer.
     * @note You must call show() to update the physical LED.
     * @param n The index of the pixel to set (0-based).
     * @param r The red color value (0-255).
     * @param g The green color value (0-255).
     * @param b The blue color value (0-255).
     */
    void setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b);

    /**
     * @brief Sets the brightness for the entire strip.
     * @note This affects all subsequent calls to show().
     * @param b The brightness level (0-255).
     */
    void setBrightness(uint8_t b);

private:
    /// @brief The underlying Adafruit_NeoPixel object that this class wraps.
    Adafruit_NeoPixel _pixels;
};

#endif // CTAG_LED_H