# dadamachines CTAG TBD Arduino Libraries

This repository contains a collection of official Arduino libraries specifically designed for the **[dadamachines CTAG TBD hardware package](https://github.com/dadamachines/dadamachines_ctag_tbd_arduino)**.

These libraries provide high-level functionality for interacting with the platform's core features, making development faster and easier.

---
## Libraries Included

This collection includes the following libraries:

* ### CTAG_Audio
  * Provides high-level functions for audio processing, managing codecs, and handling audio I/O.

* ### CTAG_Button
  * A simple interface for debouncing and reading the state of physical buttons.

* ### CTAG_Display
  * Manages the OLED display, providing functions for drawing text, shapes, and custom user interface elements.

* ### CTAG_ExtensionBoard
  * Handles communication and control for the optional hardware extension board.

* ### CTAG_LED
  * A helper library for controlling onboard LEDs, including brightness and patterns.

* ### CTAG_MIDI
  * Manages MIDI input and output, including USB-MIDI, TRS-MIDI, and internal event routing.

* ### CTAG_SPI_IPC
  * Implements Inter-Process Communication (IPC) over the SPI bus, allowing different microcontrollers on the board to communicate.

---
## Installation & Usage

### Recommended Method
These libraries are included by default when you install the main `dadamachines_ctag_tbd_arduino` hardware package using the Arduino Boards Manager. This is the recommended way to use them, as they are tightly integrated with the underlying hardware core.

### Manual Installation
For development purposes, you can clone this repository directly into your Arduino sketchbook's `libraries` folder.

```bash
git clone [https://github.com/Efratsy/dadamachines_ctag_tbd_arduino_libraries.git](https://github.com/Efratsy/dadamachines_ctag_tbd_arduino_libraries.git)