
// FASTLED to control the lights
#include <FastLED.h>

#define NUM_LEDS 24
#define DATA_PIN D7
CRGB leds[NUM_LEDS];
#define COLOR_ORDER GRB

// Top to bototm, left to right
unsigned int led_map[ 6 ][ 4 ] = { { 19, 18, 7, 6 }, { 20, 17, 8, 5}, { 21, 16, 9, 4 }, { 22, 15, 10, 3 }, { 23, 14, 11,  2 }, { 24, 13, 12, 1 } };
unsigned int colorIndex = 0;
unsigned int brightness = 255;


//Rainbow/Sunset settings
unsigned int color_step = 10; // number to increment the color index between each row, higher, more dramatic the gradient, lower, less gradient
unsigned long last_time = 0;  // ms, used for update frequency
unsigned long update_time = 1000;

CRGBPalette16 sunset_palette;

/*
const TProgmemPalette16 SunsetColors PROGMEM =
{
    CRGB(235, 235, 235), // 'white' is too bright compared to red and blue
    CRGB(238, 243, 195),
    CRGB(245, 244, 65),
    CRGB(251, 242, 43),
    CRGB(253, 223, 17),
    CRGB(253, 180, 0),
    CRGB(254, 167, 0),
    CRGB(255, 148, 0),
    CRGB(255, 100, 34),
    CRGB(255, 68, 100),
    CRGB(255, 21, 204),
    CRGB(255, 0, 255),
    CRGB(206, 0, 255),
    CRGB(121, 0, 255),
    CRGB(0, 0, 255),
    CRGB(0, 0, 255)
};
*/

void setup() {
  // put your setup code here, to run once:
  //FastLED setup
  FastLED.addLeds<WS2811, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);

  // Temporary correciton -- replace the original definition for perma-fix
  for (int row = 0; row < 6; row++) {
    for (int col = 0; col < 4; col++) {
      led_map[row][col] = led_map[row][col] - 1;
    }
  }

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
    CRGB(0, 0, 255)
    );
}

void loop() {
  // put your main code here, to run repeatedly:
  runSunColors();
  
}



void runSunColors() {

  
  unsigned long this_time = millis();

  if ((this_time - last_time) > update_time) {
    for (int row = 0; row < 6; row++) {
      for (int col = 0; col < 4; col++) {
        leds[led_map[row][col]] = ColorFromPalette( sunset_palette, (colorIndex + (5-row)*color_step) % 256, brightness, LINEARBLEND);
      }
    }
  
    colorIndex = colorIndex + 1 % 256;
  
    FastLED.show();
    last_time = this_time;
  }
}
