#include <ESP8266WiFi.h>
#include <WiFiClient.h>

// Replace with your SSID and password details
char ssid[] = "bill wi the science fi";        
char pass[] = "581321345589";

const char server[] = "www.bryanrolfe.com";
String key = "bobs_your_uncle";

WiFiClient client;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(74880);
  
  WiFi.begin(ssid,pass);
  Serial.println("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi Connected");
  printWiFiStatus();

}

void loop() {
  // put your main code here, to run repeatedly:
  //makeGetRequest(30.2);
  delay(30000);
}

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
