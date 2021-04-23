/*
 * Remote Mode 0
 * Direkte Abfrage des aktuellen Kommandos
 * Das Kommando entspricht dem zuletzt gedrückten Code
 * Nach Abfrage wird das Kommando gelöscht
 * Benötigt kein Callback und kein Refresh
 * 
 * Option:
 * - setNoRepeat(true)
 * Das Kommando wird nach Abfrage nicht mehr gelöscht
 * Jedes Kommando wird aber nur einmal zurückgegeben
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
#include "Fernbedienung.h"

void setup() {
  Serial.begin(115200);
  // Fernbedienung vorbereiten
  empfaenger.begin(0);
  // empfaenger.setNoRepeat();
}


void loop() {
  uint8_t cmd = empfaenger.getCmd();
  if (cmd) Serial.println(cmd); // IRCMD_NONE wird nicht angezeigt
  delay(50); // Eine kleine Pause zwischen den Abfragen schadet nichts
}
