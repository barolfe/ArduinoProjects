// mapping suggestion from Waveshare SPI e-Paper to generic ESP8266
// BUSY -> GPIO4, RST -> GPIO2, DC -> GPIO0, CS -> GPIO15, CLK -> GPIO14, DIN -> GPIO13, GND -> GND, 3.3V -> 3.3V

// include library, include base class, make path known
#include <GxEPD.h>

#include <GxGDEW042T2/GxGDEW042T2.h>      // 4.2" b/w


#include GxEPD_BitmapExamples

// FreeFonts from Adafruit_GFX
#include "square64pt7b.h"
//#include <Fonts/FreeMono9pt7b.h>
//#include <Fonts/FreeMonoBold12pt7b.h>
//#include <Fonts/FreeMonoBold18pt7b.h>
//#include <Fonts/FreeMonoBold24pt7b.h>


#include <GxIO/GxIO_SPI/GxIO_SPI.h>
#include <GxIO/GxIO.h>

#if defined(ESP8266)

// for SPI pin definitions see e.g.:
// C:\Users\xxx\AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\2.4.2\variants\generic\common.h

GxIO_Class io(SPI, /*CS=D8*/ SS, /*DC=D3*/ 0, /*RST=D4*/ 2); // arbitrary selection of D3(=0), D4(=2), selected for default of GxEPD_Class
GxEPD_Class display(io /*RST=D4*/ /*BUSY=D2*/); // default selection of D4(=2), D2(=4)
// Heltec E-Paper 1.54" b/w without RST, BUSY
//GxEPD_Class display(io, /*RST=D4*/ -1, /*BUSY=D2*/ -1); // no RST, no BUSY

#elif defined(ESP32)

// for SPI pin definitions see e.g.:
// C:\Users\xxx\Documents\Arduino\hardware\espressif\esp32\variants\lolin32\pins_arduino.h

GxIO_Class io(SPI, /*CS=5*/ SS, /*DC=*/ 17, /*RST=*/ 16); // arbitrary selection of 17, 16
GxEPD_Class display(io, /*RST=*/ 16, /*BUSY=*/ 4); // arbitrary selection of (16), 4

#elif defined(ARDUINO_ARCH_SAMD)

// for SPI pin definitions see e.g.:
// C:\Users\xxx\AppData\Local\Arduino15\packages\arduino\hardware\samd\1.6.19\variants\mkr1000\variant.h
// C:\Users\xxx\AppData\Local\Arduino15\packages\arduino\hardware\samd\1.6.19\variants\mkrzero\variant.h

GxIO_Class io(SPI, /*CS=*/ 4, /*DC=*/ 7, /*RST=*/ 6);
GxEPD_Class display(io, /*RST=*/ 6, /*BUSY=*/ 5);

#elif defined(ARDUINO_GENERIC_STM32F103C) && defined(MCU_STM32F103C8)

// STM32 Boards(STM32duino.com) Generic STM32F103C series STM32F103C8
// for SPI pin definitions see e.g.:
// C:\Users\xxx\Documents\Arduino\hardware\Arduino_STM32\STM32F1\variants\generic_stm32f103c\variant.h
// C:\Users\xxx\Documents\Arduino\hardware\Arduino_STM32\STM32F1\variants\generic_stm32f103c\board\board.h

// new mapping suggestion for STM32F1, e.g. STM32F103C8T6 "BluePill"
// BUSY -> A1, RST -> A2, DC -> A3, CS-> A4, CLK -> A5, DIN -> A7

GxIO_Class io(SPI, /*CS=*/ SS, /*DC=*/ 3, /*RST=*/ 2);
GxEPD_Class display(io, /*RST=*/ 2, /*BUSY=*/ 1);

#elif defined(ARDUINO_GENERIC_STM32F103V) && defined(MCU_STM32F103VB)

// STM32 Boards(STM32duino.com) Generic STM32F103V series STM32F103VB
// for SPI pin definitions see e.g.:
// C:\Users\xxx\Documents\Arduino\hardware\Arduino_STM32\STM32F1\variants\generic_stm32f103vb\variant.h
// C:\Users\xxx\Documents\Arduino\hardware\Arduino_STM32\STM32F1\variants\generic_stm32f103vb\board\board.h

// Good Display DESPI-M01
// note: needs jumper wires from SS=PA4->CS, SCK=PA5->SCK, MOSI=PA7->SDI

GxIO_Class io(SPI, /*CS=*/ SS, /*DC=*/ PE15, /*RST=*/ PE14); // DC, RST as wired by DESPI-M01
GxEPD_Class display(io, /*RST=*/ PE14, /*BUSY=*/ PE13); // RST, BUSY as wired by DESPI-M01

#else

// for SPI pin definitions see e.g.:
// C:\Users\xxx\AppData\Local\Arduino15\packages\arduino\hardware\avr\1.6.21\variants\standard\pins_arduino.h

GxIO_Class io(SPI, /*CS=*/ SS, /*DC=*/ 8, /*RST=*/ 9); // arbitrary selection of 8, 9 selected for default of GxEPD_Class
GxEPD_Class display(io /*RST=9*/ /*BUSY=7*/); // default selection of (9), 7

#endif

#include <OneWire.h>
#include <DallasTemperature.h>

#include <EEPROM.h>
#include "eeprom_templates.h"

#include <RPiCom.h>

const char *ssid = "526Beacon";
const char *pwd  = "simasucks";
String raspi_addr = "http://192.168.100.34:5000/";
String identifier = "e_ink";

RPiCom rpi(raspi_addr, identifier);

#define HIST_LEN  512
float history[HIST_LEN];
unsigned int hist_pos = 0;
unsigned int hist_cnt = 0;
unsigned int interval_min = 3;
float t_min = 100;
float t_max = 0;
int x_min = 0;
bool b_update_to_cloud = false;

// GPIO where the DS18B20 is connected to
const int oneWireBus = D1;     

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

unsigned long t_last = 0;

#include <ESP8266WiFi.h>
#include <WiFiClient.h>


// Function prototypes
float getTemp(int n);
void initHistory();
void updateHistory(float temp);
void writeToEEPROM();
bool readFromEEPROM();
void checkTMinMax(float & t_min, float & t_max);
bool connectToWifi();
void makeGetRequest(float temp);
void showTemp(float temp, const GFXfont* font);
void drawGraph();
void drawBar();


WiFiClient client;

void setup()
{

  Serial.begin(115200);

  display.init(115200); // enable diagnostic output on Serial

  int eeprom_size = sizeof(int)*HIST_LEN + sizeof(int) + 32;
  if (eeprom_size <= 4096) {
    EEPROM.begin(eeprom_size);
    // to clear memory:
    if (true) {
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
  
  // Start the DS18B20 Sensor
  sensors.begin();
  t_last = millis();

  initHistory();
  
  Serial.println("setup done");

  readFromEEPROM();
  float temp = getTemp(10);
  Serial.println(temp);
  showTemp(temp, &Square64pt7b);
  
  drawGraph();
  display.update();
 
  writeToEEPROM();

  if (b_update_to_cloud) {
    if (connectToWifi()) {
     // makeGetRequest(temp); // upload temp to cloud
      Serial.println("Sending data packet to RPI at: " + raspi_addr);
      String resp = rpi.sendLuxTempHumid(0, temp, 0);
      Serial.println(resp);
      //WiFi.end(); // there is currently no way to turn off the WiFi, but when put into deep sleep it will be disabled
    }
  }
  
  ESP.deepSleep(interval_min*60e06); // wake up every few minutes
}

void loop() {

  //delay(interval_min*60*1000);
}



/* BEGIN FUNCTION DEFS */
float getTemp(int n) {
  float t_avg = 0;
  for (int i = 0; i < n; i ++) {
    sensors.requestTemperatures(); 
    float temperature = sensors.getTempFByIndex(0);
    t_avg += temperature;
    delay(10);
  }
  t_avg = t_avg / n;
  updateHistory(t_avg);
  return t_avg;
}

void initHistory() {
  for (int i = 0; i < HIST_LEN; i++) {
    history[i] = -99;
  }
}

void updateHistory(float temp) {
  if (hist_pos >= HIST_LEN) {
    hist_pos = 0;
  }
  history[hist_pos] = temp;
  hist_pos++;
  if (hist_cnt < HIST_LEN) {
    hist_cnt++;
  }
}

void writeToEEPROM() {
  // Write data to EEPROM so that we can go into deep sleep
  int addr = 0;
  int addr_shift = EEPROM_writeAnything(addr, 1234); // identifier to signify data is present
  addr = addr + addr_shift;
  
  addr = addr + EEPROM_writeAnything(addr, hist_cnt);
  addr = addr + EEPROM_writeAnything(addr, hist_pos);
  
  for (int i = 0; i < HIST_LEN; i++) {
    addr_shift = EEPROM_writeAnything(addr, history[i]);
    addr = addr + addr_shift;
  } 
  EEPROM.commit(); // commit changes;
}

bool readFromEEPROM() {
  int addr = 0;
  int identifier;

  addr = addr +  EEPROM_readAnything(addr, identifier); // read back value to hist_cnt
  Serial.print(addr); Serial.print("\t");  Serial.println(identifier);
  if (identifier != 1234) {
    return false;
  }

  addr = addr +  EEPROM_readAnything(addr, hist_cnt); // read back value to hist_cnt
  addr = addr +  EEPROM_readAnything(addr, hist_pos);
  
  Serial.print(addr); Serial.print("\t");  Serial.println(hist_cnt);

  for (int i = 0; i < hist_cnt; i++) {
    addr = addr + EEPROM_readAnything(addr, history[i]);
    Serial.print(addr); Serial.print("\t"); Serial.println(history[i]);
  } 

  return true;
}

void checkTMinMax(float & t_min, float & t_max) {

  // reset min/max
  t_min = 100;
  t_max = 0;

  // find new min/max
  for (int i = 0; i < hist_cnt; i++) {
    if (history[i] != -99) {
      float temp = history[i];
      if (temp > t_max) {
        t_max = temp;
      } else if (temp < t_min) {
        t_min = temp;
      }
    }
  }
  
  if ((t_max - t_min) < 0.5) {
     t_min = t_max - 0.5;
  }
}

bool connectToWifi() {
  Serial.println("connecting");
  WiFi.begin(ssid, pwd);
  int cnt = 0;
  while (WiFi.status() != WL_CONNECTED) {
    if (cnt >= 10) {
      return false;
    }
    delay(500);
    Serial.print(".");
    cnt += 1;
  }
  Serial.println("WiFi Connected");
}

/*
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
 * 
void makeGetRequest(float temp) {
  // close any connection before send a new request to allow client make connection to server
  client.stop();

  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("connecting...");

    // send the HTTP PUT request:
    client.println("GET /projects/temperaturelogger/process_data.php?key=" + key + "&temp=" + temp + " HTTP/1.1");
    client.println("Host: www.bryanrolfe.com");
    client.println("User-Agent: ArduinoWiFi/1.1");
    client.println("Connection: close");
    client.println();
    
    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }
  }
  client.stop();
}
*/
  
void showTemp(float temp, const GFXfont* font) {
  //float _temp = (int) (10 * (temp + 0.5)) / 10.0;
  
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setFont(font);
  display.setCursor(0, 0);
  display.println();
  display.print(" ");
  char temp_str[5];
  dtostrf(temp, 4, 1, temp_str);
  display.print(temp_str);
  display.print("F");
  display.println();
}

// Testing graph draw feature
// We want to draw a graph with the newest value at the right, and the oldest value at left
// so we will start at hist_pos + 1 (this is our oldest value)
void drawGraph() {

  checkTMinMax(t_min, t_max);
  int x_left = 20;
  int ytop = 58+(19+5)*4+120;
  int max_height = 100.0;
  float scale = max_height / (t_max - t_min);
  Serial.println(hist_cnt);
  
  if (hist_cnt < 3) {
    return;
  }


  // Draw the graph
  display.setFont();
  int x_cnt = (hist_cnt < HIST_LEN) ? hist_cnt : HIST_LEN;
  float x_step =  64*6.0 / x_cnt;
  
  int line_width = 1;
  for (int w = 0; w < line_width; w++) {
    
    int pos = (hist_pos - (x_cnt - 1));
    pos = pos < 0 ? pos = HIST_LEN - abs(pos): pos;

    Serial.print(t_min); Serial.print(" ");
    Serial.print(t_max); Serial.print(" ");
    Serial.print(x_cnt); Serial.print(" "); 
    Serial.print(x_step); Serial.print(" ");
    Serial.print(hist_cnt); Serial.println(" ");
    Serial.print(hist_pos); Serial.println(" ");
    Serial.print(pos); Serial.println(" ");
     
    for (int i = 0; i < x_cnt - 1; i++) {
      int pos1 = pos % HIST_LEN;
      int pos2 = (pos+1) % HIST_LEN;
      int y1 = scale * (history[pos1] - t_min) - w;
      int y2 = scale * (history[pos2] - t_min) - w;
      //y1 = y1 < t_min ? t_min : y1;
      //y2 = y2 < t_min ? t_min : y2;
      Serial.print(y1); Serial.print(", "); Serial.println(y2);
      display.drawLine((int)i*x_step + x_left, ytop-y1,(int)(i+1)*x_step + x_left, ytop-y2, 0);

      // Print first and last temps
      if (i == 0) {
        display.setCursor((int)i*x_step + x_left, ytop-y1 - 10);
        display.print(history[pos1]);
        display.print("F");
      }

      if (i == (x_cnt -2)) {
        display.setCursor((int)i*x_step + x_left - 40, ytop-y1 + 5);
        display.print(history[pos1]);
        display.print("F");
      }

      if (history[pos1] == t_min) {
        display.setCursor((int)i*x_step + x_left + 20, ytop-y1 - 10);
        display.print(history[pos1]);
        display.print("F");
      }
      
      pos++;
    }
  }

  // Draw the x-axis
  display.drawLine(x_left,ytop, 64*6 + x_left,ytop,0);
  display.setFont();
  int x_interval = x_cnt / 10;
  for (int i=0; i < x_cnt; i=i + x_interval) {
    display.drawLine(x_left + (int)i*x_step, ytop, x_left + (int)i*x_step, ytop+5, 0);
    display.setCursor(x_left + (int)i*x_step, ytop+5);
    display.print((int)(HIST_LEN - i) * interval_min / 60);
  }

  display.setCursor(180, ytop + 15);
  display.print("(hours ago)");
}

void drawBar()
{
  uint8_t rotation = display.getRotation();

    //display.setRotation(r);
    //display.fillScreen(GxEPD_WHITE);
    display.fillRect(170, 58, 135*0.79, 19, GxEPD_BLACK);
    display.fillRect(170, 58+19+5, 135*0.6, 19, GxEPD_BLACK);
    display.fillRect(170, 58+(19+5)*2, 135, 19, GxEPD_BLACK);
    display.fillRect(170, 58+(19+5)*3, 135*0, 19, GxEPD_BLACK);
    display.update();
    delay(5000);
  //display.setRotation(rotation); // restore
}
