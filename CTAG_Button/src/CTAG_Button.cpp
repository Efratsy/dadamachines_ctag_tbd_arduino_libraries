/**
 * @file CTAG_Button.cpp
 * @brief Implementation of the CTAG_Button class.
 */
#include "CTAG_Button.h"

CTAG_Button::CTAG_Button(uint8_t pin, unsigned long debounceDelay, unsigned long longPressDelay)
    : _pin(pin), 
      _debounceDelay(debounceDelay), 
      _longPressDelay(longPressDelay),
      _lastDebounceTime(0),
      _lastReading(HIGH), 
      _buttonState(HIGH), 
      _pressStartTime(0),
      _wasShortPressedFlag(false),
      _wasLongPressedFlag(false),
      _longPressTriggered(false) {}

void CTAG_Button::begin(uint8_t mode) {
    pinMode(_pin, mode);
    // Initialize states with the current hardware reading
    _lastReading = digitalRead(_pin);
    _buttonState = _lastReading;
}

void CTAG_Button::update() {
    // Reset the one-shot event flags at the beginning of each update cycle.
    _wasShortPressedFlag = false;
    _wasLongPressedFlag = false;

    // --- Debounce Logic ---
    bool currentReading = digitalRead(_pin);

    // If the switch changed, reset the debounce timer.
    if (currentReading != _lastReading) {
        _lastDebounceTime = millis();
    }

    // Check if the signal has been stable for longer than the debounce delay.
    if ((millis() - _lastDebounceTime) > _debounceDelay) {
        // If the stable reading is different from the button's known state, a state change has occurred.
        if (currentReading != _buttonState) {
            _buttonState = currentReading;

            // A debounced PRESS event has just occurred.
            if (_buttonState == LOW) {
                _pressStartTime = millis();
                _longPressTriggered = false; // Reset long press tracker.
            } 
            // A debounced RELEASE event has just occurred.
            else { 
                // If a long press wasn't triggered during the press, it must have been a short press.
                if (!_longPressTriggered) {
                    _wasShortPressedFlag = true;
                }
            }
        }
    }
    _lastReading = currentReading;

    // --- Long Press Logic ---
    // Check if the button is currently held down AND the long press event has not yet fired for this press.
    if (_buttonState == LOW && !_longPressTriggered) {
        if (millis() - _pressStartTime > _longPressDelay) {
            _wasLongPressedFlag = true;
            _longPressTriggered = true; // Set the flag to prevent this event from firing multiple times per press.
        }
    }
}

bool CTAG_Button::isPressed() {
    // Return the current, debounced state of the button.
    return (_buttonState == LOW);
}

bool CTAG_Button::wasShortPressed() {
    // Return the one-shot flag for the short press event.
    return _wasShortPressedFlag;
}

bool CTAG_Button::wasLongPressed() {
    // Return the one-shot flag for the long press event.
    return _wasLongPressedFlag;
}