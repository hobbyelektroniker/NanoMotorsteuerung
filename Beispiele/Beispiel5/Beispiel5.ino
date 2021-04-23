/*
 * Beispiel 5, Nano Motorsteuerung
 * Mit Fernbedienung
 * Ohne Endschalter
 * Langsam abfahren (links / rechts) und anhalten (OK)
 * Direkt fahren (auf, ab) und anhalten bei Loslassen
 * Notstopp mit 0
 *
 * Das Anfahren und Anhalten erfolgt über die Fernbedienung.
 * 
 * Version 1.00, 18.04.2021
 *
 *
 * Der Hobbyelektroniker, AMrobot
 * https://github.com/hobbyelektroniker/NanoMotorsteuerung
 * https://community.hobbyelektroniker.ch
 * https://www.youtube.com/c/HobbyelektronikerCh
 *  
 * Bitte Lizenzhinweise in SimpleIRReceiver.h und digitalWriteFast.h beachten!
 *
 * Für den Beispielcode und Fernbedienung.h gelten diese Lizenzbedingungen: 
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

// Bibliotheken einbinden
#include "motor.h"
#include "Fernbedienung.h"

// Pins für Endschalter definieren
// Die Endschalter verbinden bei Betätigung den Pin mit GND
const int vornePin = 6;
const int hintenPin = 7;

// Zustand
const int stehend = 0;
const int abfahrend = 1;
const int ankommend = 2;
const int handbetrieb = 3;

int zustand;

// Variable für Motor erzeugen
DCMotor motor('A'); // an Motoranschluss A

// Startzeit
unsigned long startZeit;

// Callback - Funktion für Fernbedienung
void newCmd(int cmd) {
  switch (cmd) {
    case IRCMD_LEFT:  // Normal nach link abfahren
      Serial.println(String(zustand)+": "+"LEFT");
      if (zustand == stehend) abfahrt(true); // nach vorne losfahren
      break;
    case IRCMD_RIGHT: // Normal nach rechts abfahren
      Serial.println(String(zustand)+": "+"RIGHT");
      if (zustand == stehend) abfahrt(false); // nach hinten losfahren
      break;
    case IRCMD_OK: // Anhalten
      Serial.println(String(zustand)+": "+"ANHALTEN");
      if (zustand == handbetrieb) {
        Serial.println(String(zustand)+": "+"NOTSTOPP");
        motor.stop();
        zustand = stehend;
        return;
      } else {
        if (zustand != stehend && zustand != ankommend) ankunft();  // ankommen
      }
      break;
    case IRCMD_0: // Notstopp
      Serial.println(String(zustand)+": "+"NOTSTOPP");
      motor.stop();
      zustand = stehend;
      break;
    case IRCMD_UP: // Direkt nach links
      Serial.println(String(zustand)+": "+"DIREKT LINKS");
      zustand = handbetrieb;
      motor.setForward(true);
      motor.setSpeed(255);
      break;
    case IRCMD_DOWN: // Direkt nach rechts
      Serial.println(String(zustand)+": "+"DIREKT RECHTS");
      zustand = handbetrieb;
      motor.setForward(false);
      motor.setSpeed(255);
      break;
    case IRCMD_NONE: // Taste losgelassen
      Serial.println(String(zustand)+": "+"TASTE LOSGELASSEN");
      if (zustand == handbetrieb) { 
        Serial.println(String(zustand)+": "+"DIREKT STOPPEN");
        zustand = stehend;
        motor.stop();
      }
      break;
  }
}

void setup() {
  Serial.begin(115200);
  // Motor vorbereiten
  motor.begin(); 
  zustand = stehend;
  // Fernbedienung vorbereiten
  empfaenger.begin(2);
  empfaenger.setNoRepeat();
  empfaenger.setCallback(newCmd);
}

void abfahrt(bool nachVorne) {
  zustand = abfahrend;
  startZeit = millis();
  motor.setForward(nachVorne);
  motor.speedUp(255); // Auf volle Geschwindigkeit beschleunigen
}

void ankunft() {
  zustand = ankommend;
  motor.slowDown(50);
  motor.stop(); // anhalten
  zustand = stehend;
}

void loop() {
  empfaenger.refresh();
  delay(10); // Eine kleine Pause zwischen den Abfragen schadet nichts
}
