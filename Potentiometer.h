#ifndef POTENTIONMETER_h
#define POTENTIONMETER_h
#include <Arduino.h>

class Potentiometer {
  public:
    Potentiometer(uint8_t inputPin, float inputAlpha);
    ~Potentiometer(void);
    uint16_t filteredValue;
    void update();
  private:    
    float alpha;
    uint8_t pin;
    uint16_t inputValue;   
};

#endif
