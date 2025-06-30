/**
 * @file CTAG_LED.cpp
 * @brief Implementation file for the CTAG_LED class.
 */
#include "CTAG_LED.h"

CTAG_LED::CTAG_LED(uint16_t numPixels, int16_t pin, neoPixelType type)
    : _pixels(numPixels, pin, type) {}

void CTAG_LED::begin() {
    _pixels.begin();
    // Send the initial state (all lights off) to the pixels to ensure
    // a clean startup state.
    _pixels.show();
}

void CTAG_LED::show() {
    _pixels.show();
}

void CTAG_LED::clear() {
    _pixels.clear();
    // clear() only sets the buffer to 0; show() is required to turn off the physical LEDs.
    _pixels.show(); 
}

void CTAG_LED::setPixelColor(uint16_t n, uint8_t r, uint8_t g, uint8_t b) {
    _pixels.setPixelColor(n, r, g, b);
}

void CTAG_LED::setBrightness(uint8_t b) {
    _pixels.setBrightness(b);
}