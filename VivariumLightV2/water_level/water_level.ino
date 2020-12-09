#include <WiFiClient.h>

// Config (just a bunch of defines for device addresses)
#include <Vivarium2Common.h> // FLASK_ADDR

#define WATER_LOW_PIN 4
#define ENABLE_SENSORS_PIN 12 // D6 on Wemos D1

//ESP Web Server Library to host a web page
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>


//For JSON encoding
#include <ArduinoJson.h>

// Inside the brackets, 200 is the RAM allocated to this document.
StaticJsonDocument<250> json_doc;

// Upload/wake-up interval
unsigned long upload_interval = 20*60; // every 20 minutes

// Server settings
const char* deviceName = "water1";

//SSID and Password of your WiFi router
const char* ssid = "526Beacon";
const char* password = "simasucks";

//Static IP address configuration
IPAddress staticIP(192, 168, 100, 74); //ESP static ip
IPAddress gateway(192, 168, 100, 1);   //IP Address of your WiFi Router (Gateway)
IPAddress subnet(255, 255, 0, 0);  //Subnet mask
IPAddress dns1(8, 8, 8, 8);  //DNS
IPAddress dns2(8, 8, 4, 4);  //DNS

HTTPClient http;

// Sensor settings
boolean water_level_low = true; 

void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
  pinMode(ENABLE_SENSORS_PIN, OUTPUT);
  delay(50);
  pinMode(WATER_LOW_PIN, INPUT_PULLUP);

  // Configure static IP
  if (!WiFi.config(staticIP, gateway, subnet, dns1, dns2)) {
    Serial.println("STA Failed to configure");
  }
  
  if (connectToWiFi()) {
    Serial.println("I connected to WiFi! :]");
  } else {
    Serial.println("ERROR: Did not connect to WiFi.");
  }

  checkWaterLevel();
  sendDataToServer(FLASK_ADDR);

  digitalWrite(ENABLE_SENSORS_PIN, LOW); // Disable the sensor power flow
  Serial.println("I'm going to sleep now");
  Serial.println(millis()/1000);
  
  ESP.deepSleep(upload_interval*1000000); // Wake up every so often
  
}

void loop() {
  // put your main code here, to run repeatedly:

  delay(500);

}


/* CHECK WATER LEVEL */
void checkWaterLevel() {
  
  if (digitalRead(WATER_LOW_PIN) == LOW) {
    Serial.println("Water level: OK");
    water_level_low = false;
  } else {
    Serial.println("Water level: LOW");
    water_level_low = true;
  }

  digitalWrite(ENABLE_SENSORS_PIN, LOW); // Disable the sensor power flow
}


/* CONNECT TO WIFI */
// Return true if connection succesful
bool connectToWiFi() {
  // WIFI setup
  WiFi.hostname(deviceName);      // DHCP Hostname (useful for finding device for static lease)
//  if (!WiFi.config(staticIP, subnet, gateway, dns1, dns2)) {
//    Serial.println("Static IP failed to configure");
//  }
  WiFi.begin(ssid, password);

  WiFi.mode(WIFI_STA); //WiFi mode station (connect to wifi router only
  
  // Wait for connection
  int cnt = 0;
  int max_attempts = 10;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
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
  json_doc["low"] = water_level_low;

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
  
}
