/*
    This sketch sends a message to a TCP server

*/

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

ESP8266WiFiMulti WiFiMulti;
WiFiClient client;

int nRed = 0, nGreen = 0;
int led1 = 5, led2 = 4;

void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);

  // We start by connecting to a WiFi network
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("Tarka", "581321345589");

  Serial.println();
  Serial.println();
  Serial.print("Wait for WiFi... ");

  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  delay(500);

  // Use WiFiClient class to create TCP connections
  const uint16_t port = 80;
  const char * host = "192.168.4.1"; // ip or dns
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, port)) {
    Serial.println("connection failed");
    Serial.println("wait 5 sec...");
    delay(5000);
    return;
  }
}


unsigned long tLastTalk = 0;
void loop() {

  // This will send the request to the server

  
  blinkGreenLED();
  blinkRedLED();

  if ((millis() - tLastTalk) > 5000) {
    client.println("ON");
    nGreen = 1;
  
    //read back one line from server
    String line = client.readStringUntil('\r');
    Serial.println(line);
    if (line=="OK") {
        for (int i = 0; i < 5; i++) {
          digitalWrite(led2, HIGH);
          delay(100);
          digitalWrite(led2, LOW);
          delay(100);
        }
    } else {
        for (int i = 0; i < 5; i++) {
          digitalWrite(led1, HIGH);
          delay(100);
          digitalWrite(led1, LOW);
          delay(100);
        }
    }
  
  //  Serial.println("closing connection");
  //  client.stop();
  
    Serial.println("wait 5 sec...");
    tLastTalk = millis();
    //delay(5000);
  }
}


unsigned long tLast1, tLast2;
bool bOn1 = false, bOn2 = false;

void blinkRedLED() {
  if ((nRed) > 0) {
     if (((millis() - tLast1) > 1000) && !bOn1) {
      bOn1 = true;
      tLast1 = millis();
      digitalWrite(led1, HIGH);
      digitalWrite(led2, HIGH);
    }
    if (((millis() - tLast1) > 100) && bOn1) {
      nRed = nRed - 1;
      bOn1 = false;
      digitalWrite(led1, LOW);
      digitalWrite(led2, LOW);
    }
  }
}

void blinkGreenLED() {
  if ((nGreen) > 0) {
     if (((millis() - tLast2) > 1000) && !bOn2) {
      bOn2 = true;
      tLast2 = millis();
      digitalWrite(led2, HIGH);
    }
    if (((millis() - tLast2) > 100) && bOn2) {
      nGreen = nGreen - 1;
      bOn2 = false;
      digitalWrite(led2, LOW);
    }
  }
}
