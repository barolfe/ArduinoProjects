#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "Wire.h"
#include "Max44009.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//String flask_addr = "http://192.168.100.34:5000/datasend/sensors";

// Inside the brackets, 200 is the RAM allocated to this document.

// Dispaly settings
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// DHT settings
#define DHTPIN D7

// Uncomment the type of sensor in use:
//#define DHTTYPE    DHT11     // DHT 11
#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

DHT_Unified dht(DHTPIN, DHTTYPE);

// Sensor global values
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



// ===================================
// ======= MAIN SETUP AND LOOP =======
// ===================================


void setup() {
  
  // put your setup code here, to run once:
  Serial.begin(115200);

  Serial.println("I'm awake!!!!");
  dht.begin();

  // Setup display
  Serial.println(F("Setting up OLED"));
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    b_use_display = false;
  }
}

void loop() {
  updateAverages();
  updateDisplay();
  delay(250);
}


// ===================================
// ==== BEGIN FUNCTION DEFINITIONS ===
// ===================================

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

  
  if (n_sampled > num_samples) {
    n_sampled = 5;
  }
  
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

   display.print("Temp Avg: ");
  display.print(T_F_avg);
  display.println("F");

  display.print("Humid Avg: ");
  display.print(humid_avg);
  display.println("%");

  display.setTextSize(2);      // Normal 1:1 pixel scale
  display.print("T: ");
  display.print(T_F);
  display.println("F");

  display.print("H: ");
  display.print(humid);
  display.println("%");



  
  display.display();
}
