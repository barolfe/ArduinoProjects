 /*
  * Author: Emmanuel Odunlade 
  * Complete Project Details http://randomnerdtutorials.com
  */
  
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "FastLED.h"

#define LED_PIN D4
#define NUM_DAYS 7
#define NUM_LEDS 16
#define BRIGHTNESS 255
#define GREEN_LED D5
#define RED_LED D6

CRGB leds[NUM_DAYS*2 + 2]; // 7 leds for skies, 7 leds for temp, 2 leds for today

// Color pallete for temperature
DEFINE_GRADIENT_PALETTE( colormap ) {
  0,   228,  0, 255,   //purple
80,   0,  0,  255,   //blue
170,   255, 200, 0,   // yellow /orange
255,   255,100,  0};   // red

CRGBPalette16 tempPalette = colormap;
int gHue = 0;

// Reasonable temperature range (for Portland)
int temp_min = 55;
int temp_max = 95; 


// Debug settings, etc
boolean bColorTest = false;
boolean bConnect = !bColorTest; // Set to true for non-testing purposes
boolean bColorForWeather = true;

// Replace with your SSID and password details
char ssid[] = "526Beacon";        
char pass[] = "simasucks";   

WiFiClient client;

// Open Weather Map API server name
const char server[] = "api.openweathermap.org";

// How your nameOfCity variable would look like for Lagos on Nigeria
//String nameOfCity = "Lagos,NG"; 
//String lat = "45.5";
//String lon = "-122.9";
String lat = "33.876";
String lon = "-118.404";

// Replace the next line with your API Key
String apiKey = "b478cc53d70c2f7d6ce3c211e2e6deeb"; 

int jsonend = 0;
boolean startJson = false;
boolean bFirstCall = true;
int status = WL_IDLE_STATUS;

int forecast_weather[NUM_DAYS + 1];
double forecast_temp[NUM_DAYS + 1];

// moved text to global space because of what appeared to be stack overflow
#define JSON_BUFF_DIMENSION 3000
char text[JSON_BUFF_DIMENSION];

unsigned long lastConnectionTime = 10 * 60 * 1000;     // last time you connected to the server, in milliseconds
const unsigned long postInterval = 10 * 60 * 1000;  // posting interval of 10 minutes  (10L * 1000L; 10 seconds delay for testing)

void setup() {
  Serial.begin(115200);
  
  WiFi.begin(ssid,pass);
  Serial.println("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi Connected");
  printWiFiStatus();

  // Setup the WS2811 LEDs:
  FastLED.addLeds<WS2811, LED_PIN, BRG>(leds, NUM_LEDS); // Note the BRG coloring order

  // setup status LEDs
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);

  red_led(true, 3);
}

void loop() { 
  if (bColorTest) {
    colorTestRoutine();
  }
  if (bConnect) {
    if ((millis() - lastConnectionTime > postInterval) || (bFirstCall)) {
      // note the time that the connection was made:
      lastConnectionTime = millis();
      bFirstCall = false;
      makehttpRequest("forecast");
      makehttpRequest("weather");
      updateLEDs();
      Serial.println("Back to main loop");
    }
  }
  
  ESP.deepSleep(600e06);
  //delay(10000);
}

// print Wifi status
void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

// to request data from OWM
void makehttpRequest(char * type) {
  // close any connection before send a new request to allow client make connection to server
  client.stop();

  Serial.print("Making http request of type: ");
  Serial.println(type);

  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("connecting...");
    green_led(true);
    
    // send the HTTP PUT request:
    if (strcmp(type,"forecast") == 0) {
      client.println("GET /data/2.5/forecast/daily?lat=" + lat + "&lon=" + lon + "&APPID=" + apiKey + "&mode=json&units=metric&cnt=" + NUM_DAYS + " HTTP/1.1");
    } else {
      client.println("GET /data/2.5/weather?lat=" + lat + "&lon=" + lon + "&APPID=" + apiKey + "&mode=json&units=metric HTTP/1.1");
    }
    //client.println("GET /data/2.5/weather?q=" + nameOfCity + "&APPID=" + apiKey + "&mode=json HTTP/1.1");
    client.println("Host: api.openweathermap.org");
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

    // clear text buffer
    for (int j = 0; j < JSON_BUFF_DIMENSION; j++) {
      text[j] = ' ';
    }
    
    char c = 0;
    boolean bContinue = true;
    int i = 0;
    
    while (bContinue) {
      if (client.available()) {
        c = client.read();
        //Serial.print(c);
        // since json contains equal number of open and close curly brackets, this means we can determine when a json is completely received  by counting
        // the open and close occurences,
        //Serial.print(c);
        if (c == '{') {
          startJson = true;         // set startJson true to indicate json message has started
          jsonend++;
        }
        if (c == '}') {
          jsonend--;
        }
        if (startJson == true) {
          text[i] = c;
          i++;
        }

        if (i > JSON_BUFF_DIMENSION) { 
          Serial.println("ERROR: data returned larger than buffer size");
        }
        
        // if jsonend = 0 then we have have received equal number of curly braces 
        if (jsonend == 0 && startJson == true) {
          Serial.print("Json String Received in-full: ");
          Serial.print(i);
          Serial.println(" characters");
          parseJson(text, type);  // parse c string text in parseJson function
          //text = "";                // clear text string for the next time
          startJson = false;        // set startJson to false to indicate that a new message has not yet started
          bContinue = false;
        }
      } else {
         Serial.println("Client no longer available, checking...");
         long timeNow = millis();
         while (!client.available() && (millis() - timeNow < 500)) {
         }
         if (!client.available()) {
            bContinue = false;
            Serial.println("No further data available");
         }
      }
    }
    Serial.println("all done with GET request"); 
    client.stop();
    green_led(false);
  } else {
    // if no connction was made:
    Serial.println("connection failed");
    return;
  }
  Serial.println("Done with makehttpRequest");
  
  Serial.println("Still alive!");
}

//to parse json data recieved from OWM
void parseJson(const char * jsonString, char * type) {
  //Serial.println(jsonString);
  //StaticJsonDocument<8000> jsonBuffer;
  const size_t bufferSize = 6000;
  DynamicJsonDocument jsonBuffer(bufferSize);

  // FIND FIELDS IN JSON TREE
  DeserializationError err = deserializeJson(jsonBuffer, jsonString);
  
  if (err) {
    Serial.println("deserializeJson() failed");
    Serial.println(err.c_str());
    return;
  }

  if (strcmp(type,"forecast") == 0) {
    JsonArray forecast = jsonBuffer["list"];
    checkWeatherStatus(forecast);  
  } else {
    JsonArray weather = jsonBuffer["weather"];
    JsonObject main    = jsonBuffer["main"];
    checkTodayWeather(weather, main); 
  }
  

  jsonBuffer.clear();
  
  Serial.println("done with Json Parsing");
}


void colorTestRoutine() {
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
  FastLED.show();
  delay(50);
  gHue++; 
}

void checkTodayWeather(JsonArray &weather, JsonObject &main) {
  double temp = main["temp"];
  Serial.println(temp);
  const char * skies = weather[0]["main"];
  const char * detail = weather[0]["description"];
  forecast_weather[NUM_DAYS] = getSkyID(skies, detail);
  forecast_temp[NUM_DAYS] = CtoF(temp);

  Serial.println("Current weather:");
  Serial.print(" - ");
  Serial.print(skies);
  Serial.print(", ");
  Serial.print(forecast_temp[NUM_DAYS]);
  Serial.println("F");
  
}

void checkWeatherStatus(JsonArray &forecast) {
  int i = 0;
  for (JsonObject obj : forecast) {
    JsonArray weather = obj["weather"];
    const char * skies = weather[0]["main"];
    const char * detail = weather[0]["description"];
    double temp = obj["temp"]["max"];
    temp = CtoF(temp);
    Serial.print(" - ");
    Serial.print(skies);
    Serial.print(", ");
    Serial.print(temp);
    Serial.println("F");

    forecast_weather[i] = getSkyID(skies, detail);
    forecast_temp[i] = temp;
    i++;
  }
  Serial.println();
  for (int i = 0; i < NUM_DAYS; i++ ) {
    Serial.print(forecast_weather[i]);
    Serial.print(", ");
  }
}

void getFirstString(const char * str, char * buff) { 
  for (int i = 0; i <= strlen(str); i++) {
    if (str[i] == ' ' || str[i] == '\0') { // space or null-terminated
      buff[i] = '\0';
    } else {
      buff[i] = str[i];
    }
  }
}

int getSkyID(const char * skies, const char * detail) {
  
  int n = strlen(detail);
  char detail_word[n];
  getFirstString(detail, detail_word);
  Serial.print("Detail word is: ");
  Serial.println(detail_word);
  
  if (strcmp(skies,"Clear") == 0) {
      return 0; 
  } else if (strcmp(skies,"Clouds") == 0) {
    if ((strcmp(detail_word,"broken") == 0) || (strcmp(detail_word,"few") == 0)  || (strcmp(detail_word,"scattered") == 0)) {
      return 0; // mostly sunny
    }
    return 1;
  } else if (strcmp(skies,"Rain") == 0) {
    return 2;
  } else if (strcmp(skies,"Drizzle") == 0) {
    return 2;
  } else if (strcmp(skies,"Snow") == 0) {
    return 3;
  } else if (strcmp(skies,"Fog") == 0) {
    return 4;
  } else if (strcmp(skies, "Mist") == 0) {
    return 5;
  } else {
    return -1;
  }
}

void updateLEDs() {
  for (int i = 0; i < NUM_DAYS + 1; i++) {
    int n = 0;
    n = (i < NUM_DAYS) ? i : 2 * NUM_DAYS;
    if (forecast_weather[i] == 0) { // clear
      leds[n] = CRGB(255,150,0);
    } else if (forecast_weather[i] == 1) { // clouds
      leds[n] = CRGB(230,150,120);
    } else if (forecast_weather[i] == 2) { // rain
      leds[n] = CRGB(0,0,255);
    } else if (forecast_weather[i] == 3) { // snow
      leds[n] = CRGB(255,0, 180);
    } else if (forecast_weather[i] == 4) { // fog
      leds[n] = CRGB(0, 255, 0);
    } else if (forecast_weather[i] == 5) { // mist
      leds[n] = CRGB(0, 150, 200);
    } else if (forecast_weather[i] == 6) { // broken clouds
      leds[n] = CRGB(255,150,150);
    }else { // -1, or other
      leds[n] = CRGB(255,0,0);
    }
  }
  for (int i = 0; i < NUM_DAYS + 1; i++) {
    int n = 0;
    n = (i < NUM_DAYS) ? 2*NUM_DAYS - i - 1 : 2 *NUM_DAYS+1;
    int val = 255 * ((forecast_temp[i] - temp_min) / (temp_max - temp_min));
    leds[n] = ColorFromPalette(tempPalette, val);
  }
  
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.show();
}

void green_led(boolean state) {
  if (state) {
    digitalWrite(GREEN_LED, 1);
  } else {
    digitalWrite(GREEN_LED, 0);
  }
}

void green_led(boolean state, int blink_n) {
  blink_led(GREEN_LED, blink_n);
}

void red_led(boolean state) {
  if (state) {
    digitalWrite(RED_LED, 1);
  } else {
    digitalWrite(RED_LED, 0);
  }
}

void red_led(boolean state, int blink_n) {
  blink_led(RED_LED, blink_n);
}

void blink_led(int LED, int N) {
  for (int i = 0; i < N; i++) {
    digitalWrite(LED, 1);
    delay(300);
    digitalWrite(LED, 0);
    delay(200);
  }
}

double CtoF(double degC) {
  return (degC * 1.8 + 32);
}
