/*
   Trying to phase this script out, it will do light and fan handling, and can be addressed directly at its IP address
   Would like to transition to having a controller for the lights and fans separate, that talk to some web server either
   running on a pi or a esp32 -- WIP
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>

//ESP Web Server Library to host a web page
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

//For JSON encoding
#include <ArduinoJson.h>

//Static IP address configuration
IPAddress staticIP(192, 168, 100, 123); //ESP static ip
IPAddress gateway(192, 168, 100, 1);   //IP Address of your WiFi Router (Gateway)
IPAddress subnet(255, 255, 255, 0);  //Subnet mask
IPAddress dns1(8, 8, 8, 8);  //DNS
IPAddress dns2(8, 8, 4, 4);  //DNS

const char* deviceName = "viv_light1";

//SSID and Password of your WiFi router
const char* ssid = "526Beacon";
const char* password = "simasucks";

// Solar emulation settings
boolean solar_emulation = true;

// Light fading settings
unsigned int slow_interval = 20;
unsigned int warm_level_cur = 0;
unsigned int cool_level_cur = 0;
unsigned int warm_level_set = 600;  // These are used as max values when in solar mode
unsigned int cool_level_set = 1000; // These are used as max values when in solar mode
unsigned long time_last = 0;

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
unsigned int fan_level_day = 1024;
unsigned int fan_level_set = 1024;
unsigned int fan_level_night = 0; // Don't PWM plain old DC fans, it doesn't work right


//Declare a global object variable from the ESP8266WebServer class.
ESP8266WebServer server(80); //Server on port 80
HTTPClient http;

// Inside the brackets, 200 is the RAM allocated to this document.
// Don't forget to change this value to match your requirement.
// Use arduinojson.org/v6/assistant to compute the capacity.
StaticJsonDocument<200> json_doc;

//==============================================================
//                  SETUP
//==============================================================
void setup(void) {
  Serial.begin(115200);

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
  server.on("/request", handleRequest); // Handle a request from the server

  server.begin();                  //Start server
  Serial.println("HTTP server started");

}
//==============================================================
//                     LOOP
//==============================================================
void loop(void) {
  server.handleClient();          // Handle client requests
}


//===============================================================
// This routine is executed when you open its IP in browser
//===============================================================


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

    // Asking what the light levels are
    if (server.argName(i) == "config") {
      json_doc["timer"] = timer_mode ? "on" : "off";
      json_doc["cool"] = cool_level_set;
      json_doc["warm"] = warm_level_set;
      json_doc["solar"] = solar_emulation ? "on" : "off";
      json_doc["fan"] = fan_off ? "off" : "on";
      json_doc["manualon"] = manual_on ? "on" : "off";

      Serial.println("Sending this data:");
      serializeJson(json_doc, Serial);
      Serial.println();

    }

    if (server.argName(i) == "warmlevelset") {
      return_str = String(warm_level_set);
    }



    //
  }

  String json_str;
  serializeJson(json_doc, json_str);
  server.send(200, "text/html", json_str); //Send ADC value only to client ajax request
}
