  #include "FastLED.h"

  #define DATA_PIN D4

  CRGBArray<14> leds;

DEFINE_GRADIENT_PALETTE( colormap ) {
  0,   255,  0, 150,   //purple
100,   0,  0,  255,   //blue
180,   255, 180, 0,   // yellow /orange
255,   255,0,  0};   // red

CRGBPalette16 tempPalette = colormap;

void setup() { FastLED.addLeds<WS2811, DATA_PIN, BRG>(leds, 14); }
void loop() { 
    static uint8_t hue=0; 
    for (int i = 0; i < 255; i ++) {
      leds.fill_rainbow(hue++); 
      //leds.fadeLightBy(128);
      FastLED.delay(50);
    }
}
