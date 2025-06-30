/**
 * @file CTAG_MIDI.cpp
 * @brief Implementation file for the CTAG_MIDI library.
 *
 * This file contains the internal logic for the MIDI parser and message
 * sending functions.
 */

#include "CTAG_MIDI.h"

// Array of note names for the getNoteName() helper function.
static const char* noteNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};

// ====================================================================================
//                                  Public Methods
// ====================================================================================

CTAG_MIDI::CTAG_MIDI() {
  // Set all callback function pointers to NULL so we know if the user has assigned one.
  handleNoteOn = NULL;
  handleNoteOff = NULL;
  handleControlChange = NULL;
  handleProgramChange = NULL;
  handlePitchBend = NULL;

  // Initialize the parser state machine.
  status = 0;
  data1 = 0;
  data2 = 0;
}

void CTAG_MIDI::begin(Stream& port) {
  midiPort = &port;
}

void CTAG_MIDI::read() {
  // Process all available bytes in the serial buffer in one go.
  while (midiPort->available() > 0) {
    parse(midiPort->read());
  }
}

// --- SENDING MIDI MESSAGES ---

void CTAG_MIDI::sendNoteOn(byte note, byte velocity, byte channel) {
  byte status_byte = 0x90 | (channel - 1);
  midiPort->write(status_byte);
  midiPort->write(note);
  midiPort->write(velocity);
}

void CTAG_MIDI::sendNoteOff(byte note, byte velocity, byte channel) {
  byte status_byte = 0x80 | (channel - 1);
  midiPort->write(status_byte);
  midiPort->write(note);
  midiPort->write(velocity);
}

void CTAG_MIDI::sendControlChange(byte controller, byte value, byte channel) {
  byte status_byte = 0xB0 | (channel - 1);
  midiPort->write(status_byte);
  midiPort->write(controller);
  midiPort->write(value);
}

void CTAG_MIDI::sendProgramChange(byte program, byte channel) {
  byte status_byte = 0xC0 | (channel - 1);
  midiPort->write(status_byte);
  midiPort->write(program);
}

void CTAG_MIDI::sendPitchBend(int value, byte channel) {
  // Convert the -8192..8191 range to the MIDI 14-bit range 0..16383.
  unsigned int midiValue = value + 8192;
  // Split the 14-bit value into two 7-bit data bytes.
  byte lsb = midiValue & 0x7F;       // Least Significant Byte
  byte msb = (midiValue >> 7) & 0x7F; // Most Significant Byte

  byte status_byte = 0xE0 | (channel - 1);
  midiPort->write(status_byte);
  midiPort->write(lsb);
  midiPort->write(msb);
}

// --- ASSIGNING CALLBACKS ---

void CTAG_MIDI::setHandleNoteOn(NoteOnCallback fptr) { handleNoteOn = fptr; }
void CTAG_MIDI::setHandleNoteOff(NoteOffCallback fptr) { handleNoteOff = fptr; }
void CTAG_MIDI::setHandleControlChange(ControlChangeCallback fptr) { handleControlChange = fptr; }
void CTAG_MIDI::setHandleProgramChange(ProgramChangeCallback fptr) { handleProgramChange = fptr; }
void CTAG_MIDI::setHandlePitchBend(PitchBendCallback fptr) { handlePitchBend = fptr; }

// --- UTILITY HELPER FUNCTIONS ---

String CTAG_MIDI::getNoteName(byte noteNumber) {
  if (noteNumber > 127) return "Invalid";
  byte noteIndex = noteNumber % 12;
  int octave = (noteNumber / 12) - 1;
  String result = noteNames[noteIndex];
  result += String(octave);
  return result;
}


// ====================================================================================
//                                  Private Methods
// ====================================================================================

/**
 * @brief The internal MIDI parser. A state machine that processes the MIDI stream byte by byte.
 */
void CTAG_MIDI::parse(byte midiByte) {
  // Ignore Real-Time messages (System & Clock).
  if (midiByte >= 0xF8) {
    return;
  }

  // Check if it's a new status byte (most significant bit is 1).
  if (midiByte & 0x80) {
    status = midiByte;
    // Reset data bytes for the new message.
    data1 = 0;
    data2 = 0;
  } else {
    // It's a data byte.
    if (data1 == 0) {
      data1 = midiByte;

      // Check if it's a 2-byte message (like Program Change) which is now complete.
      byte command = status & 0xF0;
      if (command == 0xC0 || command == 0xD0) {
        if (handleProgramChange && command == 0xC0) {
          handleProgramChange((status & 0x0F) + 1, data1);
        }
        // TODO: Channel Pressure (0xD0) could be handled here.
        status = 0; data1 = 0; // Message processed, reset state.
      }
    } else {
      data2 = midiByte;

      // It's a 3-byte message, and it's now complete.
      byte command = status & 0xF0;
      byte channel = (status & 0x0F) + 1;

      switch(command) {
        case 0x90: // Note On
          // Note On with velocity 0 is often treated as Note Off.
          // This library calls the appropriate callback for clarity.
          if (data2 > 0) {
            if (handleNoteOn) handleNoteOn(channel, data1, data2);
          } else {
            if (handleNoteOff) handleNoteOff(channel, data1, data2);
          }
          break;
        case 0x80: // Note Off
          if (handleNoteOff) handleNoteOff(channel, data1, data2);
          break;
        case 0xB0: // Control Change
          if (handleControlChange) handleControlChange(channel, data1, data2);
          break;
        case 0xE0: // Pitch Bend
          if (handlePitchBend) {
            // Combine the two 7-bit values into one 14-bit value.
            int value = ((int)data2 << 7) | (int)data1;
            // Convert back to the -8192..8191 range.
            handlePitchBend(channel, value - 8192);
          }
          break;
        // Other 3-byte messages could be added here (e.g., Polyphonic Aftertouch 0xA0).
      }
      // Reset data bytes for the next message. Status is kept for "Running Status".
      data1 = 0;
      data2 = 0;
    }
  }
}