/*
 * Trying to phase this script out, it will do light and fan handling, and can be addressed directly at its IP address
 * Would like to transition to having a controller for the lights and fans separate, that talk to some web server either
 * running on a pi or a esp32 -- WIP
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "RTClib.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Bounce2.h>
#include <Solar.h> // stupid-simple class to get solar insolance

//ESP Web Server Library to host a web page
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

//For JSON encoding
#include <ArduinoJson.h>

// Inside the brackets, 200 is the RAM allocated to this document.
// Don't forget to change this value to match your requirement.
// Use arduinojson.org/v6/assistant to compute the capacity.
StaticJsonDocument<300> json_doc;

//RTC (real-time clock)
RTC_DS3231 rtc;

//Static IP address configuration
IPAddress staticIP(192, 168, 100, 47); //ESP static ip
IPAddress gateway(192, 168, 100, 1);   //IP Address of your WiFi Router (Gateway)
IPAddress subnet(255, 255, 0, 0);  //Subnet mask
IPAddress dns1(8, 8, 8, 8);  //DNS
IPAddress dns2(8, 8, 4, 4);  //DNS

String FAN_URL="http://192.168.100.250";

const char* deviceName = "viv_light1";

//OLED settings
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//On board LED Connected to GPIO2
#define white_warm_pin D4
#define white_cool_pin D3
#define fan_pin D5

// Input settings
#define MODE_BUTTON D7
Bounce bounce = Bounce();
unsigned int but_cnt = 0;

//SSID and Password of your WiFi router
const char* ssid = "526Beacon";
const char* password = "simasucks";

// Solar emulation settings
boolean solar_emulation = true;
Solar solar = Solar(0, 0); // lat, lon -- in radians

// Light fading settings
unsigned int slow_interval = 20;
unsigned int warm_level_cur = 0;
unsigned int cool_level_cur = 0;
unsigned int warm_level_set = 400;  // These are used as max values when in solar mode
unsigned int cool_level_set = 950; // These are used as max values when in solar mode
unsigned int warm_level_cur_solar = 0; // Current calculated solar values -- these are set in setSolarValues()
unsigned int cool_level_cur_solar = 0; // Current calculated solar values -- these are set in setSolarValues()
unsigned long time_last = 0;

// Thunderstorm settings
bool b_in_thunderstorm = false;
bool b_first_thunder = true; 
unsigned long t_thunder_start = 0;
unsigned long t_last_thunder_check = 0;
unsigned int thunder_duration = 60*20; // thunder storm duration, in seconds
float thunder_frac = 0.85; // fraction of current light level to dim to during thunderstrom
 // lightning values -- sequences are on-time, off-time, ... -- must be even, always end with an off time, must have trailing zero
 // C++ will pad with zeros
unsigned int lightning_timings[4][16] = { {50, 50, 100, 40, 30, 30}, {50, 100, 100, 30, 80, 20, 100, 100, 50}, {100, 20, 100, 50}, {40, 40, 40, 100, 150, 30, 100, 50, 70, 50, 80, 10} };  
      
// General light/timer settings
boolean timer_mode = true;
boolean timer_mode_stored = timer_mode;
boolean manual_on = false;
unsigned int hour1 = 7; // simple for now
unsigned int hour2 = 12 + 7;
unsigned long time_last_checked = 0;
unsigned long time_last_update = 0;


// Fan settings
boolean fan_timer = true;
boolean fan_off = false;
boolean b_fans_only_day = true;
unsigned int fan_level_day = 1024;
unsigned int fan_level_set = 0;
unsigned int fan_duration = 4; // minutes, duration fan is on each time it is triggered on
unsigned int fan_interval  = 10; // minutes, how often the fan is turned on
unsigned int fan_level_night = 0; // Don't PWM plain old DC fans, it doesn't work right

// Prototypes
void setLightLevelSmoothly();
void handleLightTimer();
void setLights();
void turnOffOnboardLEDs();

//Declare a global object variable from the ESP8266WebServer class.
ESP8266WebServer server(80); //Server on port 80
HTTPClient http;

//==============================================================
//                  SETUP
//==============================================================
void setup(void){
  Serial.begin(115200);
  // Button setup
  bounce.attach(MODE_BUTTON);
  bounce.interval(25);

  // Setup solar emulation as needed
  solar.setDaylightParameters(hour1, hour2);
  
  // Initialize display
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally

  Serial.println(F("Setting up OLED"));
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  } else {
    
  }

  Serial.println(F("Setting up RTC"));
  // RTC setup
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    //Serial.flush();
    //abort();
  }

  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  

  // WIFI setup
  WiFi.hostname(deviceName);      // DHCP Hostname (useful for finding device for static lease)
  if (!WiFi.config(staticIP, subnet, gateway, dns1, dns2)) {
    Serial.println("Static IP failed to configure");
  }
  WiFi.begin(ssid, password);

  WiFi.mode(WIFI_STA); //WiFi mode station (connect to wifi router only
  
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP

  // Server setup
  server.on("/lightlevel", handleLightLevel); //as Per  <a href="ledOn">, Subroutine to be called
  server.on("/request", handleRequest); // Handle a request from the server
  
  server.begin();                  //Start server
  Serial.println("HTTP server started");

  setFan(false);
  updateDisplay();
  turnOffOnboardLEDs();
}
//==============================================================
//                     LOOP
//==============================================================
void loop(void){
  bounce.update();
  handleButton();
  
  server.handleClient();          // Handle client requests
  //setLightLevelSmoothly();        // Fade into new light level (if not in timer mode)
  thunderStorm();                // Thunderstorm sequence, has to be activated
  handleLightTimer();             // Set light level if in timer mode
  perodicUpdates();               // Anything that needs to be updated periodically goes in ehre
}


//===============================================================
// This bit of code executes frequently and checks to see if a button was pressed
//===============================================================


void handleButton() {
  if ( bounce.fell() ) {
    Serial.println(F("Button was pressed!"));
    but_cnt = (but_cnt + 1) % 5;

   
    if (but_cnt == 0) {
      timer_mode = timer_mode_stored;
      manual_on = false;
      if (timer_mode) {
        setLightsToTimeOfDay();
      } else {
        setLights();
      }
    } 
    if (but_cnt == 1) {
      timer_mode_stored = timer_mode;
      timer_mode = false;
      setLights(but_cnt*100, but_cnt*100);
    } 
    if (but_cnt > 1) {
      setLights(2*but_cnt*100, 2*but_cnt*100);
    }

     updateDisplay();
  }
}

//===============================================================
// This routine is executed when you open its IP in browser
//===============================================================

void handleLightLevel() { 
  Serial.println("Processing Light Level Data...");
 for (int i = 0; i < server.args(); i++) {
   Serial.print(String(i) + " ");  //print id
   Serial.print("\"" + String(server.argName(i)) + "\" ");  //print name
   Serial.println("\"" + String(server.arg(i)) + "\"");  //print value
   if (server.argName(i) == "warm") {
    warm_level_set = server.arg(i).toInt();
   }
   if (server.argName(i) == "cool") {
    cool_level_set = server.arg(i).toInt();
   }

   if (server.argName(i) == "hour1") {
    hour1 = server.arg(i).toInt();
    solar.setDaylightParameters(hour1, hour2);
   }

   if (server.argName(i) == "hour2") {
    hour2 = server.arg(i).toInt();
    solar.setDaylightParameters(hour1, hour2);
   }

  if (server.argName(i) == "thunder") {
    if (server.arg(i) == "on") {
      b_in_thunderstorm = true;
    } else {
      b_in_thunderstorm = false;
    }
  }

  if (server.argName(i) == "lightning") {
    lightningFlash(random(4));
  }
  
  if (server.argName(i) == "solar") {
    if (server.arg(i) == "on") {
      solar_emulation = true;
    } else {
      solar_emulation = false;
    }
    setLightsToTimeOfDay();
   }
   
   if (server.argName(i) == "fan") {
    fan_level_set = server.arg(i).toInt();
   }


   if (server.argName(i) == "fanmode") {
    bool fan_on = false;
    if (server.arg(i) == "timer") {
      fan_timer = true;
    } else if (server.arg(i) == "off") {
      fan_on = false;
      fan_timer = false;
      setFan(fan_on);
    } else if (server.arg(i) == "on") {
      fan_timer = false;
      fan_on = true;
      setFan(fan_on);
    }
   }

   if (server.argName(i) == "faninterval") {
      if ((server.arg(i).toInt() > 0) && (server.arg(i).toInt() < 60)){
        fan_interval = server.arg(i).toInt();
      }
   }

   if (server.argName(i) == "fanduration") {
      if ((server.arg(i).toInt() > 0) && (server.arg(i).toInt() < 60)){
        fan_duration = server.arg(i).toInt();
      }
   }

   if (server.argName(i) == "mode") {
    if (server.arg(i) == "timer") {
      timer_mode = true;
      manual_on = false;
      Serial.println("Switching mode to TIMER");
      setLightsToTimeOfDay();
    } else if (server.arg(i) == "on") {
      manual_on = true;
      timer_mode = false;
    } else {
      timer_mode = false;
      manual_on = false;
      Serial.println("Turning TIMER mode off");
    }
   }
   
   
   setLights();
   updateDisplay();
   
 }
 server.send(200, "text/html", "Got it."); //Send ADC value only to client ajax request
}

// Handle a GET request from the server
// server might ask things like "are the lights on? at what level?"
//   what's the temp?
//   what's the humidity?
//   etc
void handleRequest() {
  String return_str = "";
  
  for (int i = 0; i < server.args(); i++) {
   Serial.print(String(i) + " ");  //print id
   Serial.print("\"" + String(server.argName(i)) + "\" ");  //print name
   Serial.println("\"" + String(server.arg(i)) + "\"");  //print value

     // Asking what the light levels are
    if (server.argName(i) == "config") {
      json_doc["timer"] = timer_mode ? "on" : "off";
      json_doc["cool"] = cool_level_set;
      json_doc["warm"] = warm_level_set;
      json_doc["solar"] = solar_emulation ? "on" : "off";
      json_doc["fan"] = fan_off ? "off" : "on";
      json_doc["manualon"] = manual_on ? "on" : "off";
      json_doc["fanint"] = fan_interval;
      json_doc["fandur"] = fan_duration;
      json_doc["hour1"] = hour1;
      json_doc["hour2"] = hour2;
  
      Serial.println("Sending this data:");
      serializeJson(json_doc, Serial);
      Serial.println();

      // print the data to the return string so we can send it (below)
      serializeJson(json_doc, return_str);
  
    }


   // 
  }
  server.send(200, "text/html", return_str); //Send ADC value only to client ajax request
}

// Always running, to ramp to set level
void setLightLevelSmoothly() {
  if (warm_level_cur != warm_level_set) {
    if ((millis() - time_last) > slow_interval) {
      warm_level_cur < warm_level_set ? warm_level_cur++ : warm_level_cur--;
      analogWrite(white_warm_pin, warm_level_cur);
      time_last = millis();
    }
  }
}

// Set light level as needed
void handleLightTimer() {
  if ((millis() - time_last_checked) < 5000) {
    return;
  }

  time_last_checked = millis();

  if (!timer_mode) {
    return;
  }

  setSolarValues();
  setLightsToTimeOfDay();
}

// Sets global warm and cool levels based on max set values, and time of day
void setSolarValues() {
      DateTime now = rtc.now();

      float hour_frac = float(now.hour()) + now.minute()/60.0;
      float I = solar.getFromSine(hour_frac);

      warm_level_cur_solar = warm_level_set*I;
      cool_level_cur_solar = cool_level_set*I;
}

void setLightsToTimeOfDay() {
  DateTime now = rtc.now();
  setLightsToTimeOfDay(now);
}

// Set lights to the time of day
void setLightsToTimeOfDay(DateTime now) {

  if (b_in_thunderstorm) { return; } // Don't set lights to current time of day if we're in a thunderstorm!
  
  if ( (now.hour() >= hour1) && (now.hour() < hour2) ) {
    // will setup to follow an intensity curve in the future
    Serial.println("Inside hour bounds, lights ON");
    
    if (solar_emulation) {
      //float hour_frac = float(now.hour()) + now.minute()/60.0;
      //float I = solar.getInsolance(hour_frac); // Technically accurate, but has some issues
      //float I = solar.getMoreSquareSine(hour_frac); // More bright light in the afternoon (gets to the peak max faster)
      //float I = solar.getFromSine(hour_frac);
      
      warm_level_cur = warm_level_cur_solar;
      cool_level_cur = cool_level_cur_solar;
      
      analogWrite(white_warm_pin, int(warm_level_cur));
      analogWrite(white_cool_pin, int(cool_level_cur));
    } else { // simple mode
      analogWrite(white_cool_pin, cool_level_set);
      analogWrite(white_warm_pin, warm_level_set);
    }
  } else {
    Serial.println("Outside hour bounds, lights OFF");
    analogWrite(white_cool_pin, 0);
    analogWrite(white_warm_pin, 0);
  }
}


// THUNDERSTORM! 
// On the light side of things, the program can be commanded to exceute a thunderstorm, 
// where lights will dim over a duration, and to a fraction of the current solar value
// and then slowly return to the correct current solar vlaue

void randomLightning() {
    unsigned int rand_val = random(100);
    if (rand_val == 15) {
      unsigned int rand_row = random(4);
      
      lightningFlash(rand_row);
    }
}

void lightningFlash(unsigned int row) {
      Serial.println('Lightning!');
      unsigned int _cool = cool_level_cur;
      unsigned int _warm = warm_level_cur;

      // timings alternate between on/off times, starting with on
      unsigned int cnt = 0;
      unsigned int this_delay = lightning_timings[row][cnt];

      boolean b_on = true;
      while (this_delay != 0) {
        if (b_on) {
          setLights(1023, 0);
          delay(this_delay);
        } else {
          setLights(_cool, _warm);
          delay(this_delay);
        }
        b_on = !b_on;
        cnt += 1;
        this_delay = lightning_timings[row][cnt];
      }

      setLights(_cool, _warm); // Just in case, reset the lights back to where they were
}

void thunderStorm() {

  if (b_in_thunderstorm) {
    if (b_first_thunder) {
      //thunder_interval = (thunder_duration/2/max((warm_thunder_start - warm_thunder_min, cool_thunder_start - cool_thunder_min))); // seconds between updates
      b_first_thunder = false; 
      t_thunder_start = millis();
    }

    unsigned long t_now = millis();
    if ( (t_now - t_last_thunder_check) > 500) { // based on a 10 minute target: 60*10/1000 = 600ms
        t_last_thunder_check = t_now;
        
        // using a sine profile, that we'll subtract from what the current solar values should be
       // Serial.println(t_now - t_thunder_start);
        float frac_into_storm = float(t_now - t_thunder_start) / float(thunder_duration * 1000);

        if (frac_into_storm > 1) { // We're done with the thunder storm, so bail out
          b_in_thunderstorm = false;
          return;
        }

        if ((frac_into_storm > 0.4) && (frac_into_storm < 0.65)) {
          randomLightning();
        }
        
        float sine_frac = (sin( frac_into_storm * 2*PI - PI/2 ) + 1) / 2;
        int warm_new = int( warm_level_cur_solar - (sine_frac * warm_level_cur_solar) * thunder_frac);
        int cool_new = int( cool_level_cur_solar - (sine_frac * cool_level_cur_solar) * thunder_frac);

        Serial.print("Frac into storm: ");
        Serial.print(frac_into_storm);
        Serial.print(", warm_new: ");
        Serial.print(warm_new);
        Serial.print(", cool_new: ");
        Serial.println(cool_new);
        
        setLights(cool_new, warm_new);
    }

  } else {
    b_first_thunder = true;
  }
}

// Periodic update catch-all
void perodicUpdates() {
  if ((millis() - time_last_update) > 1000) {
    time_last_update = millis();
    updateDisplay();
    periodicFans();
  }
}

// Update display
void updateDisplay() {
  
  display.clearDisplay();

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  // Not all the characters will fit on the display. This is normal.
  // Library will draw what it can and the rest will be clipped.
  display.print(F("Cool:"));
  display.print(" ");
  display.print(cool_level_cur);
  display.print(F("/")); 
  display.println(cool_level_set);
  display.print(F("Warm:"));
  display.print(" ");
  display.print(warm_level_cur);
  display.print(F("/")); 
  display.println(warm_level_set);

  display.print(F("On: "));
  display.print(hour1);
  display.print(F(" Off: "));
  display.print(hour2);

  display.print(" ");
  
  if (timer_mode & solar_emulation) {
    display.println(F("SOLAR"));
  } else if (timer_mode) {
    display.println(F("ON"));
  } else {
    display.println(F("OFF"));
  }

  // Print out what we think the time is (good for debugging)
  DateTime now = rtc.now();
  display.print(now.hour());
  display.print(':');
  display.print(now.minute());
  display.print(':');
  display.println(now.second());
  
  // Print out this IP
  display.println(WiFi.localIP());

  display.display();
}

// Set lights
void setLights() { 
  setLights(true);
}

void setLights(unsigned int cool_level, unsigned int warm_level) {
    cool_level_cur = cool_level;
    warm_level_cur = warm_level;
    analogWrite(white_cool_pin, cool_level);
    analogWrite(white_warm_pin, warm_level);
}

void setLights(boolean lights_on) {
  Serial.println("Setting light level inside setLights()");

  if (!lights_on) {
    warm_level_cur = 0;
    cool_level_cur = 0;
    
    analogWrite(white_cool_pin, 0);
    analogWrite(white_warm_pin, 0);
    return;
  }
  // otherwise:
  if (!timer_mode && manual_on) {
    warm_level_cur = warm_level_set;
    cool_level_cur = cool_level_set;
    
    analogWrite(white_cool_pin, cool_level_set);
    analogWrite(white_warm_pin, warm_level_set);
  }

  if (!timer_mode && !manual_on) {
    setLights(false);
  }
}

// Set Fan
void periodicFans() {
  DateTime now = rtc.now();
  // Turn on fans first 10 minutes of every hour, during the day
  if (( (now.hour() < hour1) || (now.hour() > hour2) ) && b_fans_only_day) {
    setFan(false);
    return;
  }
  
  if (fan_timer && (fan_level_set == 0) && ((now.minute() % fan_interval) < fan_duration)) {
      Serial.println(F("Fan timer on. Fans seem to be OFF. First 10 mintues of hour, turning fans on!"));
      setFan(true);
  } else if (fan_timer && (fan_level_set > 0) && ((now.minute() % fan_interval) > fan_duration)) {
    Serial.println(F("Fan timer on. Fans seem to be ON. Outside first 10 mintues of hour, turning fans off!"));
     setFan(false);
  }
}

void setFan() {
    setFan(true);
}

void setFan(boolean fan_on) {
  if (fan_on && (fan_level_set == 1024)) {
    return;
  }

  if (!fan_on && (fan_level_set == 0)) {
    return;
  }
  
  Serial.println("Setting fan level in setFan()");
  Serial.print("communicating to fan unit at: ");
  Serial.println(FAN_URL);

  if (fan_on) {
    fan_level_set = 1024;
  } else {
    fan_level_set = 0;
  }
  String fanmode_str = fan_on ? "on" : "off"; 
  String server_request = FAN_URL + "/fan?fanmode=" + fanmode_str;
  Serial.println("Sending this request: " + server_request);
  http.begin(server_request.c_str());

  int http_response_code = http.GET();

  if (http_response_code == 200) {
    Serial.println("Good response");
  } else {
    Serial.println("Bad response");
  }
  http.end();
}


// Turn off ESP8266 / NodeMCU LEDs
void turnOffOnboardLEDs() {
  digitalWrite(2, HIGH);
  digitalWrite(LED_BUILTIN, HIGH);
}
