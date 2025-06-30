/**
 * @file CTAG_ExtensionBoard.h
 * @brief Header file for the CTAG Extension Board library.
 *
 * This library provides an interface to control and read data from a CTAG extension
 * board via the I2C protocol. It handles LEDs, buttons, potentiometers, and an encoder.
 */
#ifndef CTAG_EXTENSIONBOARD_H
#define CTAG_EXTENSIONBOARD_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>

/**
 * @brief Data structure for the UI state received from the extension board via I2C.
 * @note This struct must exactly match the one on the slave device to ensure correct data parsing.
 */
typedef struct {
  uint16_t pot_adc_values[8];      ///< RAW ADC values of the potentiometers.
  uint16_t pot_positions[4];       ///< Absolute position of the potentiometers (0-65535).
  uint8_t  pot_states[4];          ///< State of the potentiometers (e.g., movement flags).
  uint16_t d_btns;                 ///< Digital buttons D1-D16 (short press state, bitmask).
  uint16_t d_btns_long_press;      ///< Digital buttons D1-D16 (long press state, bitmask).
  uint8_t  f_btns;                 ///< Function buttons F1-F4 (short press state, bitmask).
  uint8_t  f_btns_long_press;      ///< Function buttons F1-F4 (long press state, bitmask).
  uint8_t  encoder_state;          ///< State of the encoder (button press, direction flags).
  uint32_t encoder_counter;        ///< 32-bit incremental counter of the encoder.
  uint8_t  encoder_absolute_pos;   ///< Absolute position of the encoder.
  uint8_t  encoder_speed;          ///< Rotational speed of the encoder.
  uint32_t systicks;               ///< Timestamp from the slave device.
} ui_data_t;


/**
 * @class CTAG_ExtensionBoard
 * @brief Main class to interact with the CTAG Extension Board.
 */
class CTAG_ExtensionBoard {
public:
  /**
   * @brief Constructs a new CTAG_ExtensionBoard object.
   * @param num_leds Number of NeoPixel LEDs on the board.
   * @param led_pin The GPIO pin connected to the NeoPixel data line.
   * @param i2c_addr The I2C address of the board (default: 0x42).
   * @param wire A pointer to the TwoWire object to use (default: &Wire1 for RP2040).
   */
  CTAG_ExtensionBoard(uint8_t num_leds, uint8_t led_pin, uint8_t i2c_addr = 0x42, TwoWire* wire = &Wire1);

  /**
   * @brief Initializes the I2C connection and the LEDs.
   * @note Must be called in the setup() function.
   * @param sdaPin The pin for the I2C data line (SDA).
   * @param sclPin The pin for the I2C clock line (SCL).
   */
  void begin(int8_t sdaPin, int8_t sclPin);

  /**
   * @brief Retrieves the latest data from the extension board.
   * @note Must be called regularly in the main loop to get fresh data.
   * @return True if data was read successfully, false otherwise.
   */
  bool update();

  //------------- LEDs -------------
  /** @brief Sets the color of a single LED. Call show() to update. */
  void setLed(uint8_t index, uint8_t r, uint8_t g, uint8_t b);
  /** @brief Sets the color of all LEDs. Call show() to update. */
  void setAll(uint8_t r, uint8_t g, uint8_t b);
  /** @brief Sends the updated color data to the LEDs. */
  void show();
  /** @brief Sets the brightness of all LEDs (0-255). Call show() to update. */
  void setBrightness(uint8_t brightness);

  //------------- Digital Buttons (D1-D16) -------------
  /** @brief Checks if a digital button is pressed. @param index 0-15. */
  bool isButtonPressed(uint8_t index) const;
  /** @brief Checks if a digital button is long-pressed. @param index 0-15. */
  bool isButtonLongPressed(uint8_t index) const;

  //------------- Function Buttons (F1-F4) -------------
  /** @brief Checks if a function button is pressed. @param index 0-3. */
  bool isFButtonPressed(uint8_t index) const;
  /** @brief Checks if a function button is long-pressed. @param index 0-3. */
  bool isFButtonLongPressed(uint8_t index) const;

  //----------- Potentiometers ----------
  /** @brief Gets the absolute position of a potentiometer. @param index 0-3. @return 0-65535. */
  uint16_t getPot(uint8_t index) const;
  /** @brief Gets the raw ADC value of a potentiometer. @param index 0-7. */
  uint16_t getPotRaw(uint8_t index) const;
  /** @brief Gets the state of a potentiometer. @param index 0-3. */
  uint8_t getPotState(uint8_t index) const;

  //----------- Encoder ----------
  /** @brief Gets the encoder's incremental counter. */
  uint32_t getEncoderCounter() const;
  /** @brief Gets the encoder's absolute position. */
  uint8_t getEncoderAbsolutePosition() const;
  /** @brief Gets the encoder's rotational speed. */
  uint8_t getEncoderSpeed() const;
  /** @brief Gets the raw state byte of the encoder. */
  uint8_t getEncoderState() const;
  /** @brief Checks if the encoder's button is pressed. */
  bool isEncoderPressed() const;
  /** @brief Checks if the encoder was rotated forward since the last update. */
  bool isEncoderRotatedForward() const;
  /** @brief Checks if the encoder was rotated backward since the last update. */
  bool isEncoderRotatedBackward() const;
  
  //----------- System -----------
  /** @brief Gets the system tick timestamp from the slave device. */
  uint32_t getSysticks() const;


private:
  uint8_t       _i2cAddr;   ///< I2C address of the slave device.
  TwoWire* _wire;      ///< Pointer to the I2C peripheral to use (e.g., &Wire1).
  Adafruit_NeoPixel _strip; ///< NeoPixel strip object.
  ui_data_t     _data;      ///< Local buffer to hold the data from the slave.
};

#endif // CTAG_EXTENSIONBOARD_H