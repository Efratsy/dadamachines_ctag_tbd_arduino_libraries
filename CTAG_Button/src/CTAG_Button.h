/**
 * @file CTAG_Button.h
 * @brief Header file for a versatile button handling library.
 *
 * This library provides a non-blocking way to handle button inputs,
 * including debouncing, short press, and long press detection.
 */
#pragma once
#ifndef CTAG_BUTTON_H
#define CTAG_BUTTON_H

#include <Arduino.h>

/**
 * @class CTAG_Button
 * @brief Represents a single physical button and its state.
 */
class CTAG_Button {
public:
    /**
     * @brief Constructs a new CTAG_Button object.
     * @param pin The GPIO pin the button is connected to.
     * @param debounceDelay The time in milliseconds to wait for the button signal to stabilize (default: 50).
     * @param longPressDelay The time in milliseconds a button must be held down to be considered a long press (default: 800).
     */
    CTAG_Button(uint8_t pin, unsigned long debounceDelay = 50, unsigned long longPressDelay = 800);

    /**
     * @brief Initializes the button's pin mode.
     * @note This should be called in the main setup() function.
     * @param mode The pin mode, typically INPUT or INPUT_PULLUP. INPUT_PULLUP is recommended for most button circuits.
     */
    void begin(uint8_t mode = INPUT_PULLUP);

    /**
     * @brief Updates the button's internal state machine.
     * @note This method must be called in every cycle of the main loop() to ensure proper operation.
     */
    void update();

    /**
     * @brief Checks if the button is currently being held down.
     * @return True if the button is currently pressed, false otherwise.
     */
    bool isPressed();

    /**
     * @brief Checks if the button was just released after a short press.
     * @note This is a "one-shot" event. It returns true for only one update() cycle after the event occurs.
     * @return True if a short press event has just occurred, false otherwise.
     */
    bool wasShortPressed();

    /**
     * @brief Checks if the button has been held down long enough to trigger a long press.
     * @note This is a "one-shot" event. It returns true for only one update() cycle after the event occurs.
     * @return True if a long press event has just occurred, false otherwise.
     */
    bool wasLongPressed();

private:
    // --- Configuration ---
    uint8_t _pin;                   ///< GPIO pin number of the button.
    unsigned long _debounceDelay;   ///< Debounce delay in milliseconds.
    unsigned long _longPressDelay;  ///< Long press delay in milliseconds.
    
    // --- Debounce State ---
    unsigned long _lastDebounceTime;///< Timestamp of the last time the pin's raw state changed.
    bool _lastReading;              ///< The raw, physical reading from the last update cycle.
    bool _buttonState;              ///< The stable, debounced state of the button (HIGH or LOW).

    // --- Press Logic State ---
    unsigned long _pressStartTime;  ///< Timestamp of when the button was initially pressed.
    bool _wasShortPressedFlag;      ///< One-shot flag, true for one cycle after a short press event.
    bool _wasLongPressedFlag;       ///< One-shot flag, true for one cycle after a long press event.
    bool _longPressTriggered;       ///< Internal flag to track if the long press event has already fired for the current press.
};

#endif // CTAG_BUTTON_H