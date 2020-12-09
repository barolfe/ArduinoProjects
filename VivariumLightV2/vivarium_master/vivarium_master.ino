/*
 * ESP8266 NodeMCU LED Control over WiFi Demo
 * Using Static IP Address for ESP8266
 * https://circuits4you.com
 */
#include <WiFi.h>
//#include <WiFiClient.h>
#include "RTClib.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Bounce2.h>

//ESP Web Server Library to host a web page
//#include <ESP8266WebServer.h>
//#include <ESPAsyncWebServer.h>
#include <WebServer.h>

//RTC (real-time clock)
RTC_DS3231 rtc;

//Static IP address configuration
IPAddress staticIP(192, 168, 100, 51); //ESP static ip
IPAddress gateway(192, 168, 100, 1);   //IP Address of your WiFi Router (Gateway)
IPAddress subnet(255, 255, 255, 0);  //Subnet mask
IPAddress dns1(8, 8, 8, 8);  //DNS
IPAddress dns2(8, 8, 4, 4);  //DNS

// Clients
String url_light1 = "http://192.168.100.49/lightlevel";

const char* deviceName = "viv_light1";

//OLED settings
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


// Input settings
#define MODE_BUTTON 8
Bounce bounce = Bounce();
unsigned int but_cnt = 0;

// Timer settings
boolean timerMode = true;
unsigned long time_last_update = 0;

//SSID and Password of your WiFi router
const char* ssid = "526Beacon";
const char* password = "simasucks";


// Prototypes

//Declare a global object variable from the ESP8266WebServer class.
//ESP8266WebServer server(80); //Server on port 80
//AsyncWebServer server(80);
WebServer server(80);

//==============================================================
//                  SETUP
//==============================================================
void setup(void){
  Serial.begin(115200);
  // Button setup
  bounce.attach(MODE_BUTTON);
  bounce.interval(25);
  
  // Initialize display
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally

  Serial.println(F("Setting up OLED"));
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  Serial.println(F("Setting up RTC"));
  // RTC setup
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    //Serial.flush();
    //abort();
  }

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
  //WiFi.hostname(deviceName);      // DHCP Hostname (useful for finding device for static lease)
  //if (!WiFi.config(staticIP, subnet, gateway, dns1, dns2)) {
  //  Serial.println("Static IP failed to configure");
  //}
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
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP

  // Server setup
  //server.on("/lightlevel", handleLightLevel); // This callback is intended to handle messages from the user/UI
  server.on("/requests", handleRequests); // this callback handles requests to the master
  server.on("/time", handleTime); // this callback handles requests to the master
  /*server.on("/time", HTTP_GET, [] (AsyncWebServerRequest *request) {
   Serial.println("Time request received");
   DateTime now = RTC.now();
   request->send(200, "text/plain", now.unixtime());
  });*/
  
  server.begin();                  //Start server
  Serial.println("HTTP server started");

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
    } 
    if (but_cnt == 1) {
      timerMode = false;
    } 
    if (but_cnt == 2) {
      timerMode = false;
    }

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
  
 }
 server.send(200, "text/html", "Got it."); //Send ADC value only to client ajax request
}

void handleRequests() {

}

void handleTime() {
   Serial.println("Time request received");
   DateTime now = rtc.now();
   String payload = String(now.unixtime());
   Serial.println("Sending this payload: " + payload);
   server.send(200, "text/plain", payload);
}

// Send time packet to a requesting client
// Periodic update catch-all
void perodicUpdates() {
  if ((millis() - time_last_update) > 10000) {
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

  display.print(F("Timer: "));
  if (timerMode) {
    display.println(F("ON"));
  } else {
    display.println(F("OFF"));
  }

  display.println(WiFi.localIP());

  display.display();
}
