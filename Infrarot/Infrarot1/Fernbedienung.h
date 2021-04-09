/* Klasse für Infrarotempfänger
 * Speziell für 'Nano Motorsteuerung'
 * 
 * Version 1.00, 09.04.2021
 *
 * Der Hobbyelektroniker, AMrobot
 * https://github.com/hobbyelektroniker/NanoMotorsteuerung
 * https://community.hobbyelektroniker.ch
 * https://www.youtube.com/c/HobbyelektronikerCh
 *  
 * Bitte Copyright - Hinweise in SimpleIRReceiver.h und digitalWriteFast.h beachten!
 *
 * Für Fernbedienung.h gelten diese Lizenzbedingungen:
 * ***************************************************************************************
 * MIT License
 *
 * Copyright (c) 2021 Der Hobbyelektroniker (René Lobsiger)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 * OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * ***************************************************************************************
 *
 */

#pragma once

#include <arduino.h>

#define IRCMD_NONE  0x00

#define IRCMD_UP    0x46
#define IRCMD_DOWN  0x15
#define IRCMD_LEFT  0x44
#define IRCMD_RIGHT 0x43
#define IRCMD_OK    0x40

#define IRCMD_0     0x52
#define IRCMD_1     0x16
#define IRCMD_2     0x19
#define IRCMD_3     0x0D
#define IRCMD_4     0x0C
#define IRCMD_5     0x18
#define IRCMD_6     0x5E
#define IRCMD_7     0x08
#define IRCMD_8     0x1C
#define IRCMD_9     0x5A

#define IRCMD_STAR  0x42
#define IRCMD_HASH  0x4A

typedef void (*CallBackFunction) (int);

// Klasse, die etwas weiter unten implementiert wird
class IREmpfaenger {
  public:
    void begin(uint8_t mode = 0, uint8_t pin = 2);
    void refresh();
    void setCallback(CallBackFunction newCmd);
    uint8_t getCmd();
    void newCode(uint8_t aCode, bool isRepeat);
  private:
    void newCmd(int cmd);

    uint8_t _pin;
    uint8_t _mode;
    uint8_t _code, _oldCode;
    bool _repeat;
    unsigned long _oldMillis;
    CallBackFunction _callback; 
};

// Die einzige Instanz des Empfängers
IREmpfaenger empfaenger;

// Wird automatisch aufgerufen, wenn eine Taste gedrückt wird
void handleReceivedTinyIRData(uint16_t aAddress, uint8_t aCommand, bool isRepeat) {
  empfaenger.newCode(aCommand, isRepeat);
}

#include "SimpleIRReceiver.h"

// Initialisierung
// In der Nano Motorsteuerung muss Pin 2 verwendet werden!
// Standard ist 2
void IREmpfaenger::begin(uint8_t mode, uint8_t pin) {
  _mode = mode;
  _pin = pin;
  _code = 0;
  _oldCode = 0;
  _repeat = false;
  _oldMillis = millis();
  initPCIInterruptForTinyReceiver(_pin);
}

// Verarbeitet die Kommandos
// Muss in kurzen Abständen aufgerufen werden.
// Nicht abgerufene Kommandos gehen verloren
// Wenn länger als 200 ms kein neues Komando mehr empfangen
// wird, sendet die Klassen ein IRCMD_NONE
void IREmpfaenger::refresh() {
  if (millis() - _oldMillis > 200) {
    if (_oldCode) newCmd(IRCMD_NONE);
    _code = IRCMD_NONE;
    _oldCode = _code;
    _oldMillis = millis();
  } else {
    if (_oldCode != _code) {
      if (_code && _mode == 0) newCmd(_code);
      _oldCode = _code;
    }
  }
}

uint8_t IREmpfaenger::getCmd() {
  if (millis() - _oldMillis > 200) _code = IRCMD_NONE;
  return _code;
}

// Wird automatisch vom aufgerufen, wenn ein Code empfangen wird
void IREmpfaenger::newCode(uint8_t aCode, bool isRepeat) {
  _oldMillis = millis();
  _code = aCode;
  _repeat = isRepeat;  
  if (_mode == 1) newCmd(_code);
}

// Aufruf der Callback - Funktion
void IREmpfaenger::newCmd(int cmd) {
  if (_callback && _mode < 2) _callback(cmd);
}

// Übergibt eine Callback - Funktion, die bei jedem Kommandowechsel einmal aufgerufen wird
void IREmpfaenger::setCallback(CallBackFunction newCmd) {
  _callback = newCmd;
}
