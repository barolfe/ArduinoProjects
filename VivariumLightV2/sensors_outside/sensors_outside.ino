#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "Wire.h"
#include "Max44009.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//ESP Web Server Library to host a web page
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

//For JSON encoding
#include <ArduinoJson.h>

// This device needs to operate in a lower power mode eventually
// 1. Wake-up at some pre-defined itnerval (every few minutes, maybe every 5)
// 2. Take sensor readings
// 3. SEND (POST/GET) to server
// 4. Go back to sleep

String flask_addr = "http://192.168.100.34:5000/datasend/sensors";

// Inside the brackets, 200 is the RAM allocated to this document.
StaticJsonDocument<250> json_doc;

// Dispaly settings
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// DHT settings
#define DHTPIN D5

// Uncomment the type of sensor in use:
#define DHTTYPE    DHT11     // DHT 11
//#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

DHT_Unified dht(DHTPIN, DHTTYPE);

// Light sensor:
Max44009 myLux(0x4A);

// Sensor global values
#define ENABLE_SENSORS_PIN D6
float T_F = 0;
float humid = 0;
float lux = 0;
float watts = 0;
float T_F_arr[5] = {0, 0, 0, 0, 0};
float humid_arr[5] = {0, 0, 0, 0, 0};
float T_F_avg = 0;
float humid_avg = 0;
unsigned int avg_cnt = 0; // this is a counter, it needs to be 0 
unsigned int num_samples = 1; // if you change this, you need to change the arrays too.
unsigned int avg_interval = 5; // in seconds
unsigned long last_sensor_check = 0;

// Upload/wake-up interval
unsigned long upload_interval = 10*60; // every 5 minutes

// Server settings
const char* deviceName = "therm2";

//SSID and Password of your WiFi router
const char* ssid = "526Beacon";
const char* password = "simasucks";

//Static IP address configuration
IPAddress staticIP(192, 168, 100, 73); //ESP static ip
IPAddress gateway(192, 168, 100, 1);   //IP Address of your WiFi Router (Gateway)
IPAddress subnet(255, 255, 0, 0);  //Subnet mask
IPAddress dns1(8, 8, 8, 8);  //DNS
IPAddress dns2(8, 8, 4, 4);  //DNS


//Declare a global object variable from the ESP8266WebServer class.
//ESP8266WebServer server(80); //Server on port 80
HTTPClient http;

void setup() {
  // Enable the sensors
  pinMode(ENABLE_SENSORS_PIN, OUTPUT);
  digitalWrite(ENABLE_SENSORS_PIN, HIGH); // Enable the sensor power flow
  delay(500);
  
  // put your setup code here, to run once:
  Serial.begin(115200);

  Serial.println("I'm awake!!!!");
  dht.begin();
  
  // For light sensor:
  Wire.begin();
  Wire.setClock(100000);

  // Configure static IP
  if (!WiFi.config(staticIP, gateway, subnet, dns1, dns2)) {
    Serial.println("STA Failed to configure");
  }
  
  if (connectToWiFi()) {
    Serial.println("I connected to WiFi! :]");
  } else {
    Serial.println("ERROR: Did not connect to WiFi.");
  }

  updateAverages();

  sendDataToServer(flask_addr);

  digitalWrite(ENABLE_SENSORS_PIN, LOW); // Disable the sensor power flow
  Serial.println("I'm going to sleep now");
  Serial.println(millis()/1000);
  
  ESP.deepSleep(upload_interval*1000000); // Wake up every so often
}

void loop() {
// Do nothing -- this will never execute
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
  json_doc["T_F"] = T_F_avg;
  json_doc["humid"] = humid_avg;
  json_doc["lux"]  = lux;
  json_doc["watts"] = watts;
  
  http.begin(server_addr);
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

/*
void handleDataRequest(){

  String return_str = "";
  
  Serial.println("Incoming data request!");
  
  for (int i = 0; i < server.args(); i++) {
   Serial.print(String(i) + " ");  //print id
   Serial.print("\"" + String(server.argName(i)) + "\" ");  //print name
   Serial.println("\"" + String(server.arg(i)) + "\"");  //print value
    
    // Asking for some temperature goodies
    if (server.argName(i) == "sensors") {
      json_doc["T_F"] = T_F_avg;
      json_doc["humid"] = humid_avg;
      
      Serial.println("Sending this data:");
      serializeJson(json_doc, Serial);
      Serial.println();

      // print the data to the return string so we can send it (below)
      serializeJson(json_doc, return_str);
    }
  }
//  server.send(200, "text/html", return_str); //Send ADC value only to client ajax request
}
*/

// Function to periodically update the global averages
void updateAverages() {

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

  T_F_arr[avg_cnt]   = T_F;
  humid_arr[avg_cnt] = humid;

  T_F_avg = 0;
  humid_avg = 0;
  
  for (int i = 0; i < num_samples; i++) {
    T_F_avg += T_F_arr[i];
    humid_avg += humid_arr[i];
  }
  
  T_F_avg   = T_F_avg / float(num_samples);
  humid_avg = humid_avg / float(num_samples);


  lux = myLux.getLux();
  watts = lux * 0.0079;
  Serial.print(F("Lux: "));
  Serial.print(lux);
  Serial.println(F(" lux"));

  Serial.print(F("Power: "));
  Serial.print(watts);
  Serial.println(F(" w/m2"));
    
  avg_cnt = (avg_cnt + 1) % num_samples;
  
}
