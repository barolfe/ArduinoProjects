/*
   Copyright (c) 2015, Majenko Technologies
   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

 * * Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.

 * * Redistributions in binary form must reproduce the above copyright notice, this
     list of conditions and the following disclaimer in the documentation and/or
     other materials provided with the distribution.

 * * Neither the name of Majenko Technologies nor the names of its
     contributors may be used to endorse or promote products derived from
     this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* Create a WiFi access point and provide a web server on it. */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

/* Set these to your desired credentials. */
const char *ssid = "Tarka";
const char *password = "581321345589";

ESP8266WebServer server(80);

//Pins for LEDs
int led1 = 5;
int led2 = 4;

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
   connected to this access point to see it.
*/
void handleRoot() {
  server.send(200, "text/html", "<h1>You are connected</h1>");
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);

}

void handleSentVar() {
  if (server.hasArg("sensor_reading")) { // this is the variable sent from the client
    int readingInt = server.arg("sensor_reading").toInt();
    char readingToPrint[5];
    itoa(readingInt, readingToPrint, 10); //integer to string conversion for OLED library
    u8g2.firstPage();
    u8g2.drawUTF8(0, 64, readingToPrint);
    u8g2.nextPage();
    server.send(200, "text/html", "Data received");
  }
}

unsigned long tLast1, tLast2;
bool bOn1 = false, bOn2 = false;

void loop() {
  server.handleClient();

  if (((millis() - tLast) > 1000) && !bOn) {
    bOn = true;
    tLast = millis();
    digitalWrite(led1, HIGH);
    digitalWrite(led2, HIGH);
  }
  if (((millis() - tLast) > 100) && bOn) {
    bOn = false;
    digitalWrite(led1, LOW);
    digitalWrite(led2, LOW);
  }

}

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
