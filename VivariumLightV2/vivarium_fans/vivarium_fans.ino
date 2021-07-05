/*

*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "RTClib.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Bounce2.h>

//ESP Web Server Library to host a web page
#include <ESP8266WebServer.h>


//Static IP address configuration
IPAddress staticIP(192, 168, 100, 123); //ESP static ip
IPAddress gateway(192, 168, 100, 1);   //IP Address of your WiFi Router (Gateway)
IPAddress subnet(255, 255, 255, 0);  //Subnet mask
IPAddress dns1(8, 8, 8, 8);  //DNS
IPAddress dns2(8, 8, 4, 4);  //DNS


const char* deviceName = "viv_fans";

//OLED settings
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//On board LED Connected to GPIO2
#define fan_pin D5

// Input settings
#define MODE_BUTTON D7
Bounce bounce = Bounce();
unsigned int but_cnt = 0;

//SSID and Password of your WiFi router
const char* ssid = "526Beacon";
const char* password = "simasucks";

// General light/timer settings
boolean timerMode = true; // NOT USED (it's implemented by the vivarium master controller, since this contains the RTC)
boolean manual_on = false;
unsigned int hour1 = 7; // simple for now // NOT USED (see above)
unsigned int hour2 = 12 + 7; // NOT USED (see above)
unsigned long time_last_checked = 0;
unsigned long time_last_update = 0;

// Fan settings
boolean fan_timer = false;
boolean fan_off = true;
unsigned int fan_level_day = 1024;
unsigned int fan_level_set = 1024;
unsigned int fan_level_night = 0; // Don't PWM plain old DC fans, it doesn't work right

//Declare a global object variable from the ESP8266WebServer class.
ESP8266WebServer server(80); //Server on port 80

//==============================================================
//                  SETUP
//==============================================================
void setup(void){
  Serial.begin(115200);
  // Button setup
  bounce.attach(MODE_BUTTON);
  bounce.interval(25);
  
  turnOffOnboardLEDs();
  // Initialize display
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally

  Serial.println(F("Setting up OLED"));
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
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
  server.on("/fan", handleCommand); //as Per  <a href="ledOn">, Subroutine to be called
  server.on("/request", handleRequest); //as Per  <a href="ledOn">, Subroutine to be called

  server.begin();                  //Start server
  Serial.println("HTTP server started");

  analogWrite(fan_pin, 0); // Make sure the fans are off! (in case of a power outage, for example)
  setFan();
  updateDisplay();
}
//==============================================================
//                     LOOP
//==============================================================
void loop(void){
  bounce.update();
  handleButton();
  
  server.handleClient();          // Handle client requests
  //setLightLevelSmoothly();        // Fade into new light level (if not in timer mode)
  perodicUpdates();               // Anything that needs to be updated periodically goes in ehre
}

void handleButton() {
  if ( bounce.fell() ) {
    Serial.println(F("Button was pressed!"));
    but_cnt = (but_cnt + 1) % 3;
    
    if (but_cnt == 0) {
      timerMode = true;
      manual_on = false;
    } 
    if (but_cnt == 1) {
      timerMode = false;
      manual_on = true;
    } 
    if (but_cnt == 2) {
      timerMode = false;
      manual_on = false;
    }

     updateDisplay();
  }
}
//===============================================================
// This routine is executed when you open its IP in browser
//===============================================================

void handleCommand() { 
  Serial.println("Processing incoming command...");
 for (int i = 0; i < server.args(); i++) {
   Serial.print(String(i) + " ");  //print id
   Serial.print("\"" + String(server.argName(i)) + "\" ");  //print name
   Serial.println("\"" + String(server.arg(i)) + "\"");  //print value
  
   if (server.argName(i) == "fan") {
    fan_level_set = server.arg(i).toInt();
    setFan();
   }


   if (server.argName(i) == "fanmode") {
    if (server.arg(i) == "timer") {
      fan_timer = true;
    } else if (server.arg(i) == "off") {
      fan_off = true;
      analogWrite(fan_pin, 0);
    } else {
      fan_timer = false;
      fan_off = false;
      setFan();
    }
   }

   if (server.argName(i) == "mode") {
    if (server.arg(i) == "timer") {
      timerMode = true;
      manual_on = false;
      Serial.println("Switching mode to TIMER");
    } else if (server.arg(i) == "on") {
      manual_on = true;
      timerMode = false;
    } else {
      timerMode = false;
      manual_on = false;
      Serial.println("Turning TIMER mode off");
    }
   }
   
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
  String return_str = "none";
  
  for (int i = 0; i < server.args(); i++) {
   Serial.print(String(i) + " ");  //print id
   Serial.print("\"" + String(server.argName(i)) + "\" ");  //print name
   Serial.println("\"" + String(server.arg(i)) + "\"");  //print value
   if (server.argName(i) == "fans") {
    if (fan_off) {
      return_str = "off";
    } else {
      return_str = "on";
    }
   }
  }
  server.send(200, "text/html", return_str); //Send ADC value only to client ajax request
}


// Periodic update catch-all
void perodicUpdates() {
  if ((millis() - time_last_update) > 10000) {
    time_last_update = millis();
    updateDisplay();
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
  display.print(F("Fan:"));
  display.print(" ");
  display.print(fan_level_set);
  display.print(F(" Status:"));
  display.print(" ");
  String mode_str = fan_off ? "OFF" : "ON";
  display.println(mode_str);
  
  display.println(WiFi.localIP());

  display.display();
}

// Set Fan
void setFan() {
  Serial.println("Setting fan level in setFan()");
  if (!fan_timer && !fan_off) {
    analogWrite(fan_pin, fan_level_set);
  }
}

// Turn off ESP8266 / NodeMCU LEDs
void turnOffOnboardLEDs() {
  digitalWrite(2, HIGH);
  digitalWrite(LED_BUILTIN, HIGH);
}
