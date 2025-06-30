/**
 * @file CTAG_Display.cpp
 * @brief Implementation file for the CTAG_Display class.
 */
#include "CTAG_Display.h"

// Constructor: Initializes the display pointer to null.
CTAG_Display::CTAG_Display() : _disp(nullptr) {
}

// Destructor: Frees the memory allocated for the display object.
CTAG_Display::~CTAG_Display() {
  delete _disp;
}

bool CTAG_Display::begin(uint8_t sda, uint8_t scl, uint8_t address) {
  // Configure the I2C1 bus for the display.
  Wire1.setSDA(sda);
  Wire1.setSCL(scl);
  Wire1.setClock(400000); // Use 400kHz fast I2C
  Wire1.begin();

  // Create the display object on the heap.
  _disp = new Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire1, OLED_RESET);
  if (!_disp) {
    return false; // Memory allocation failed.
  }
  
  // Initialize the display driver.
  if (!_disp->begin(address, true)) { // `true` = reset display
    return false; // Initialization failed.
  }

  // Set up initial display state.
  clear();
  display();
  _disp->setRotation(2); // Rotate 180 degrees to match common hardware layouts.
  _disp->setTextSize(1);
  _disp->setTextColor(SH110X_WHITE);
  
  return true;
}

void CTAG_Display::clear() {
  // Safety check to ensure the display object exists.
  if (!_disp) return;
  _disp->clearDisplay();

  // Clear the internal text buffer.
  for (auto &row : _buffer) {
    row[0] = '\0';
  }
}

void CTAG_Display::display() {
  // Safety check.
  if (!_disp) return;

  // Redraw the entire display from the buffer content.
  _disp->clearDisplay();
  for (uint8_t r = 0; r < 8; ++r) {
    _disp->setCursor(0, r * 8);
    _disp->print(_buffer[r]);
  }
  _disp->display();
}

void CTAG_Display::writeRow(uint8_t row, const char* text) {
  // Check for valid row number and display object.
  if (row >= 8 || !_disp) return;
  
  // Update the buffer with the new text.
  strncpy(_buffer[row], text, 21);
  _buffer[row][21] = '\0'; // Ensure null-termination for safety.
  
  // Redraw the entire display to show the change immediately.
  display();
}

String CTAG_Display::readDisplay() const {
  String out;
  // Concatenate all rows from the buffer into a single String.
  for (uint8_t r = 0; r < 8; ++r) {
    out += _buffer[r];
    if (r < 7) out += '\n'; // Add newline characters between rows.
  }
  return out;
}