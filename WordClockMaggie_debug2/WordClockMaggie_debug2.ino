#include <FastLED.h>
#include <EEPROM.h>


#define NUM_LEDS 99
#define DATA_PIN 3
#define COLOR_ORDER GRB
#define LED_TYPE    WS2812B
#define BRIGHTNESS 100
#define ENABLE_RTC

#include "RTClib.h"
RTC_DS3231 rtc;
int minutes_global  = 53;
int hours_global = 9;
unsigned long time_last = 0;
unsigned long last_update = 0;
int update_interval = 100;

// Button items
int but_pins[] = {4, 5, 6, 7};
int state_last[] = {1, 1, 1, 1};
unsigned long state_debounce[] = {0, 0, 0, 0};
int but_state[] = {0, 0, 0, 0};
int debounce_time = 100;
uint8_t color_state = 9;

// States and EEPROM stuff
int eepromAddr = 0;

int R = 100;
int G = 0;
int B = 255;

uint8_t gHue = 255;
uint8_t gSat = 210;
uint8_t gVal = 255;


CRGB leds[NUM_LEDS];

struct Word {
  int row;
  int cols[6]; 
};

Word Yo     = {0, {0, 1, -1, -1, -1, -1}};
Word Mags   = {0, {3, 4, 5, 6, -1, -1}};
Word Dude   = {0, {7, 8, 9, 10, -1, -1}};
Word It     = {1, {0, 1, -1, -1, -1, -1}};
Word Is     = {1, {3, 4, -1, -1, -1, -1}};
Word About  = {1, {6, 7, 8, 9, 10, -1}};
Word Half   = {2, {0, 1, 2, 3, -1, -1}};
Word TenTo  = {2, {4, 5, 6, -1, -1, -1}};
Word To     = {2, {9, 10, -1, -1, -1, -1}};
Word Twenty = {3, {0, 1, 2, 3, 4, 5}};
Word Past   = {3, {7, 8, 9, 10, -1, -1}};
Word Eleven = {4, {0, 1, 2, 3, 4, 5}};
Word Four   = {4, {7, 8, 9, 10, -1, -1}};
Word Ten    = {5, {0, 1, 2, -1, -1, -1}};
Word Six    = {5, {3, 4, 5, -1, -1, -1}};
Word Seven  = {5, {6, 7, 8, 9, 10, -1}};
Word One    = {6, {0, 1, 2, -1, -1, -1}};
Word Twelve = {6, {4, 5, 6, 7, 8, 9}};
Word Five   = {7, {0, 1, 2, 3, -1, -1}};
Word Two    = {7, {4, 5, 6, -1, -1, -1}};
Word Nine   = {7, {7, 8, 9, 10, -1, -1}};
Word Three  = {8, {2, 3, 4, 5, 6, -1}};
Word Eight  = {8, {6, 7, 8, 9, 10, -1}};
Word Sea    = {1, {4, 5, 6, -1, -1, -1}};
Word U      = {1, {9, -1, -1, -1, -1, -1}};
Word In     = {7, {8, 9, -1, -1, -1, -1}};
Word LA     = {8, {0, 1, -1, -1, -1, -1}};

int secret_sequence[8] = {15, 16, 17, 20, 85, 86, 98, 97}; // hard-coded secret sequence, these are LED-positions

Word secret_words[4] = {Sea, U, In, LA};
Word hour_words[12] = {Twelve, One, Two, Three, Four, Five, Six, Seven, Eight, Nine, Ten, Eleven};
Word all_words[23] = {Yo, Mags, Dude, It, Is, About, Half, TenTo, To, Twenty, Past, Eleven,
            Four, Ten, Six, Seven, One, Twelve, Five, Two, Nine, Three, Eight};

int Test[] = {0, 1, 2, 3, 9, 10, 11, 12, 14, 15, 17, 18, 19, 20, 21, 26, 27, 28};

int letters[9][11] = {{11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1},
                      {12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22},
                      {23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33},
                      {34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44},
                      {55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45},
                      {56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66},
                      {77, 76, 75, 74, 73, 72, 71, 70, 69, 68, 67},
                      {78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88},
                      {99, 98, 97, 96, 95, 94, 93, 92, 91, 90, 89}};
                      
void setup() {
  // Button setup
  pinMode(but_pins[0], INPUT);
  pinMode(but_pins[1], INPUT);
  pinMode(but_pins[2], INPUT);
  pinMode(but_pins[3], INPUT);

  // Get EEPROM data saved
  color_state = getColorState();

  // LED setup
  FastLED.addLeds<WS2812B, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 11; j++) {
      letters[i][j] = letters[i][j] - 1; // Set to 0-indexing
    }
  }
  
  turnOffAll();
  stripeDownRainbow();
  stripeDownRainbowOff();
  delay(1000);
  
  //testAll();
  
  Serial.begin(9600);

  updateTime();
  time_last = millis();
  
}

void loop() {
  //testAll();
  /*
  for (int i = 0; i < 23; i++) {
    turnOn(all_words[i]);
    FastLED.show();
    delay(100);
    turnOff(all_words[i]);
    
  }
  */
  checkButtons();
  
  if ((millis() - time_last) > 10*60000) {
    updateTime();
    time_last = millis();
  }
  
  if ((millis() - last_update) > update_interval) {
    setColor(color_state);
    displayTime();
    last_update = millis();
  }

}

// function that parses the RTC clock time, and outputs it correctly to the display

void updateTime() {
  DateTime now = getRTCTime();
  int minutes = now.minute();
  int hours   = now.hour() % 12; // if time is 12 or 24 --> 0, 0 == 12
  
  minutes_global = minutes;
  hours_global = hours;
  
  Serial.print(hours);
  Serial.println(minutes);
}

void displayTime() { 
  int minutes = minutes_global;
  int hours   = hours_global;
  Word first_word;
  Word second_word;
  bool b_about = true;
  bool b_past  = true;
  bool b_first_word = true;
  
  if (minutes >= 55) {
    b_first_word = false;
  } else if (minutes >= 45) {
    first_word = TenTo;
  } else if (minutes >= 35) {
    first_word = Twenty;
  } else if (minutes >= 25) {
    first_word = Half;
  } else if (minutes >= 15) {
    first_word = Twenty;
  } else if (minutes >= 5) {
    first_word = TenTo;
  } else {
    b_first_word = false;
  }

  if (minutes >= 35) {
    b_past = false;
    second_word = hour_words[(hours+1) % 12]; // to the next hour
  } else {
    second_word = hour_words[hours]; // past the current hour
  }

  // Now turn on the appropriate LEDS
  turnOffAll();
  turnOn(Yo);
  turnOn(Dude);
  turnOn(It);
  turnOn(Is);
  if ((b_past) & (b_first_word)) {
    turnOn(Past);
  } else if ((!b_past) & (b_first_word)) {
    turnOn(To);
  }
  if ((minutes % 10) != 0) {
    turnOn(About);
  }
  if (b_first_word) {
    turnOn(first_word);
  } 
  
  turnOn(second_word);
  FastLED.show();
}

DateTime getRTCTime() {
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    return false;
  } else {
    Serial.println("RTC online");
    DateTime now = rtc.now();
    // Line below is necessary to prevent issues between i2c and FastLED interrupts(?)
    TWCR = 0; // reset TwoWire Control Register to default, inactive state
    return now;
  }
  
}

void turnOn(Word this_word) {
  for (int j = 0; j < 6; j++) {
    if (this_word.cols[j] == -1) {
      return;
    }
    leds[letters[this_word.row][this_word.cols[j]]] = CHSV( gHue, gSat, gVal);
  }
}

void turnOff(Word this_word) {
  for (int j = 0; j < 6; j++) {
    if (this_word.cols[j] == -1) {
      return;
    }
    leds[letters[this_word.row][this_word.cols[j]]] = CRGB(0, 0, 0);
  }
}

void turnOffAll() {
  fill_solid( leds, NUM_LEDS, CRGB(0,0,0));
  //FastLED.show();
}

void changeColor() {
  
}

void testAll() {
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 11; j++) {
      leds[letters[i][j]] = CRGB(R, G, B);
      FastLED.show();
      delay(20);
      leds[letters[i][j]] = CRGB(0, 0, 0);
      
    }
  }
}


// designs
void stripeDownRainbow() {
  uint8_t hue = 0;
  uint8_t dHue = 255 / NUM_LEDS;
  
  for (int j = 0; j < 11; j++) {
    for(int i = 0; i < 9; i++) {
      hue += dHue;
      leds[letters[i][j]] = CHSV(hue, 200, 255);
      FastLED.show();
      delay(5);
    }
  }
}

void stripeDownRainbowOff() {
  for (int j = 0; j < 11; j++) {
    for(int i = 0; i < 9; i++) {
      leds[letters[i][j]] = CHSV(0, 0, 0);
      FastLED.show();
      delay(5);
    }
  }
}

void turnAllOnAtRandom() {
  uint8_t sequence[NUM_LEDS];
  for (int i = 0; i < NUM_LEDS; i++) {
    sequence[i] = i;
  }

  //shuffle the sequence
  for (int i = 0; i < NUM_LEDS; i++) {
    int n = random(0, NUM_LEDS);
    int temp = sequence[n];
    sequence[n] = sequence[i]; // swap i and n
    sequence[i] = temp; 
  }

  uint8_t dHue = 255 / NUM_LEDS;
  
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t hue = dHue * sequence[i];
    leds[sequence[i]] = CHSV(hue, 200, 255);
    FastLED.show();
    delay(5);
  }
}

void turnAllOffAtRandom() {
  uint8_t sequence[NUM_LEDS];
  for (int i = 0; i < NUM_LEDS; i++) {
    sequence[i] = i;
  }

  //shuffle the sequence
  for (int i = 0; i < NUM_LEDS; i++) {
    int n = random(0, NUM_LEDS);
    int temp = sequence[n];
    sequence[n] = sequence[i]; // swap i and n
    sequence[i] = temp; 
  }

  for (int i = 0; i < NUM_LEDS; i++) {
    if (!checkInSecretSequence(sequence[i])) {
      leds[sequence[i]] = CRGB(0,0,0);
    }
    FastLED.show();
    delay(5);
  }
}

bool checkInSecretSequence(int n) {
  for (int i = 0; i < 8; i++) {
    if (n == secret_sequence[i]) {
      return true;
    }
  }
  return false;
}

void secretMessage() {
  turnOffAll();
  for (int i = 0; i < 4; i++) {
    turnOn(secret_words[i]);
  }
  FastLED.show();
}

void checkButtons() {
  checkButton(0);
  checkButton(1);
  checkButton(2);
  checkButton(3);
}

void checkButton(int n) {
  int state = digitalRead(but_pins[n]);

  if (state != state_last[n]) {
    state_debounce[n] = millis();
  }

  if ((millis() - state_debounce[n]) > debounce_time) {
    if (state != but_state[n]) {
      but_state[n] = state;
      Serial.print("Button Activity: But ");
      Serial.println(n);
      if (state == HIGH) {
        if (n < 2) {
          set_time(n);
        }
        if (n == 2) {
          turnAllOnAtRandom(); //blocking
          turnAllOffAtRandom(); // also blocking
          delay(1000);
        }
        if (n == 3) {
          changeColorState();
        }
      }
    }
  }
  state_last[n] = state;
}

// Cycle through color states
void changeColorState() {
  color_state = (color_state + 1) % 11;
  setColor(color_state);

  // write color state to eeprom
  EEPROM.write(eepromAddr, color_state); // 1 byte
}


uint8_t getColorState() {
  uint8_t state =  EEPROM.read(eepromAddr);
  return (state % 11);
}

// cycle through colors
void setColor(uint8_t color_state) {
  gVal = 255;
  gSat = 210; 
  
  if (color_state == 10) {
    gHue = (gHue+1) % 255;
  }

  if (color_state <= 8) {
    gHue = 31 * color_state;
  }

  if (color_state == 9) {
    gHue = 0;
    gSat = 0;
    gVal = 255;
  }
}

void set_time(int n) {
  int del = (n == 1 ? 1 : -1);
  minutes_global += del * (5); // resolution of 5 minutes.
  if (minutes_global >=60) {
    minutes_global = 0;
    hours_global += 1;
  } else if (minutes_global < 0) {
    minutes_global = 55;
    hours_global -= 1;
  }
  hours_global = hours_global % 12;
}
