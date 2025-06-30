#include "CTAG_Audio.h"

/**
 * @file CTAG_Audio.cpp
 * @brief Implementation file for the CTAG audio library for ESP32.
 * @note This implementation assumes that the I2S pin macros
 * (e.g., PIN_I2S_MCLK, PIN_I2S_BCLK) are defined in the user's
 * project scope, typically via a board-specific "pins_arduino.h".
 */

// --- Constants ---
#define SAMPLE_RATE     (44100)
#define BITS_PER_SAMPLE (I2S_BITS_PER_SAMPLE_16BIT)


// =========================================================================
// === Layer 1: CTAG_AudioCodec Implementation                           ===
// =========================================================================

/**
 * @class CTAG_AudioCodec
 * @brief Low-level driver to control the TLV320AIC3254 audio codec via I2C.
 */

CTAG_AudioCodec::CTAG_AudioCodec(uint8_t i2c_addr) : _i2c_addr(i2c_addr) {}

void CTAG_AudioCodec::_write_register(uint8_t page, uint8_t reg, uint8_t value) {
    Wire.beginTransmission(_i2c_addr);
    Wire.write(0x00); // Address of the Page Select Register
    Wire.write(page); // Select the desired page
    Wire.endTransmission();
    
    Wire.beginTransmission(_i2c_addr);
    Wire.write(reg);   // The register address on the selected page
    Wire.write(value); // The value to be written
    Wire.endTransmission();
}

void CTAG_AudioCodec::_configure_tlv320aic3254() {
    // Default initialization sequence for the codec.
    _write_register(0, 1, 0x01); delay(10);
    _write_register(1, 1, 0x08); _write_register(1, 2, 0x01); _write_register(1, 10, 0x08);
    _write_register(0, 27, 0x10); _write_register(0, 28, 0x00); _write_register(0, 4, 0x00);
    _write_register(0, 5, 0x00); _write_register(0, 13, 0x00); _write_register(0, 14, 0x80);
    _write_register(0, 20, 0x80); _write_register(0, 11, 0x81); _write_register(0, 12, 0x82);
    _write_register(0, 18, 0x81); _write_register(0, 19, 0x82); _write_register(1, 14, 0x08);
    _write_register(1, 15, 0x08); _write_register(1, 12, 0x08); _write_register(1, 13, 0x08);
    _write_register(0, 64, 0x00); _write_register(0, 65, 0x00); _write_register(0, 66, 0x00);
    _write_register(0, 63, 0xD4); _write_register(1, 9, 0x3C); _write_register(1, 16, 0x00);
    _write_register(1, 17, 0x00); _write_register(1, 18, 0x06); _write_register(1, 19, 0x06);
    _write_register(1, 52, 0x40); _write_register(1, 55, 0x40); _write_register(1, 54, 0x40);
    _write_register(1, 57, 0x40); _write_register(1, 59, 0x00); _write_register(1, 60, 0x00);
    _write_register(0, 81, 0xC0); _write_register(0, 82, 0x00);
    delay(10);
}

bool CTAG_AudioCodec::begin(int sdaPin, int sclPin) {
    bool success = Wire.begin(sdaPin, sclPin);
    if (success) {
        _configure_tlv320aic3254();
    }
    return success;
}

void CTAG_AudioCodec::setHeadphoneVolume(uint8_t volume) {
    volume = constrain(volume, 0, 100);
    int8_t reg_val = map(volume, 0, 100, 0x3B, 0x14);
    _write_register(1, 16, (uint8_t)reg_val);
    _write_register(1, 17, (uint8_t)reg_val);
}

void CTAG_AudioCodec::setLineOutVolume(uint8_t volume) {
    volume = constrain(volume, 0, 100);
    int8_t reg_val = map(volume, 0, 100, 0x3A, 0x1D);
    _write_register(1, 18, (uint8_t)reg_val);
    _write_register(1, 19, (uint8_t)reg_val);
}


// =========================================================================
// === Layer 2: Audio Engine Implementation                              ===
// =========================================================================

namespace CTAG_AudioEngine {
    static CTAG_AudioSource* currentSource = nullptr;
    static i2s_port_t _i2s_port;
    
    void audio_task(void* params) {
        const int buffer_samples = 256;
        int16_t i2s_buffer[buffer_samples * 2];
        size_t bytes_written = 0;

        while(true) {
            for(int i = 0; i < buffer_samples * 2; i+=2) {
                if (currentSource != nullptr) {
                    int16_t sample = currentSource->getNextSample();
                    i2s_buffer[i] = sample;
                    i2s_buffer[i+1] = sample;
                } else {
                    i2s_buffer[i] = 0;
                    i2s_buffer[i+1] = 0;
                }
            }
            i2s_write(_i2s_port, i2s_buffer, sizeof(i2s_buffer), &bytes_written, portMAX_DELAY);
        }
    }
    
    bool begin(i2s_port_t i2s_port) {
        _i2s_port = i2s_port;

        i2s_config_t i2s_config = {
            .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
            .sample_rate = SAMPLE_RATE,
            .bits_per_sample = BITS_PER_SAMPLE,
            .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
            .communication_format = I2S_COMM_FORMAT_STAND_I2S,
            .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
            .dma_buf_count = 8,
            .dma_buf_len = 256,
            .use_apll = true,
            .tx_desc_auto_clear = true
        };
        i2s_driver_install(_i2s_port, &i2s_config, 0, NULL);

        i2s_pin_config_t pin_config = {
            .mck_io_num = PIN_I2S_MCLK,
            .bck_io_num = PIN_I2S_BCLK,
            .ws_io_num = PIN_I2S_WS,
            .data_out_num = PIN_I2S_SDOUT,
            .data_in_num = I2S_PIN_NO_CHANGE
        };
        i2s_set_pin(_i2s_port, &pin_config);
        i2s_set_clk(_i2s_port, SAMPLE_RATE, BITS_PER_SAMPLE, I2S_CHANNEL_STEREO);

        xTaskCreatePinnedToCore(audio_task, "AudioTask", 4096, NULL, 5, NULL, 0);
        
        return true;
    }

    void setSource(CTAG_AudioSource* source) {
        currentSource = source;
    }
}


// =========================================================================
// === Layer 3: CTAG_VCO_Sine Implementation                             ===
// =========================================================================

CTAG_VCO_Sine::CTAG_VCO_Sine(float sampleRate)
    : _sampleRate(sampleRate), _frequency(440.0f), _amplitude(0.5f), _phase(0.0f) {
    setFrequency(_frequency);
}

void CTAG_VCO_Sine::setFrequency(float freq) {
    _frequency = freq;
    _phase_increment = (2.0f * M_PI * _frequency) / _sampleRate;
}

void CTAG_VCO_Sine::setAmplitude(float amp) {
    _amplitude = constrain(amp, 0.0f, 1.0f);
}

int16_t CTAG_VCO_Sine::getNextSample() {
    int16_t sample = (int16_t)(_amplitude * 32767.0f * sin(_phase));
    _phase += _phase_increment;
    if (_phase >= 2.0f * M_PI) {
        _phase -= 2.0f * M_PI;
    }
    return sample;
}