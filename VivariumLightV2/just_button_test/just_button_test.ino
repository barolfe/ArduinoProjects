/*
 * Trying to phase this script out, it will do light and fan handling, and can be addressed directly at its IP address
 * Would like to transition to having a controller for the lights and fans separate, that talk to some web server either
 * running on a pi or a esp32 -- WIP
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Bounce2.h>

// Input settings
#define MODE_BUTTON 14
Bounce bounce = Bounce();
unsigned int but_cnt = 0;
//==============================================================
//                  SETUP
//==============================================================
void setup(void){
  Serial.begin(115200);
  // Button setup
  pinMode(MODE_BUTTON, INPUT_PULLUP);

 // bounce.attach(MODE_BUTTON);
 // bounce.interval(25);
  delay(2000);
  if (digitalRead(MODE_BUTTON) == HIGH) {
    Serial.println("Button is high, waiting 2 seconds, press it!");
  }
  delay(2000);
  if (digitalRead(MODE_BUTTON) == LOW) {
    Serial.println("Button is low, good jerb");
  } else {
    Serial.println("Uh oh");
  }

}
//==============================================================
//                     LOOP
//==============================================================
void loop(void){
 // bounce.update();
 // handleButton();
  
}


//===============================================================
// This bit of code executes frequently and checks to see if a button was pressed
//===============================================================


void handleButton() {
  if ( bounce.fell() ) {
    Serial.println(F("Button was pressed!"));
    but_cnt = (but_cnt + 1) % 5;
 /*   
    if (but_cnt == 0) {
      timer_mode = timer_mode_stored;
      manual_on = false;
      setLights();
    } 
    if (but_cnt == 1) {
      timer_mode_stored = timer_mode;
      timer_mode = false;
      setLights(but_cnt*100, but_cnt*100);
    } 
    if (but_cnt > 1) {
      setLights(2*but_cnt*100, 2*but_cnt*100);
    }
*/
  }
}
