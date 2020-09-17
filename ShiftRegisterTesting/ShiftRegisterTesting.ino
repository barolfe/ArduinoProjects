/*
Adafruit Arduino - Lesson 4. 8 LEDs and a Shift Register
*/
// NodeMCU definitions
int latchPin = D7; // GPIO12  - ESP8266 definitions
int clockPin = D8; // GPIO15
int dataPin = D6;  // GPIO13 

int leds = 0;

//  |1||2||  3||4||5|  
// 01111111, 11111111
// BGR
const unsigned int led5 = 57344; // 1110000000000000 in binary
const unsigned int led4 = led5 >> 3;
const unsigned int led3 = led4 >> 3;
const unsigned int led2 = led3 >> 3;
const unsigned int led1 = led2 >> 3;
const int ledArray[] = {led1, led2, led3, led4, led5};
const unsigned int blues = 37448; // 0100100100100100 in binary
const unsigned int greens = blues >> 1;
const unsigned int reds  = greens >> 1; 


void setup() 
{
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);
  delay(1000);
  Serial.begin(9600);
  for (int i = 0; i < 16; i++) {
    Serial.print(bitRead(led5,i));
  }
  Serial.println();
  
}

void loop() 
{
  leds = 0;
  updateShiftRegister(leds);
  delay(500);

  // Custom color sequences
  leds = (blues | greens) & (led1 | led3 | led5);
  updateShiftRegister(leds);
  delay(500);
  leds = (blues | reds) & (led2 | led4);
  updateShiftRegister(leds);
  delay(500);
  leds = (greens | reds) & ((~0) >> 1);
  updateShiftRegister(leds);
  delay(500);

  // Each a different color
  leds = 0;
  leds = (blues & led1) | (greens & led2) | ((reds | greens) & led3) | ((blues | greens) & led4) | ((reds | blues) & led5);
  updateShiftRegister(leds);
  delay(1000);
  
  updateShiftRegister(blues);
  delay(500);
  updateShiftRegister(greens);
  delay(500);
  updateShiftRegister(reds);
  delay(500);

  for (int i = 0; i < 5; i++) {
    int mask = ledArray[i];
    updateShiftRegister(blues & mask);
    delay(150);
    updateShiftRegister(greens & mask);
    delay(150);
    updateShiftRegister(reds & mask);
    delay(150); 
  }

  
  if (false) {
    for (int j = 1; j < 4; j++) {
      leds = 0;
      for (int i = j; i < 16; i+=3)
      {
        bitSet(leds, i);
        updateShiftRegister(leds);
        delay(100);
      }
    }
  }
}

void updateShiftRegister(int leds)
{
   digitalWrite(latchPin, LOW);
   shiftOut(dataPin, clockPin, LSBFIRST, leds);
   shiftOut(dataPin, clockPin, LSBFIRST, (leds >> 8));
   digitalWrite(latchPin, HIGH);
}
