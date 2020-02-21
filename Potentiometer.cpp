#include "Potentiometer.h"
Potentiometer::Potentiometer(uint8_t inputPin, float inputAlpha) {
  pin = inputPin;
  alpha = inputAlpha;
  filteredValue = analogRead(pin);
}

Potentiometer::~Potentiometer() {
}


void Potentiometer::update(){
  inputValue = analogRead(pin);
  filteredValue = alpha*inputValue + (1-alpha)*filteredValue;
}
