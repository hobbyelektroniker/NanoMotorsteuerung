/* 
  Motorsteuerung mit L298P
  Version für Nano Motortreiber
  Version 1.0, 18.03.2021
  
  Der Hobbyelektroniker, AMrobot
  https://github.com/hobbyelektroniker/NanoMotorsteuerung
  https://community.hobbyelektroniker.ch
  https://www.youtube.com/c/HobbyelektronikerCh
  Der Code kann mit Quellenangabe frei verwendet werden.
*/

#pragma once

// Konstanten für Beschleunigen und Abbremsen
const int stdAccelStep = 5;
const unsigned long stdAccelDelay = 100;

class DCMotor {
  public:
    DCMotor(char id) {
      if (id == 'B') {
        _directionPin = 13; // Kolidiert mit LED_BUILTIN
        _speedPin = 11;
        _breakPin = 8;
        _monitorPin = A1;
      } else {
        _directionPin = 12;
        _speedPin = 3;
        _breakPin = 9;
        _monitorPin = A0;
      }
      _direction = true; // forward
      analogWrite(_speedPin,0); // Motor aus 
      pinMode(_directionPin,OUTPUT);
      pinMode(_breakPin,OUTPUT);
    }

    void begin() {
      stop();
    }

    void setAcceleration(int step, unsigned long delay = 0) {
      _accelStep = step;
      if (delay) _accelDelay = delay;
    }

    void setForward(bool forward = true) {
      if (forward != _direction) stop();
      _direction = forward;
      digitalWrite(_directionPin, _direction);
    }

    bool getForward() {
      return _direction; 
    }

    void setSpeed(int speed) {
      _speed = constrain(speed, 0, 255);
      analogWrite(_speedPin,_speed);
    }

    int getSpeed() {
      return _speed;
    }

    void stop() {
      int oldSpeed = getSpeed();
      setSpeed(0);
      _inAction = false;
      if (oldSpeed) delay(1000);
    }

    // Anfahren
    void speedUp(int to, bool (*abortAction) () = NULL) {
      _inAction = true;
      for (int i=_speed; i <= to; i+= _accelStep) {
        if (abortAction && abortAction()) _inAction = false;;
        if (!_inAction) return;
        setSpeed(i);
        delay(_accelDelay);      
      }
    }
   
    // Anhalten
    void slowDown(int to, bool (*abortAction) () = NULL) {
      _inAction = true;
      for (int i=_speed; i >= to; i-= _accelStep) {
        if (abortAction && abortAction()) _inAction = false;
        if (!_inAction) return;
        setSpeed(i);
        delay(_accelDelay);      
      }
    }

    int getMonitor() {
      return analogRead(_monitorPin);
    }

  private:
    int _directionPin;
    int _speedPin;
    int _breakPin;
    int _monitorPin;
    int _direction; 
    int _speed;
    bool _inAction = false;
    int _accelStep = 5;
    unsigned long _accelDelay = 100;
};
