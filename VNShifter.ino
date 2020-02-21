#include "Gamepad.h"
#include "Potentiometer.h"

#define BAUD_RATE 115200

Gamepad gamepad;
//A0 for X and A1 for Y
Potentiometer potX(A0, 0.1), potY(A1, 0.1) ;

uint8_t red = 0, green = 0, blue = 0;
const uint8_t redPin = 3, greenPin = 5, bluePin = 6;




uint16_t x = 0; // for horizontal
uint16_t y = 0; //for vertical
uint8_t shifterValue = 0;
uint8_t lastShifterValue = 0;
bool blink_led = false;
uint8_t hshifter = 1;
const uint16_t xThressHold[6] = {510, 495, 485, 480, 470, 468};
const uint16_t yHigh = 480;
const uint16_t yLow = 750;
const uint16_t y1SeqTH = 560;
const uint16_t y2SeqTH = 498;

void setup() {
  Serial.begin(BAUD_RATE);
  gamepad.begin();
  cli();
  /* Reset Timer/Counter1 */
  TCCR1A = 0;
  TCCR1B = 0;
  TIMSK1 = 0;
  /* Setup Timer/Counter1 */
  TCCR1B |= (1 << WGM12) | (1 << CS11) | (1 << CS10); // prescale = 64 and CTC mode 4
  OCR1A = 249;              // initialize OCR1A
  TIMSK1 = (1 << OCIE1A);     // Output Compare Interrupt Enable Timer/Counter1 channel A
  sei();
}

ISR(TIMER1_COMPA_vect) {
  //  release all previos value of gamepad
  gamepad.releaseAll();
  potX.update();
  potY.update();
    hshifter = digitalRead(10);
    gamepad.press(hshifter);
  if (hshifter) {
    x = mappedX(potX.filteredValue);
    y = mappedY(potY.filteredValue);
    shifterValue = calculatedHShifter(x, y);      
    gamepad.write();
  } else {
    shifterValue = calculatedSShifter(potX.filteredValue,potY.filteredValue);
    gamepad.press(shifterValue);
    gamepad.write();
  }
  setColors();
}

int16_t i = 0; // for rgb value
uint8_t _delay =  6;    // Delay time
uint8_t _step =  1;    // Increment/decrement size
uint8_t fadePin = redPin; // 0 for red, 1 for green, 2 for blue;
boolean fadeOn = true;
void loop() {
  if (shifterValue != lastShifterValue and shifterValue != 0 ) {
    lastShifterValue = shifterValue;
    for (uint8_t j = 0; j < 5; j++) {
      writeColor( red, green, blue );
      delay(100);
      writeColor( 0, 0, 0);
      delay(100);
    }
  }
  writeHSV(i,1,1); 
  i++;
  if (i>360) {
    i=0;
  }
  delay (20);

}

//copy this function from RGBLED created by Bret Stateham
void writeHSV(int h, double s, double v) {
  //this is the algorithm to convert from RGB to HSV
  double r=0; 
  double g=0; 
  double b=0;

  double hf=h/60.0;

  int i=(int)floor(h/60.0);
  double f = h/60.0 - i;
  double pv = v * (1 - s);
  double qv = v * (1 - s*f);
  double tv = v * (1 - s * (1 - f));

  switch (i)
  {
  case 0: //rojo dominante
    r = v;
    g = tv;
    b = pv;
    break;
  case 1: //verde
    r = qv;
    g = v;
    b = pv;
    break;
  case 2: 
    r = pv;
    g = v;
    b = tv;
    break;
  case 3: //azul
    r = pv;
    g = qv;
    b = v;
    break;
  case 4:
    r = tv;
    g = pv;
    b = v;
    break;
  case 5: //rojo
    r = v;
    g = pv;
    b = qv;
    break;
  }

  //set each component to a integer value between 0 and 255
   red=constrain((int)255*r,0,255);
   green=constrain((int)255*g,0,255);
   blue=constrain((int)255*b,0,255);

  writeColor(red,green,blue);
}

void setColors() {
  switch (shifterValue) {
    case 0:
      //Serial.println("0");
      red = 0;
      green = 0;
      blue = 0;
      break;
    case 1:
      red = 255;
      green = 0;
      blue = 0;
      break;
    case 2:
      red = 0;
      green = 255;
      blue = 0;
      break;
    case 3:
      red = 0;
      green = 0;
      blue = 255;
      break;
    case 4:
      red = 255;
      green = 255;
      blue = 0;
      break;
    case 5:
      red = 255;
      green = 0;
      blue = 255;
      break;
    case 6:
      red = 0;
      green = 255;
      blue = 255;
      break;
    case 7:
      red = 255;
      green = 255;
      blue = 255;
      break;
    default:
      break;
  }
}
void writeColor(uint8_t red, uint8_t green, uint8_t blue) {
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
}

uint8_t mappedX(uint16_t x) {
  if ( x > xThressHold[0]) {
    return 0;
  } else if (x < xThressHold[1] && x > xThressHold[2]) {
    return 1;
  }
  else if (x < xThressHold[3] && x > xThressHold[4]) {
    return 2;
  } else if (x < xThressHold[5]) {
    return 3;
  }
  else {
    return 4;
  }
}

uint8_t mappedY(uint16_t y) {
  if ( y > yLow) {
    return 2;
  }
  else {
    if ( y < yHigh) {
      return 0;
    }
    else {
      return 1;
    }

  }
}
uint8_t calculatedSShifter(uint16_t x, uint16_t y) {
  if (x >= y1SeqTH) {
    return 1;
  }
  if (x <= y2SeqTH) {
    return 2;
  }
  return 0;
}

uint8_t calculatedHShifter(uint8_t x, uint8_t y) {
  if (x == 0 && y == 0) {
    return 1;
  } else if (x == 0 && y == 2) {
    return 2;
  } else if (x == 1 && y == 0) {
    return 3;
  }
  else if (x == 1 && y == 2) {
    return 4;
  }
  else if (x == 2 && y == 0) {
    return 5;
  }
  else if (x == 2 && y == 2) {
    return 6;
  }
  else if (x == 3 && y == 2) {
    return 7;
  } else {
    return 0;
  }
}
