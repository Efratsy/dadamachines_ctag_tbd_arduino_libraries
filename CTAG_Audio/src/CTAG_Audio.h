/**
 * @file CTAG_Audio.h
 * @brief Main header file for the CTAG audio library for ESP32.
 *
 * This file defines the public interface for the three main components of the library:
 * 1. CTAG_AudioCodec: A low-level driver for the audio codec hardware.
 * 2. CTAG_AudioSource: An abstract base class for creating audio-generating "plugins".
 * 3. CTAG_AudioEngine: The main engine that handles I2S streaming and processing.
 */
#pragma once
#ifndef CTAG_AUDIO_H
#define CTAG_AUDIO_H

#include <Arduino.h>
#include <Wire.h>
#include "driver/i2s.h"
#include <math.h>

// =========================================================================
// === Layer 1: The Codec Driver                                         ===
// =========================================================================

/**
 * @class CTAG_AudioCodec
 * @brief Low-level driver to control the audio codec chip via I2C.
 */
class CTAG_AudioCodec {
public:
    CTAG_AudioCodec(uint8_t i2c_addr = 0x18);

    /**
     * @brief Initializes the I2C communication and configures the codec.
     * @param sdaPin The I2C SDA pin.
     * @param sclPin The I2C SCL pin.
     * @return True on success.
     */
    bool begin(int sdaPin, int sclPin);
    
    /**
     * @brief Sets the volume for the headphone output.
     * @param volume 0 (mute) to 100 (maximum volume).
     */
    void setHeadphoneVolume(uint8_t volume);

    /**
     * @brief Sets the volume for the line-out.
     * @param volume 0 (mute) to 100 (maximum volume).
     */
    void setLineOutVolume(uint8_t volume);

private:
    void _write_register(uint8_t page, uint8_t reg, uint8_t value);
    void _configure_tlv320aic3254();
    uint8_t _i2c_addr;
};


// =========================================================================
// === Layer 2 & 3: Audio Engine & Audio Sources                         ===
// =========================================================================

/**
 * @class CTAG_AudioSource
 * @brief Abstract base class for all audio sources (oscillators, samplers, etc.).
 * @note Any audio-generating "plugin" must inherit from this class
 * and implement the getNextSample() method.
 */
class CTAG_AudioSource {
public:
    virtual ~CTAG_AudioSource() {}

    /**
     * @brief Must be implemented by the derived class to generate a single audio sample.
     * @return A 16-bit signed audio sample (-32768 to 32767).
     */
    virtual int16_t getNextSample() = 0;
};


/**
 * @namespace CTAG_AudioEngine
 * @brief The main audio engine, implemented as a static namespace.
 */
namespace CTAG_AudioEngine {
    /**
     * @brief Initializes the I2S interface and starts the audio task.
     * @param i2s_port The I2S port to use (e.g., I2S_NUM_0).
     * @return True on success.
     */
    bool begin(i2s_port_t i2s_port = I2S_NUM_0);

    /**
     * @brief Connects an audio source to the engine.
     * From this moment on, the samples from this source will be played.
     * @param source A pointer to an object that inherits from CTAG_AudioSource.
     */
    void setSource(CTAG_AudioSource* source);
}


// =========================================================================
// === Example Implementation of a "Plugin"                              ===
// =========================================================================

/**
 * @class CTAG_VCO_Sine
 * @brief A simple sine wave oscillator (VCO).
 */
class CTAG_VCO_Sine : public CTAG_AudioSource {
public:
    /**
     * @brief Constructs a new sine wave oscillator.
     * @param sampleRate The sample rate of the audio engine (e.g., 44100.0f).
     */
    CTAG_VCO_Sine(float sampleRate = 44100.0f);

    /**
     * @brief Sets the frequency of the oscillator.
     * @param freq The desired frequency in Hz.
     */
    void setFrequency(float freq);

    /**
     * @brief Sets the amplitude (volume) of the oscillator.
     * @param amp Amplitude from 0.0 (silence) to 1.0 (max).
     */
    void setAmplitude(float amp);
    
    /**
     * @brief Generates the next sample of the sine wave.
     * @return A 16-bit signed audio sample.
     */
    int16_t getNextSample() override;

private:
    float _sampleRate;
    float _frequency;
    float _amplitude;
    float _phase;
    float _phase_increment;
};

#endif // CTAG_AUDIO_H