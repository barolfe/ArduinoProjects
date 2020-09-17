#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// GPIO where the DS18B20 is connected to
const int oneWireBus = D1;     

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

// Replace with your SSID and password details
char ssid[] = "bill wi the science fi";        
char pass[] = "581321345589";

const char server[] = "www.bryanrolfe.com";
String key = "bobs_your_uncle";

WiFiClient client;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(74880);

  pinMode(D2, OUTPUT);
  
  
  WiFi.begin(ssid,pass);
  Serial.println("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi Connected");
  printWiFiStatus();

  // Start the DS18B20 Sensor
  sensors.begin();

 
  for(int i = 0; i < 1; i++) {
    digitalWrite(D2, HIGH);
    delay(300);
    digitalWrite(D2, LOW);
    delay(200);
  }
  
  

  Serial.println("Getting temp");
  float temp = getTemp(1);

  Serial.println("Making HTTP Request");
  makeGetRequest(temp);
  
  Serial.println(temp);

  int voltage = analogRead(A0);
  float v_mes = 4.2 * (voltage / 1023.0);

  Serial.println(v_mes);
  Serial.println(voltage);

  ESP.deepSleep(5*60e06); // wake up every few minutes
}

void loop() {

  // do nothing
  //delay(30000);
}

void makeGetRequest(float temp) {
  // close any connection before send a new request to allow client make connection to server
  client.stop();

  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("connecting...");

    // send the HTTP PUT request:
    client.println("GET /projects/temperaturelogger/process_data.php?key=" + key + "&temp=" + temp + "&id=2 HTTP/1.1");
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

float getTemp(int n) {
  float t_avg = 0;
  for (int i = 0; i < n; i ++) {
    sensors.requestTemperatures(); 
    float temperature = sensors.getTempFByIndex(0);
    t_avg += temperature;
    delay(10);
  }
  t_avg = t_avg / n;
  return t_avg;
}
