/*
  Beispiel 2, Nano Motorsteuerung
  Langsam anfahren und anhalten 
  
  Version 1.20, 06.04.2021

  Der Hobbyelektroniker, AMrobot
  https://github.com/hobbyelektroniker/NanoMotorsteuerung
  https://community.hobbyelektroniker.ch
  https://www.youtube.com/c/HobbyelektronikerCh
  Der Code kann mit Quellenangabe frei verwendet werden.
*/

// Bibliothek für Motortreiber einbinden
#include "motor.h"


// Variable für Motor erzeugen
DCMotor motor('A'); // an Motoranschluss A

void demo() {
  // zuerst steht der Motor
  motor.stop();

  // vorwärts
  motor.setForward();
  
  // auf volle Geschwindigkeit (255) beschleunigen
  motor.speedUp(255);

  delay(1200); // Geschwindigkeit halten
  
  // auf halbe Geschwindigkeit reduzieren
  motor.slowDown(127);

  delay(1200); // Geschwindigkeit halten

  // ganz langsam anhalten
  motor.setAcceleration(1); // Standard wäre 5
  motor.slowDown(0);

  // Anhalten
  motor.stop();  
}

void setup() {
  motor.begin(); // Motor vorbereiten
}


void loop() {
  demo();         // Demo ablaufen lassen
  delay(10000);   // 10 Sekunden warten
}
