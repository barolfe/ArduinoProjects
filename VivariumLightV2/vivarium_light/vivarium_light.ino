#include <NTPClient.h>
#include <WiFiUdp.h>

/*
 * ESP8266 NodeMCU LED Control over WiFi Demo
 * Using Static IP Address for ESP8266
 * https://circuits4you.com
 */
#include <ESP8266WiFi.h>
#include <WiFiClient.h>


#include <Time.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Bounce2.h>

//ESP Web Server Library to host a web page
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

//Static IP address configuration
IPAddress staticIP(192, 168, 100, 123); //ESP static ip
IPAddress gateway(192, 168, 100, 1);   //IP Address of your WiFi Router (Gateway)
IPAddress subnet(255, 255, 255, 0);  //Subnet mask
IPAddress dns1(8, 8, 8, 8);  //DNS
IPAddress dns2(8, 8, 4, 4);  //DNS

// Master info
String url_master = "http://192.168.100.52";

const char* device_name = "light1";

//OLED settings
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

//On board LED Connected to GPIO2
#define white_warm_pin D4
#define white_cool_pin D3
#define fan_pin D5

// Input settings
#define MODE_BUTTON D7
Bounce bounce = Bounce();
unsigned int but_cnt = 0;

//SSID and Password of your WiFi router
const char* ssid = "526Beacon";
const char* password = "simasucks";

// Light fading settings
unsigned int slow_interval = 20;
unsigned int warm_level_cur = 0;
unsigned int warm_level_set = 400;
unsigned int cool_level_set = 800;
unsigned long time_last = 0;

// General light/timer settings
boolean timerMode = true;
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

// Prototypes
void setLightLevelSmoothly();
void handleLightTimer();
void setLights();
void turnOffOnboardLEDs();

// Time settings
WiFiUDP UDP;                     // Create an instance of the WiFiUDP class to send and receive
IPAddress timeServerIP;          // time.nist.gov NTP server address
const char* NTPServerName = "time.nist.gov";
const int NTP_PACKET_SIZE = 48;  // NTP time stamp is in the first 48 bytes of the message
byte NTPBuffer[NTP_PACKET_SIZE]; // buffer to hold incoming and outgoing packets

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
  WiFi.hostname(device_name);      // DHCP Hostname (useful for finding device for static lease)
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
  server.on("/lightlevel", handleMessages); //as Per  <a href="ledOn">, Subroutine to be called

  server.begin();                  //Start server
  Serial.println("HTTP server started");

  // Get time from NTP server
  startUDP();

  if(!WiFi.hostByName(NTPServerName, timeServerIP)) { // Get the IP address of the NTP server
    Serial.println("DNS lookup failed. Rebooting.");
    Serial.flush();
    ESP.reset();
  }
  
  Serial.print("Time server IP:\t");
  Serial.println(timeServerIP);
  
  Serial.println("\r\nSending NTP request ...");
  sendNTPpacket(timeServerIP); 
  
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
  handleLightTimer();             // Set light level if in timer mode
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

     setLights();
     updateDisplay();
  }
}

void startUDP() {
  Serial.println("Starting UDP");
  UDP.begin(123);                          // Start listening for UDP messages on port 123
  Serial.print("Local port:\t");
  Serial.println(UDP.localPort());
  Serial.println();
}

uint32_t getNTPTime() {
  if (UDP.parsePacket() == 0) { // If there's no response (yet)
    return 0;
  }
  UDP.read(NTPBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
  // Combine the 4 timestamp bytes into one 32-bit number
  uint32_t NTPTime = (NTPBuffer[40] << 24) | (NTPBuffer[41] << 16) | (NTPBuffer[42] << 8) | NTPBuffer[43];
  // Convert NTP time to a UNIX timestamp:
  // Unix time starts on Jan 1 1970. That's 2208988800 seconds in NTP time:
  const uint32_t seventyYears = 2208988800UL;
  // subtract seventy years:
  uint32_t UNIXTime = NTPTime - seventyYears;
  return UNIXTime;
}

void sendNTPpacket(IPAddress& address) {
  memset(NTPBuffer, 0, NTP_PACKET_SIZE);  // set all bytes in the buffer to 0
  // Initialize values needed to form NTP request
  NTPBuffer[0] = 0b11100011;   // LI, Version, Mode
  // send a packet requesting a timestamp:
  UDP.beginPacket(address, 123); // NTP requests are to port 123
  UDP.write(NTPBuffer, NTP_PACKET_SIZE);
  UDP.endPacket();
}

//===============================================================
// This routine is executed when you open its IP in browser
//===============================================================

void handleMessages() { 
  Serial.println("Processing Light Level Data...");
 for (int i = 0; i < server.args(); i++) {
   Serial.print(String(i) + " ");  //print id
   Serial.print("\"" + String(server.argName(i)) + "\" ");  //print name
   Serial.println("\"" + String(server.arg(i)) + "\"");  //print value
   if (server.argName(i) == "warm") {
    warm_level_set = server.arg(i).toInt();
   }
   if (server.argName(i) == "cool") {
    cool_level_set = server.arg(i).toInt();
   }

   if (server.argName(i) == "hour1") {
    hour1 = server.arg(i).toInt();
   }

   if (server.argName(i) == "hour2") {
    hour2 = server.arg(i).toInt();
   }

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
      Serial.println("Switching mode to TIMER");
    } else {
      timerMode = false;
      Serial.println("Turning TIMER mode off");
    }
   }
   
   
   setLights();
   updateDisplay();
   
 }
 server.send(200, "text/html", "Got it."); //Send ADC value only to client ajax request
}

// Always running, to ramp to set level
void setLightLevelSmoothly() {
  if (warm_level_cur != warm_level_set) {
    if ((millis() - time_last) > slow_interval) {
      warm_level_cur < warm_level_set ? warm_level_cur++ : warm_level_cur--;
      analogWrite(white_warm_pin, warm_level_cur);
      time_last = millis();
    }
  }
}

// Set light level as needed
void handleLightTimer() {
  if ((millis() - time_last_checked) < 10000) {
    return;
  }

  time_last_checked = millis();

  if (!timerMode) {
    return;
  }
  /*
  if ( (now.hour() >= hour1) && (now.hour() < hour2) ) {
    // will setup to follow an intensity curve in the future
    Serial.println("Inside hour bounds, lights ON");
    analogWrite(white_cool_pin, cool_level_set);
    analogWrite(white_warm_pin, warm_level_set);
    if (!(fan_off) && (fan_timer)) {
      analogWrite(fan_pin, fan_level_day);
      //light_status = true;
    }
  } else {
    Serial.println("Outside hour bounds, lights OFF");
    analogWrite(white_cool_pin, 0);
    analogWrite(white_warm_pin, 0);
    if (!(fan_off) && (fan_timer)) {
      analogWrite(fan_pin, fan_level_night);
      //light_status = false;
    }
  }
  */
  
  
}

// Get time from master
void updateTime() {
  uint32_t NTPTime = getNTPtime();
}
void getTime() {
  if (WiFi.status() == WL_CONNECTED) {

    HTTPClient http;

    String server_request = url_master + "/time?clientName=" + device_name;
    Serial.println("Sending this request: " + server_request);
    http.begin(server_request.c_str());

    int http_response_code = http.GET();

    if (http_response_code == 200) {
      Serial.println("Good response");
      int resp_size = http.getSize();
      Serial.println(resp_size);
      http.getString(); // get the payload
      //Serial.println(payload);
    } else {
      //Serial.println("Bad response");
    }
    Serial.println("Closing connection");
    yield();
     http.end();
    Serial.println("Connection closed");
    
  }
}

// Periodic update catch-all
void perodicUpdates() {
  if ((millis() - time_last_update) > 10000) {
    //getTime(); // not working
    updateTime();
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
  display.print(F("Cool:"));
  display.print(" ");
  display.print(cool_level_set);
  display.print(F(" Warm:"));
  display.print(" ");
  display.println(warm_level_set);

  display.print(F("On: "));
  display.print(hour1);
  display.print(F(" Off: "));
  display.println(hour2);

  display.print(F("Timer: "));
  if (timerMode) {
    display.println(F("ON"));
  } else {
    display.println(F("OFF"));
  }

  display.println(WiFi.localIP());

  display.display();
}

// Set lights
void setLights() { 
  setLights(true);
}
void setLights(boolean lights_on) {
  Serial.println("Setting light level inside setLights()");

  if (!lights_on) {
    analogWrite(white_cool_pin, 0);
    analogWrite(white_warm_pin, 0);
    return;
  }
  // otherwise:
  if (!timerMode && manual_on) {
    analogWrite(white_cool_pin, cool_level_set);
    analogWrite(white_warm_pin, warm_level_set);
  }

  if (!timerMode && !manual_on) {
    setLights(false);
  }
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
