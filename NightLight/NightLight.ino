// Purpose:
//  Backlight for a "night light" sort of project for Katie's Birthday
//  Idea is to have the lights mimic a sky backdrop, particularly sunset/morning
//  Lights will turn on/off automatically based on ambient light
//  Whole thing can be turned off by rotating 90 or 180 degrees

// FASTLED to control the lights
#include <FastLED.h>

#define NUM_LEDS 24
#define DATA_PIN D7
CRGB leds[NUM_LEDS];
#define COLOR_ORDER GRB

// LED layout, Top to bototm, left to right
unsigned int led_map[ 6 ][ 4 ] = { { 18, 17, 6, 5 }, { 19, 16, 7, 4}, { 20, 15, 8, 3 }, { 21, 14, 9, 2 }, { 22, 13, 10,  1 }, { 23, 12, 11, 0 } };
int colorIndex = 0;
unsigned int brightness = 100;
int color_direction = 1;

CRGBPalette16 sunset_palette;

//Rainbow/Sunset settings
unsigned int color_step = 8; // number to increment the color index between each row, higher, more dramatic the gradient, lower, less gradient
unsigned long last_time = 0;  // ms, used for update frequency
unsigned long update_time = 3000;

// Light sensor to measure ambient lgiht
#include "Wire.h"
#include "Max44009.h"
Max44009 myLux(0x4A);
bool b_enable_light_sensor = true;

// Global settings
float light_threshold = 5;

// Mercury switch "setup"
#define SWITCH_PIN D5


// ********************
void setup() {
  Serial.begin(115200);
  
  //FastLED setup
  FastLED.addLeds<WS2811, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);

  //Light Sensor setup
  Wire.begin();
  Wire.setClock(100000);

  //Switch setup
  pinMode(SWITCH_PIN, INPUT);

  turnLightsOff();

  // Testing a setup of a color pallete
  sunset_palette = CRGBPalette16(
    CRGB(235, 235, 235), // 'white' is too bright compared to red and blue
    CRGB(238, 243, 195),
    CRGB(245, 244, 65),
    CRGB(251, 242, 43),
    CRGB(253, 223, 17),
    CRGB(253, 134, 0),
    CRGB(254, 76, 0),
    CRGB(255, 50, 0),
    CRGB(255, 42, 30),
    CRGB(255, 22, 108),
    CRGB(255, 10, 204),
    CRGB(255, 0, 255),
    CRGB(206, 0, 255),
    CRGB(121, 0, 255),
    CRGB(0, 0, 255),
    CRGB(0, 0, 0)
    );
  
}

void loop() {

  // Check if mercury switch is ON
 // delay(500);
  
  if (checkSwitchOn()) {  
    delay(100);
    if (checkLightsOn()) {
      turnLightsOff();
    } else { 
      runSunColors();
    }
  } else {
    turnLightsOff();
  }
}


// ********************
void turnLightsOn() {
  fill_solid( leds, NUM_LEDS, CRGB(50,0,200));
  FastLED.show();  
}

void turnLightsOff() {
  fill_solid( leds, NUM_LEDS, CRGB(0,0,0));
  FastLED.show();  
}

void runSunColors() {
  
  unsigned long this_time = millis();

  if ((this_time - last_time) > update_time) {

    
    for (int row = 0; row < 6; row++) {
      for (int col = 0; col < 4; col++) {

        leds[led_map[row][col]] = ColorFromPalette( sunset_palette, (colorIndex + (5-row)*color_step), brightness, LINEARBLEND);
        /*
        if (color_direction > 0) {
          unsigned int this_color_index = (colorIndex + (5-row)*color_step) < 240 ? (colorIndex + (5-row)*color_step) : 240;
          Serial.println(this_color_index);
          leds[led_map[row][col]] = ColorFromPalette( sunset_palette, this_color_index, brightness, LINEARBLEND);
        } else {
          unsigned int this_color_index = (colorIndex - (row)*color_step) >= 0 ? (colorIndex - (row)*color_step) : 0;
          //Serial.println(this_color_index);
          leds[led_map[row][col]] = ColorFromPalette( sunset_palette, this_color_index, brightness, LINEARBLEND);
        }
        */
      }
    }

    colorIndex = (colorIndex) + 1 % 256;
    //colorIndex = colorIndex + color_direction;

    // logic to switch the direction of the colormap so when we get to the end, we start counting back to the beginning
    //Serial.println(colorIndex);

    /*
    if (colorIndex > 240 ) {
      color_direction = -1;
      colorIndex == 240;
    } else if (colorIndex < 0 ) {
      color_direction = 1;
      colorIndex == 0;
    } 
    */
  
    FastLED.show();
    last_time = this_time;
  }
}

bool checkSwitchOn() {
  if ( digitalRead(SWITCH_PIN) == HIGH) { // switch is disconnected from ground
    //Serial.println("Switch is off");
    return false; // Lights should be off
  } else {
    //Serial.println("Switch is on");
    return true; // Lights should be on
  }
}

bool checkLightsOn() {
  if (!b_enable_light_sensor) {
    return true; // not using the light sensor, then just return true
  }
  
  if (getAmbientLight() > light_threshold) {
    return true;
  } else {
    return false;
  }
}

float getAmbientLight() {
  float lux = 0;
  lux = myLux.getLux();
  //Serial.println(lux);
  return lux;
}
