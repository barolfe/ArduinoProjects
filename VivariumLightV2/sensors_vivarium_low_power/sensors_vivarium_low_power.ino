#include <WiFiClient.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "Wire.h"


//ESP Web Server Library to host a web page
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

//For JSON encoding
#include <ArduinoJson.h>

// Config (just a bunch of defines for device addresses)
#include <Vivarium2Common.h> // FLASK_ADDR, 

// This device needs to operate in a lower power mode eventually
// 1. Wake-up at some pre-defined itnerval (every few minutes, maybe every 5)
// 2. Take sensor readings
// 3. SEND (POST/GET) to server
// 4. Go back to sleep

//String flask_addr = "http://192.168.100.34:5000/datasend/sensors";

// Inside the brackets, 200 is the RAM allocated to this document.
StaticJsonDocument<250> json_doc;

// DHT settings
#define DHTPIN 14

// Uncomment the type of sensor in use:
//#define DHTTYPE    DHT11     // DHT 11
#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

DHT_Unified dht(DHTPIN, DHTTYPE);

// Sensor global values
#define ENABLE_SENSORS_PIN 13 
float T_F = 0;
float humid = 0;

// Upload/wake-up interval
unsigned long upload_interval = 10*60; // every 2 minutes

// Server settings
const char* deviceName = "therm_right";

//SSID and Password of your WiFi router
const char* ssid = "526Beacon";
const char* password = "simasucks";

//Static IP address configuration
IPAddress staticIP(192, 168, 100, 80); //ESP static ip
IPAddress gateway(192, 168, 100, 1);   //IP Address of your WiFi Router (Gateway)
IPAddress subnet(255, 255, 0, 0);  //Subnet mask
IPAddress dns1(8, 8, 8, 8);  //DNS
IPAddress dns2(8, 8, 4, 4);  //DNS


//Declare a global object variable from the ESP8266WebServer class.
//ESP8266WebServer server(80); //Server on port 80
HTTPClient http;


// ===================================
// ======= MAIN SETUP AND LOOP =======
// ===================================


void setup() {

  Serial.begin(115200);
  
  Serial.println("I'm awake!!!!");

  if (!WiFi.config(staticIP, gateway, subnet, dns1, dns2)) {
    Serial.println("Static IP failed to configure");
  }
  
  bool b_connected = false;
  
  if (connectToWiFi()) {
    Serial.println("I connected to WiFi! :]");
    b_connected = true;
  } else {
    Serial.println("ERROR: Did not connect to WiFi.");
  }
  

  pinMode(ENABLE_SENSORS_PIN, OUTPUT);

  digitalWrite(ENABLE_SENSORS_PIN, HIGH); // Enable power to the sensor device
  delay(750);
  dht.begin();
  
  if (b_connected) {
    getTemperatureValue();
    sendDataToServer(FLASK_ADDR);
  }

  digitalWrite(ENABLE_SENSORS_PIN, LOW); // Disable power to the sensor device before going to sleep
  Serial.println("I'm going to sleep now");

  Serial.print("Time running:");
  Serial.println(millis());
   
  ESP.deepSleep(upload_interval*1000000); // Wake up every so often
}

void loop() {
// Do nothing -- this will never execute
}


// ===================================
// ==== BEGIN FUNCTION DEFINITIONS ===
// ===================================


/* CONNECT TO WIFI */
// Return true if connection succesful
bool connectToWiFi() {
  // WIFI setup
  WiFi.hostname(deviceName);      // DHCP Hostname (useful for finding device for static lease)

  WiFi.begin(ssid, password);

  WiFi.mode(WIFI_STA); //WiFi mode station (connect to wifi router only
  
  // Wait for connection
  int cnt = 0;
  int max_attempts = 20;
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    cnt += 1;

    if (cnt == max_attempts) {
      return false;
    }
    Serial.print(".");
  }

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP

  return true;
}

/* POST DATA TO SERVER */

bool sendDataToServer(String server_addr) {

  json_doc["id"] = deviceName;
  json_doc["T_F"] = T_F;
  json_doc["humid"] = humid;
  
  http.begin(server_addr + "/datasend/sensors");
  http.addHeader("Content-Type", "application/json");

  String json_str = "";
  serializeJson(json_doc, json_str);

  int httpCode = http.POST(json_str);
  String payload = http.getString();

  Serial.println("JSON package:");
  Serial.println(json_str);
  Serial.println("Response:");
  Serial.println(payload);

  http.end();

  return true;
  
}


// Function to periodically update the global temp and humidity
void getTemperatureValue() {

  sensors_event_t event;
  dht.temperature().getEvent(&event);

  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
    return;
  }
  else {
    T_F = event.temperature*(9.0/5.0)+32;
    Serial.print(F("Temperature: "));
    Serial.print(T_F);
    Serial.println(F("°F"));
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  }
  else {
    humid = event.relative_humidity;
    Serial.print(F("Humidity: "));
    Serial.print(humid);
    Serial.println(F("%"));
  }

  
}
