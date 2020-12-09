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

// Dispaly settings
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// DHT settings
#define DHTPIN D7

// Uncomment the type of sensor in use:
#define DHTTYPE    DHT11     // DHT 11
//#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

DHT_Unified dht(DHTPIN, DHTTYPE);

// Light sensor:
Max44009 myLux(0x4A);

// Sensor global values
#define ENABLE_SENSORS_PIN D5 // Not used right now
float T_F = 0;
float humid = 0;
float lux = 0;
float watts = 0;
float T_F_arr[5] = {0, 0, 0, 0, 0};
float humid_arr[5] = {0, 0, 0, 0, 0};
float T_F_avg = 0;
float humid_avg = 0;
unsigned int avg_cnt = 0; // this is a counter, it needs to be 0 
unsigned int num_samples = 5; // if you change this, you need to change the arrays too.
unsigned int n_sampled = 0;
unsigned int avg_interval = 1; // in seconds
unsigned long last_sensor_check = 0;
boolean b_use_display = true;

// Upload/wake-up interval
unsigned long upload_interval = 2*60; // every 2 minutes

// Server settings
const char* deviceName = "therm1";

//SSID and Password of your WiFi router
const char* ssid = "526Beacon";
const char* password = "simasucks";

//Static IP address configuration
IPAddress staticIP(192, 168, 100, 75); //ESP static ip
IPAddress gateway(192, 168, 100, 1);   //IP Address of your WiFi Router (Gateway)
IPAddress subnet(255, 255, 255, 0);  //Subnet mask
IPAddress dns1(8, 8, 8, 8);  //DNS
IPAddress dns2(8, 8, 4, 4);  //DNS


//Declare a global object variable from the ESP8266WebServer class.
//ESP8266WebServer server(80); //Server on port 80
HTTPClient http;


// ===================================
// ======= MAIN SETUP AND LOOP =======
// ===================================


void setup() {
  
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

  // Setup display
  Serial.println(F("Setting up OLED"));
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    b_use_display = false;
  }
  
  bool b_connected = false;
  if (connectToWiFi()) {
    Serial.println("I connected to WiFi! :]");
    b_connected = true;
  } else {
    Serial.println("ERROR: Did not connect to WiFi.");
  }

  if (b_connected) {
    updateAverages(num_samples);
    updateDisplay();
    sendDataToServer(FLASK_ADDR);
    delay(500);
  }
  
  Serial.println("I'm going to sleep now");
  Serial.println(millis()/1000);
  
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
  if (!WiFi.config(staticIP, subnet, gateway, dns1, dns2)) {
    Serial.println("Static IP failed to configure");
  }
  WiFi.begin(ssid, password);

  WiFi.mode(WIFI_STA); //WiFi mode station (connect to wifi router only
  
  // Wait for connection
  int cnt = 0;
  int max_attempts = 30;
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

void updateAverages(int n) {
  n_sampled = 0;
  for (int i = 0; i < n; i ++) {
    updateAverages();
    delay(avg_interval*1000);
  }
}
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

  n_sampled += 1;

  T_F_arr[avg_cnt]   = T_F;
  humid_arr[avg_cnt] = humid;

  T_F_avg = 0;
  humid_avg = 0;
  
  for (int i = 0; i < num_samples; i++) {
    T_F_avg += T_F_arr[i];
    humid_avg += humid_arr[i];
  }
  
  T_F_avg   = T_F_avg / float(n_sampled);
  humid_avg = humid_avg / float(n_sampled);

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


// Function to update the display
void updateDisplay() {

  if (!b_use_display) {
    return;
  }
  
  display.clearDisplay();

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  display.print("Temp: ");
  display.print(T_F);
  display.println("F");

  display.print("Humid: ");
  display.print(humid);
  display.println("%");

  display.print("Temp Avg: ");
  display.print(T_F_avg);
  display.println("F");

  display.print("Humid: ");
  display.print(humid_avg);
  display.println("%");

  display.print("Lux: ");
  display.print(lux);
  display.println(" lux");

  display.print("Power: ");
  display.print(watts);
  display.println(" w/m2");


  // Print out this IP
  display.println(WiFi.localIP());
   
  
  display.display();
}
