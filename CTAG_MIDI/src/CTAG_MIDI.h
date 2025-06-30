/*
  CTAG_MIDI.h - v1.0.0
  A flexible, callback-based MIDI library for Arduino and other platforms.
  Created by Tim Sonnenschein & Gemini, June 20, 2025.
  Released into the public domain.
*/
#ifndef CTAG_MIDI_h
#define CTAG_MIDI_h

#include <Arduino.h>

// --- Typdefinitionen für die Callback-Funktionen ---
// Dies macht den Code im Hauptprogramm sauberer und lesbarer.
typedef void (*NoteOnCallback)(byte channel, byte note, byte velocity);
typedef void (*NoteOffCallback)(byte channel, byte note, byte velocity);
typedef void (*ControlChangeCallback)(byte channel, byte controller, byte value);
typedef void (*ProgramChangeCallback)(byte channel, byte program);
typedef void (*PitchBendCallback)(byte channel, int value); // Pitch Bend ist 14-bit (-8192 to 8191)

class CTAG_MIDI {
public:
  /**
   * @brief Konstruktor
   */
  CTAG_MIDI();

  /**
   * @brief Initialisiert die Library mit einer seriellen Schnittstelle.
   * @param port Eine Referenz auf den Serial Stream (z.B. Serial1, Serial2).
   */
  void begin(Stream& port);

  /**
   * @brief Liest eingehende MIDI-Daten. Muss in der loop() aufgerufen werden.
   */
  void read();

  // --- SENDEN VON MIDI-NACHRICHTEN ---

  void sendNoteOn(byte note, byte velocity, byte channel);
  void sendNoteOff(byte note, byte velocity, byte channel);
  void sendControlChange(byte controller, byte value, byte channel);
  void sendProgramChange(byte program, byte channel);
  /**
   * @brief Sendet eine Pitch-Bend-Nachricht.
   * @param value Der Pitch-Bend-Wert. Bereich: -8192 (ganz runter) bis 8191 (ganz hoch). 0 ist die Mitte.
   * @param channel Der MIDI-Kanal (1-16).
   */
  void sendPitchBend(int value, byte channel);

  // --- CALLBACKS ZUWEISEN ---

  void setHandleNoteOn(NoteOnCallback fptr);
  void setHandleNoteOff(NoteOffCallback fptr);
  void setHandleControlChange(ControlChangeCallback fptr);
  void setHandleProgramChange(ProgramChangeCallback fptr);
  void setHandlePitchBend(PitchBendCallback fptr);


  // --- NÜTZLICHE HELFERFUNKTIONEN ---

  /**
   * @brief Wandelt eine MIDI-Notennummer (0-127) in einen lesbaren Namen um (z.B. "C#4").
   * @param noteNumber Die MIDI-Notennummer.
   * @return Ein String-Objekt mit dem Notennamen.
   * @note Diese Funktion ist 'static', d.h. sie kann auch ohne ein CTAG_MIDI-Objekt aufgerufen werden: CTAG_MIDI::getNoteName(60);
   */
  static String getNoteName(byte noteNumber);

private:
  // Interne Implementierungsdetails
  Stream* midiPort;
  void parse(byte midiByte);
  byte status;
  byte data1;
  byte data2;

  NoteOnCallback handleNoteOn;
  NoteOffCallback handleNoteOff;
  ControlChangeCallback handleControlChange;
  ProgramChangeCallback handleProgramChange;
  PitchBendCallback handlePitchBend;
};

#endif