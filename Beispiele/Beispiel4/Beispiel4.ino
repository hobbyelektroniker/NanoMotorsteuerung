/*
  Beispiel 4, Nano Motortreiber
  Mit Endschalter
  Langsam abfahren
  Beim Ankommen wird verlangsamt 
  
  Da kein Sensor dafür zur Verfügung steht, wird
  die Verlangsamung zeitgesteuert eingeleitet
  
  Der Endschalter führt zum definitiven Stopp
  
  Version 1.20, 06.04.2021

  Der Hobbyelektroniker, AMrobot
  https://github.com/hobbyelektroniker/NanoMotorsteuerung
  https://community.hobbyelektroniker.ch
  https://www.youtube.com/c/HobbyelektronikerCh
  Der Code kann mit Quellenangabe frei verwendet werden.
*/

// Bibliothek für Motortreiber einbinden
#include "motor.h"

// Pins für Endschalter definieren
// Die Endschalter verbinden bei Betätigung den Pin mit GND
const int vornePin = 6;
const int hintenPin = 7;

// Variable für Motor erzeugen
DCMotor motor('A'); // an Motoranschluss A

// Startzeit
unsigned long startZeit;

void setup() {
  // Motor vorbereiten
  motor.begin(); 

  // Endschalter vorbereiten
  pinMode(vornePin, INPUT_PULLUP);
  pinMode(hintenPin, INPUT_PULLUP);

  // Wir testen, ob wir schon an einem Ende stehen
  if (!digitalRead(vornePin)) {
    // wir stehen vorne, müssen also nach hinten losfahren
    abfahrt(false);
  } else if (!digitalRead(hintenPin)) {
    abfahrt(true);
  } else {
    // wir stehen irgendwo in der Mitte und fahren nach vorne los
    abfahrt(true);    
  }
}


bool testVorne() {
  return (motor.getForward() && !digitalRead(vornePin));
}

bool testHinten() {
  return (!motor.getForward() && !digitalRead(hintenPin));
}

void abfahrt(bool nachVorne) {
  startZeit = millis();
  motor.setForward(nachVorne);
  motor.speedUp(255); // Auf volle Geschwindigkeit beschleunigen
}

void ankunftUndAbfahrt() {
  if (motor.getForward()) {
    // Wir sind fahren vorwärts
    motor.slowDown(50, testVorne);
    while (!testVorne()) delay(5);
    motor.stop(); // anhalten
    delay(3000);  // 3 Sekunden Pause
    abfahrt(false);
  } else {
    motor.slowDown(50, testHinten);
    while (!testHinten()) delay(5);
    motor.stop(); // anhalten
    delay(3000);  // 3 Sekunden Pause
    abfahrt(true); 
  }
}


void loop() {
  if (millis() - startZeit > 8000) ankunftUndAbfahrt();
  delay(50); // Eine kleine Pause zwischen den Abfragen schadet nichts
}
