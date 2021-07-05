/*
V2 Plans:
- Switch to ESP32 based NodeMCU
- Control all vivarium lights (3x currently)
- Status to multiple dispalys 
- Synchronize lightning
- Basic thermostating (disable/dim lights if a temperature is exceeded -- might offload this onto RaspberryPi though, since it gets the temperature data)
- 
*/

#include <WiFi.h>
#include <HTTPClient.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include "RTClib.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Bounce2.h>
#include <Solar.h> // stupid-simple class to get solar insolance

// Because the ESP32 doesn't have an analogWrite function
#include <analogWrite.h>

//For JSON encoding
#include <ArduinoJson.h>

// Inside the brackets, 200 is the RAM allocated to this document.
// Don't forget to change this value to match your requirement.
// Use arduinojson.org/v6/assistant to compute the capacity.
StaticJsonDocument<300> json_doc;

//RTC (real-time clock)
RTC_DS3231 rtc;

//Static IP address configuration
IPAddress staticIP(192, 168, 100, 45); //ESP static ip
IPAddress gateway(192, 168, 100, 1);   //IP Address of your WiFi Router (Gateway)
IPAddress subnet(255, 255, 0, 0);  //Subnet mask
IPAddress dns1(8, 8, 8, 8);  //DNS
IPAddress dns2(8, 8, 4, 4);  //DNS

//String FAN_URL="http://192.168.100.250";
// Config (just a bunch of defines for device addresses)
#include <Vivarium2Common.h> // FLASK_ADDR, FAN_URL

const char* deviceName = "viv_light1";

//OLED settings
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//On board LED Connected to GPIO2
int white_warm_pin[] = {16, 15, 14};
int white_cool_pin[] = {20, 18, 17};
#define fan_pin D4

// Input settings
#define MODE_BUTTON 7
Bounce bounce = Bounce();
unsigned int but_cnt = 0;

// All the config/variable definitions:
#include "config.h"

// Prototypes
void setLights();
void setLights(unsigned int cool_level[], unsigned int warm_level[]);
void setLights(boolean lights_on);

// EEPROM stuff
#include <EEPROM.h>
#include "eeprom_functions.h"

// Weather FX
#include "weather_fx.h"

//Declare a global object variable from the ESP8266WebServer class.
WebServer server(80); //Server on port 80
HTTPClient http;

//==============================================================
//                  SETUP
//==============================================================
void setup(void){
  Serial.begin(115200);

  // EEPROM setup
  int eeprom_size = sizeof(int)*12 + sizeof(int) + 32;
  if (eeprom_size <= 4096) {
    EEPROM.begin(eeprom_size);
    // to clear memory:
    if (b_clear_eeprom) {
      for (int i = 0; i < eeprom_size; i++) {
        EEPROM.write(i, 0);
      }
      EEPROM.commit();
    }
    Serial.print("EEPROM reserved: ");
    Serial.println(eeprom_size);
  } else {
    Serial.println("Not enough eeprom space available to store data"); 
  }

  readFromEEPROM(); // Read in variables/settings from EEPROM, if any
  
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

  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  

  // WIFI setup
  WiFi.setHostname(deviceName);      // DHCP Hostname (useful for finding device for static lease)
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
  server.on("/lightlevel", handleIncomingData); //as Per  <a href="ledOn">, Subroutine to be called
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
  thunderStorm();                // Thunderstorm sequence, has to be activated externally
  handleLightTimer();             // Set light level if in timer mode
  perodicUpdates();               // Anything that needs to be updated periodically goes in ehre
}


//===============================================================
// This bit of code executes frequently and checks to see if a button was pressed
//===============================================================


void handleButton() {
  /* Temporarilty disabled
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
      setLights({but_cnt*100,, but_cnt*100);
    } 
    if (but_cnt > 1) {
      setLights(2*but_cnt*100, 2*but_cnt*100);
    }

     updateDisplay();
  }
  */
}

//===============================================================
// This routine is executed when /lightlevel is passed to its IP
//===============================================================

void handleIncomingData() { 
  Serial.println("Processing Light Level Data...");
 for (int i = 0; i < server.args(); i++) {
   Serial.print(String(i) + " ");  //print id
   Serial.print("\"" + String(server.argName(i)) + "\" ");  //print name
   Serial.println("\"" + String(server.arg(i)) + "\"");  //print value


   // LIGHT SETTINGS
   unsigned int light_id = 0;
   if (server.argName(i) == "id") {
    light_id = server.arg(i).toInt();
   }

   // Must now specify a light ID (1, 2, or 3) to address
   if ((light_id != 0) && (light_id <= n_lights)) {
     if (server.argName(i) == "warm") {
      warm_level_set[light_id-1] = server.arg(i).toInt();
     }
 
     if (server.argName(i) == "cool") {
      cool_level_set[light_id-1] = server.arg(i).toInt();
     }
   } 


   if (server.argName(i) == "hour1") {
    hour1 = server.arg(i).toInt();
    solar.setDaylightParameters(hour1, hour2);
   }

   if (server.argName(i) == "hour2") {
    hour2 = server.arg(i).toInt();
    solar.setDaylightParameters(hour1, hour2);
   }

  // STORM SETTINGS
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


  // SOLAR SETTINGS
  if (server.argName(i) == "solar") {
    if (server.arg(i) == "on") {
      solar_emulation = true;
    } else {
      solar_emulation = false;
    }
    setLightsToTimeOfDay();
   }


   // FAN SETTINGS
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

   if (server.argName(i) == "fanonlyday") {
    if (server.arg(i) == "true") {
       b_fans_only_day = true;
    } else if (server.arg(i) == "false") {
      b_fans_only_day = false;
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
   
   writeToEEPROM(); // Store the new values (if any) to the EEPROM
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
      json_doc["cool"] = cool_level_set[0];
      json_doc["warm"] = warm_level_set[0];
      json_doc["solar"] = solar_emulation ? "on" : "off";
      json_doc["fan"] = fan_off ? "off" : "on";
      json_doc["manualon"] = manual_on ? "on" : "off";
      json_doc["fanint"] = fan_interval;
      json_doc["fandur"] = fan_duration;
      json_doc["hour1"] = hour1;
      json_doc["hour2"] = hour2;
      
      JsonArray cool_levels = json_doc.createNestedArray("cool_levels");
      for (int i = 0; i < n_lights; i++) {
        cool_levels.add(cool_level_set[i]);
      }

      JsonArray warm_levels = json_doc.createNestedArray("warm_levels");
      for (int i = 0; i < n_lights; i++) {
        warm_levels.add(warm_levels[i]);
      }
  
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

      for (int i = 0; i < n_lights; i++) {
        warm_level_cur_solar[i] = warm_level_set[i]*I;
        cool_level_cur_solar[i] = cool_level_set[i]*I;
      }
}

void setLightsToTimeOfDay() {
  DateTime now = rtc.now();
  setLightsToTimeOfDay(now);
}

// Set lights to the time of day
void setLightsToTimeOfDay(DateTime now) {

  if (b_in_thunderstorm) { return; } // Don't set lights to current time of day if we're in a thunderstorm!

  for (int i = 0; i < n_lights; i++) {
    if ( (now.hour() >= hour1) && (now.hour() < hour2) ) {
    // will setup to follow an intensity curve in the future
    Serial.println("Inside hour bounds, lights ON");

    
    if (solar_emulation) {
        //float hour_frac = float(now.hour()) + now.minute()/60.0;
        //float I = solar.getInsolance(hour_frac); // Technically accurate, but has some issues
        //float I = solar.getMoreSquareSine(hour_frac); // More bright light in the afternoon (gets to the peak max faster)
        //float I = solar.getFromSine(hour_frac);
        
        warm_level_cur[i] = warm_level_cur_solar[i];
        cool_level_cur[i] = cool_level_cur_solar[i];
        
        analogWrite(white_warm_pin[i], int(warm_level_cur[i]), 1023);
        analogWrite(white_cool_pin[i], int(cool_level_cur[i]), 1023);
      } else { // simple mode
        analogWrite(white_cool_pin[i], cool_level_set[i], 1023);
        analogWrite(white_warm_pin[i], warm_level_set[i], 1023);
      }
    } else {
      Serial.println("Outside hour bounds, lights OFF");
      analogWrite(white_cool_pin[i], 0, 1023);
      analogWrite(white_warm_pin[i], 0, 1023);
    }
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
  display.print(cool_level_cur[0]); // COME BACK TO THIS
  display.print(F("/")); 
  display.println(cool_level_set[0]);
  display.print(F("Warm:"));
  display.print(" ");
  display.print(warm_level_cur[0]);
  display.print(F("/")); 
  display.println(warm_level_set[0]);

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

 
  DateTime now = rtc.now();
  
  
  if ((now.second() % 5) == 0) {
    b_IP_time_flip_switch = !b_IP_time_flip_switch;
  }

  if (b_IP_time_flip_switch) {
    // Print out this IP
    display.println(WiFi.localIP());
  } else {
    // Print out what we think the time is (good for debugging)
    display.print(now.hour());
    display.print(':');
    display.print(now.minute());
    display.print(':');
    display.println(now.second());
  }

  display.display();
}

// Set lights
void setLights() { 
  setLights(true);
}

void setLights(unsigned int cool_level[], unsigned int warm_level[]) {
  for (int i = 0; i < n_lights; i++) {
    cool_level_cur[i] = cool_level[i];
    warm_level_cur[i] = warm_level[i];
    analogWrite(white_cool_pin[i], cool_level[i], 1023);
    analogWrite(white_warm_pin[i], warm_level[i], 1023);
  }
}

void setLights(boolean lights_on) {
  Serial.println("Setting light level inside setLights()");

  for (int i = 0;  i++; i < n_lights) {
    if (!lights_on) {
      warm_level_cur[i] = 0;
      cool_level_cur[i] = 0;
      
      analogWrite(white_cool_pin[i], 0, 1023);
      analogWrite(white_warm_pin[i], 0, 1023);
      return;
    }
    // otherwise:
    if (!timer_mode && manual_on) {
      warm_level_cur[i] = warm_level_set[i];
      cool_level_cur[i] = cool_level_set[i];
      
      analogWrite(white_cool_pin[i], cool_level_set[i], 1023);
      analogWrite(white_warm_pin[i], warm_level_set[i], 1023);
    }
  }

  if (!timer_mode && !manual_on) {
    setLights(false);
  }
}

// Set Fan
void periodicFans() {
  DateTime now = rtc.now();
  // Turn on fans first X minutes every Y minutes, during the day (currently)
  if (( (now.hour() < hour1) || (now.hour() > hour2) ) && b_fans_only_day) {
    setFan(false);
    return;
  }
  
  if (fan_timer && (fan_level_set == 0) && ((now.minute() % fan_interval) < fan_duration)) {
      Serial.println(F("Fan timer on. Fans seem to be OFF. Inside of fan duration, turning fans on!"));
      setFan(true);
  } else if (fan_timer && (fan_level_set > 0) && ((now.minute() % fan_interval) >= fan_duration)) {
    Serial.println(F("Fan timer on. Fans seem to be ON. Outside of fanduration, turning fans off!"));
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
  String server_request = String(FAN_URL) + "/fan?fanmode=" + fanmode_str;
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
  /*
  digitalWrite(2, HIGH);
  digitalWrite(LED_BUILTIN, HIGH);
  */
}
