/**
 * @file CTAG_ExtensionBoard.cpp
 * @brief Implementation file for the CTAG_ExtensionBoard class.
 */
#include "CTAG_ExtensionBoard.h"

CTAG_ExtensionBoard::CTAG_ExtensionBoard(uint8_t num_leds, uint8_t led_pin, uint8_t i2c_addr, TwoWire* wire)
 : _i2cAddr(i2c_addr)
 , _wire(wire)
 , _strip(num_leds, led_pin, NEO_GRB + NEO_KHZ800)
{
  // The constructor initializes members via the initializer list.
}

void CTAG_ExtensionBoard::begin(int8_t sdaPin, int8_t sclPin) {
  // Configure the I2C bus
  _wire->setSDA(sdaPin);
  _wire->setSCL(sclPin);
  _wire->setClock(400000); // 400kHz I2C clock speed
  _wire->begin();

  // Initialize the NeoPixel strip
  _strip.begin();
  _strip.setBrightness(50); // Set a default brightness
  _strip.show();  // Initialize all LEDs to an off state
}

bool CTAG_ExtensionBoard::update() {
  // Request the UI data structure from the I2C slave
  _wire->requestFrom(_i2cAddr, sizeof(ui_data_t));

  // Check if the correct number of bytes was received
  if (_wire->available() < (int)sizeof(ui_data_t)) {
    return false; // Not enough data received
  }

  // Read the data into the local buffer
  _wire->readBytes((uint8_t*)&_data, sizeof(ui_data_t));
  return true;
}

//------------- LEDs -------------
void CTAG_ExtensionBoard::setLed(uint8_t i, uint8_t r, uint8_t g, uint8_t b) {
  if (i < _strip.numPixels()) {
    _strip.setPixelColor(i, _strip.Color(r, g, b));
  }
}

void CTAG_ExtensionBoard::setAll(uint8_t r, uint8_t g, uint8_t b) {
  _strip.fill(_strip.Color(r, g, b));
}

void CTAG_ExtensionBoard::show() {
  _strip.show();
}

void CTAG_ExtensionBoard::setBrightness(uint8_t brightness) {
    _strip.setBrightness(brightness);
}

//------------- Digital Buttons (D1-D16) -------------
bool CTAG_ExtensionBoard::isButtonPressed(uint8_t i) const {
  if (i > 15) return false;
  return (_data.d_btns & (1 << i)) != 0;
}

bool CTAG_ExtensionBoard::isButtonLongPressed(uint8_t i) const {
  if (i > 15) return false;
  return (_data.d_btns_long_press & (1 << i)) != 0;
}

//------------- Function Buttons (F1-F4) -------------
bool CTAG_ExtensionBoard::isFButtonPressed(uint8_t i) const {
    if (i > 3) return false;
    return (_data.f_btns & (1 << i)) != 0;
}

bool CTAG_ExtensionBoard::isFButtonLongPressed(uint8_t i) const {
    if (i > 3) return false;
    return (_data.f_btns_long_press & (1 << i)) != 0;
}

//----------- Potentiometers ----------
uint16_t CTAG_ExtensionBoard::getPot(uint8_t i) const {
  return (i < 4) ? _data.pot_positions[i] : 0;
}

uint16_t CTAG_ExtensionBoard::getPotRaw(uint8_t i) const {
    return (i < 8) ? _data.pot_adc_values[i] : 0;
}

uint8_t CTAG_ExtensionBoard::getPotState(uint8_t i) const {
    return (i < 4) ? _data.pot_states[i] : 0;
}


//----------- Encoder ----------
uint32_t CTAG_ExtensionBoard::getEncoderCounter() const {
    return _data.encoder_counter;
}

uint8_t CTAG_ExtensionBoard::getEncoderAbsolutePosition() const {
    return _data.encoder_absolute_pos;
}

uint8_t CTAG_ExtensionBoard::getEncoderSpeed() const {
    return _data.encoder_speed;
}

uint8_t CTAG_ExtensionBoard::getEncoderState() const {
    return _data.encoder_state;
}

bool CTAG_ExtensionBoard::isEncoderPressed() const {
    // Check bit 0 for the pressed state
    return (_data.encoder_state & (1 << 0)) != 0;
}

bool CTAG_ExtensionBoard::isEncoderRotatedForward() const {
    // Check bit 1 for forward rotation event
    return (_data.encoder_state & (1 << 1)) != 0;
}

bool CTAG_ExtensionBoard::isEncoderRotatedBackward() const {
    // Check bit 2 for backward rotation event
    return (_data.encoder_state & (1 << 2)) != 0;
}

//----------- System -----------
uint32_t CTAG_ExtensionBoard::getSysticks() const {
    return _data.systicks;
}