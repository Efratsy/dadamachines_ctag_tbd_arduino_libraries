/**
 * @file CTAG_SPI_IPC.cpp
 * @brief Implementation file for the CTAG_SPI_IPC library.
 */
#include "CTAG_SPI_IPC.h"

namespace CTAG_SPI_IPC {

// --- Module-level static variables ---
static SPIClass* _spi = nullptr;        // Pointer to the SPI peripheral
static uint8_t   _csPin = 255;          // Chip Select pin for master mode
static SPISettings _settings;           // SPI settings for master mode

/**
 * @brief Calculates the CRC8 checksum for a given data buffer.
 * @param data Pointer to the data.
 * @param len Length of the data.
 * @return The calculated 8-bit CRC.
 */
static uint8_t calcCRC(const uint8_t* data, size_t len) {
  uint8_t crc = 0;
  while (len--) {
    crc ^= *data++;
    for (int i = 0; i < 8; ++i)
      crc = (crc & 0x80) ? (crc << 1) ^ 0x07 : (crc << 1);
  }
  return crc;
}


// --- Master Implementation ---

bool beginMaster(uint8_t csPin, SPIClass& spi, uint32_t speed, uint8_t mode) {
  _spi = &spi;
  _csPin = csPin;
  pinMode(_csPin, OUTPUT);
  digitalWrite(_csPin, HIGH);
  _settings = SPISettings(speed, MSBFIRST, mode);
  return true;
}

bool send(const uint8_t* data, size_t len) {
  // Check for initialization and valid length
  if (!_spi || len > 61) return false;

  // Construct the frame: [Magic1, Magic2, Len, Payload..., CRC]
  uint8_t frame[64];
  frame[0] = 0xCA;
  frame[1] = 0xFE;
  frame[2] = (uint8_t)len;
  memcpy(&frame[3], data, len);
  frame[3 + len] = calcCRC(&frame[3], len);
  size_t frameLen = 3 + len + 1;

  // Perform SPI transaction
  _spi->beginTransaction(_settings);
  digitalWrite(_csPin, LOW);
  _spi->transfer(frame, frameLen);
  digitalWrite(_csPin, HIGH);
  _spi->endTransaction();

  return true;
}


// --- Slave Implementation ---

#ifdef ESP32
#include "driver/spi_slave.h"

#define BUFFER_SIZE 64

// Static variables for the ESP32 slave implementation
static spi_slave_transaction_t _trans;
WORD_ALIGNED_ATTR static uint8_t _rxBuf[BUFFER_SIZE];
WORD_ALIGNED_ATTR static uint8_t _txBuf[BUFFER_SIZE]; // Not used for sending yet, but required by API
static Callback _slaveCb = nullptr;
volatile static bool _rxReady = false; // Flag to indicate a complete, valid packet has arrived

/**
 * @brief ISR callback triggered after an SPI slave transaction is complete.
 * @note This runs in an interrupt context. It validates the received packet
 * and calls the user-provided callback if the packet is valid.
 */
static void IRAM_ATTR post_trans_cb(spi_slave_transaction_t *trans) {
  // Check for magic bytes to identify a valid start of frame
  if (_rxBuf[0] == 0xCA && _rxBuf[1] == 0xFE) {
    uint8_t len = _rxBuf[2];
    // Validate length and CRC checksum
    if (len <= 61 && calcCRC(&_rxBuf[3], len) == _rxBuf[3 + len]) {
      // If a user callback is registered, call it with the payload
      if (_slaveCb) {
        _slaveCb(&_rxBuf[3], len);
      }
      // Set the flag for the polling method (available()/receive())
      _rxReady = true;
    }
  }
  // Re-queue the transaction to be ready for the next one
  spi_slave_queue_trans(SPI3_HOST, &_trans, portMAX_DELAY);
}

bool beginSlave(uint8_t sckPin, uint8_t misoPin, uint8_t mosiPin, uint8_t csPin, Callback cb) {
  _slaveCb = cb;
  
  // Configure the SPI bus
  spi_bus_config_t buscfg = {
      .mosi_io_num = mosiPin,
      .miso_io_num = misoPin,
      .sclk_io_num = sckPin,
      .quadwp_io_num = -1,
      .quadhd_io_num = -1,
      .max_transfer_sz = BUFFER_SIZE
  };

  // Configure the SPI slave interface
  spi_slave_interface_config_t slvcfg = {
      .spics_io_num = csPin,
      .flags = 0,
      .queue_size = 1,
      .mode = 0, // Mode 0 for ESP32 slave. Master must match.
      .post_trans_cb = post_trans_cb
  };

  // Initialize the SPI slave driver
  esp_err_t ret = spi_slave_initialize(SPI3_HOST, &buscfg, &slvcfg, SPI_DMA_CH_AUTO);
  if (ret != ESP_OK) {
      return false;
  }

  // Set up the transaction buffer details
  memset(&_trans, 0, sizeof(_trans));
  _trans.length = BUFFER_SIZE * 8; // Length in bits
  _trans.rx_buffer = _rxBuf;
  _trans.tx_buffer = _txBuf;

  // Queue the first transaction
  spi_slave_queue_trans(SPI3_HOST, &_trans, portMAX_DELAY);

  return true;
}

bool available() { return _rxReady; }

size_t receive(uint8_t* buffer, size_t maxLen) {
  if (!_rxReady) return 0;
  
  size_t len = _rxBuf[2];
  if (maxLen >= len) {
    memcpy(buffer, &_rxBuf[3], len);
  } else { 
    len = 0; // Buffer too small, return 0
  }
  
  _rxReady = false; // Reset the flag after reading
  return len;
}

#else // Placeholder for other architectures
bool beginSlave(uint8_t sckPin, uint8_t misoPin, uint8_t mosiPin, uint8_t csPin, Callback cb) { return false; }
bool available() { return false; }
size_t receive(uint8_t* buffer, size_t maxLen) { return 0; }
#endif

} // namespace CTAG_SPI_IPC