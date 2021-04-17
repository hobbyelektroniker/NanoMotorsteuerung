/*
  Beispiel 3, Nano Motorsteuerung
  Endschalter verwenden
  
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

void setup() {
  // Motor vorbereiten
  motor.begin(); 

  // Endschalter vorbereiten
  pinMode(vornePin, INPUT_PULLUP);
  pinMode(hintenPin, INPUT_PULLUP);

  // Wir testen, ob wir schon an einem Ende stehen
  if (!digitalRead(vornePin)) {
    // wir stehen hinten, müssen also nach vorne losfahren
    motor.setForward(false);
  } else if (!digitalRead(hintenPin)) {
    motor.setForward();
  } else {
    // wir stehen irgendwo in der Mitte und fahren nach vorne los
    motor.setForward();    
  }
  motor.setSpeed(255); // Es geht mit voller Geschwindigkeit los
}


void loop() {
  if (motor.getForward() && !digitalRead(vornePin)) {
    // Wenn wir nach vorne fahren und vorne angekomen sind
    // halten wir an und fahren nach 3 Sekunden Pause nach hinten los
    motor.stop(); // anhalten
    delay(3000);  // 3 Sekunden Pause
    motor.setForward(false); // Richtung nach hinten
    motor.setSpeed(255); // los geht's 
  } else if (!motor.getForward() && !digitalRead(hintenPin)) {
    // Wenn wir nach hinten fahren und hinten angekomen sind
    // halten wir an und fahren nach 3 Sekunden Pause nach vorne los
    motor.stop(); // anhalten
    delay(3000);  // 3 Sekunden Pause
    motor.setForward(); // Richtung nach vorne
    motor.setSpeed(255); // los geht's 
  }
  delay(50); // Eine kleine Pause zwischen den Abfragen schadet nichts
}
