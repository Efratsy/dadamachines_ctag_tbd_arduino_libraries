/**
 * @file CTAG_MIDI_Demo.ino
 * @brief A comprehensive I/O test sketch for the CTAG_MIDI library.
 *
 * This example demonstrates both sending and receiving all common MIDI messages.
 * It uses a callback system to handle incoming data and a non-blocking state
 * machine in the loop() to automatically send a cycle of test messages.
 * For a full test, connect the MIDI OUT to the MIDI IN of your device.
 */

#include "CTAG_MIDI.h"

/**
 * @brief Global instance of our MIDI library.
 */
CTAG_MIDI midi;

/**
 * @brief Variables for the automatic message sending test.
 */
unsigned long lastSendTime = 0;
const int sendInterval = 3000; // Send a new type of message every 3 seconds.
int sendState = 0;             // The current state of the sending cycle.


// ====================================================================================
//                            1. RECEIVE FUNCTIONS (CALLBACKS)
// ====================================================================================

/** @brief Callback for Note On messages. */
void handleNoteOn(byte channel, byte note, byte velocity) {
  String noteName = CTAG_MIDI::getNoteName(note);
  Serial.print("<- IN: Note On\t(Channel: ");
  Serial.print(channel);
  Serial.print(", Note: ");
  Serial.print(noteName);
  Serial.print(", Velocity: ");
  Serial.print(velocity);
  Serial.println(")");
}

/** @brief Callback for Note Off messages. */
void handleNoteOff(byte channel, byte note, byte velocity) {
  String noteName = CTAG_MIDI::getNoteName(note);
  Serial.print("<- IN: Note Off\t(Channel: ");
  Serial.print(channel);
  Serial.print(", Note: ");
  Serial.print(noteName);
  Serial.print(", Velocity: ");
  Serial.print(velocity);
  Serial.println(")");
}

/** @brief Callback for Control Change (CC) messages. */
void handleControlChange(byte channel, byte controller, byte value) {
  Serial.print("<- IN: CC\t\t(Channel: ");
  Serial.print(channel);
  Serial.print(", Controller: ");
  Serial.print(controller);
  Serial.print(", Value: ");
  Serial.print(value);
  Serial.println(")");
}

/** @brief Callback for Program Change messages. */
void handleProgramChange(byte channel, byte program) {
  Serial.print("<- IN: Prg Change\t(Channel: ");
  Serial.print(channel);
  Serial.print(", Program: ");
  Serial.print(program);
  Serial.println(")");
}

/** @brief Callback for Pitch Bend messages. */
void handlePitchBend(byte channel, int value) {
  Serial.print("<- IN: Pitch Bend\t(Channel: ");
  Serial.print(channel);
  Serial.print(", Value: ");
  Serial.print(value);
  Serial.println(")");
}


// ====================================================================================
//                                      SETUP
// ====================================================================================

/**
 * @brief Runs once at startup to initialize everything.
 */
void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }
  Serial.println("\n--- CTAG_MIDI v1.0 Complete I/O Test ---");
  Serial.println("Connect MIDI OUT to MIDI IN for a full loopback test.");

  // Start the hardware serial port for MIDI communication (e.g., on RP2040 IO8/IO9).
  Serial2.begin(31250); // Standard MIDI baud rate

  // Initialize the library with the MIDI port.
  midi.begin(Serial2);

  // Register all callback functions.
  midi.setHandleNoteOn(handleNoteOn);
  midi.setHandleNoteOff(handleNoteOff);
  midi.setHandleControlChange(handleControlChange);
  midi.setHandleProgramChange(handleProgramChange);
  midi.setHandlePitchBend(handlePitchBend);
  
  Serial.println("System ready. Listening for MIDI data and starting send cycle...");
}


// ====================================================================================
//                                       LOOP
// ====================================================================================

/**
 * @brief Runs continuously after setup.
 */
void loop() {
  // ALWAYS FIRST: Process any incoming MIDI data.
  midi.read();

  // --- Automatic Send Test ---
  // Every `sendInterval` milliseconds, the next test message is sent.
  if (millis() - lastSendTime > sendInterval) {
    lastSendTime = millis();

    Serial.println("------------------------------------------");
    
    switch (sendState) {
      case 0:
        Serial.println("-> OUT: Sending Note On/Off (C4)...");
        midi.sendNoteOn(60, 100, 1); // Note C4, Velocity 100, Channel 1
        // This small delay is just to make the note audible in a loopback test.
        // In real applications, avoid using delay() in the main loop.
        delay(500);
        midi.sendNoteOff(60, 0, 1);
        break;
        
      case 1:
        { // Use a block to create a local scope for the variable.
          byte ccValue = (millis() / 50) % 128; // A changing value.
          Serial.print("-> OUT: Sending Control Change (CC 74, Value ");
          Serial.print(ccValue);
          Serial.println(")...");
          midi.sendControlChange(74, ccValue, 1); // CC 74 (Filter Cutoff), Channel 1
        }
        break;
        
      case 2:
        {
          byte program = random(1, 40);
          Serial.print("-> OUT: Sending Program Change (Program ");
          Serial.print(program);
          Serial.println(")...");
          midi.sendProgramChange(program, 1);
        }
        break;
        
      case 3:
        Serial.println("-> OUT: Sending Pitch Bend (UP)...");
        midi.sendPitchBend(8191, 1); // Maximum pitch bend up.
        break;
        
      case 4:
        Serial.println("-> OUT: Sending Pitch Bend (CENTER)...");
        midi.sendPitchBend(0, 1); // Return pitch bend to center.
        break;
    }

    sendState = (sendState + 1) % 5; // Advance to the next send state.
  }
}