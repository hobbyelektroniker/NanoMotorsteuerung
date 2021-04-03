/*
  Beispiel 1, Nano Motortreiber
  Grundlegende Steuerbefehle
  vorwärts, rückwärts, anhalten, Geschwindigkeit 
  
  Version 1.00, 17.03.2021

  Der Hobbyelektroniker, AMrobot
  https://github.com/hobbyelektroniker/NanoMotortreiber
  https://community.hobbyelektroniker.ch
  https://www.youtube.com/c/HobbyelektronikerCh
  Der Code kann mit Quellenangabe frei verwendet werden.
*/

// Bibliothek für Motortreiber einbinden
#include "motor.h"


// Variable für Motor erzeugen
DCMotor motor('A'); // an Motoranschluss A

void demo() {
  // Mit mittlerer Geschwindigkeit vorwärts fahren
  motor.setForward();
  motor.setSpeed(127); // 0 .. 255 möglich
  delay(3000);

  // Auf volle Geschwindigkeit wechseln
  motor.setSpeed(255);
  delay(3000);

  // Sehr langsam fahren
  motor.setSpeed(50);
  delay(3000);

  // Anhalten
  motor.stop();

  // Mit mittlerer Geschwindigkeit rückwärts fahren
  motor.setForward(false);
  motor.setSpeed(127);
  delay(3000);

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
