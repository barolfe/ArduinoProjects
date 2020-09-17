/*
 * ESP8266 NodeMCU Real Time Graphs Demo
 * Updates and Gets data from webpage without page refresh
 * https://circuits4you.com
 */
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include "FS.h" //File System headers
#include "config.h" // information for Victron 
#include <SoftwareSerial.h>

#define LED 2  //On board LED

//SSID and Password of your WiFi router
const char* ssid = "ESP8266";
const char* password = "ESP8266Test";

// Software Serial Variables
#define rxPin D7                           // rx pin
#define txPin D8                           // transmit pin (not used)
SoftwareSerial victronSerial(rxPin, txPin); // initialize softare serial for Victron

// Victron VE.direct variables
char receivedChars[buffsize];                       // an array to store the received data
char tempChars[buffsize];                           // an array to manipulate the received data
char recv_label[num_keywords][label_bytes]  = {0};  // {0} tells the compiler to initalize it with 0. 
char recv_value[num_keywords][value_bytes]  = {0};  // That does not mean it is filled with 0's
char victronValues[num_keywords][value_bytes]       = {0};  // The array that holds the verified data
static byte blockindex = 0;
bool new_data = false;
bool blockend = false;
bool bSkip    = false;
bool bVictronConnected = false;

// Stuff for file handling
long lastDataStoreTime = -300000;    
long lastDataCheck = -30000;            
unsigned long storeEvery = 300000;                  // time interval in ms in which to store data
unsigned long checkEvery = 30000;
bool bError = false;
File f;

ESP8266WebServer server(80); //Server on port 80
IPAddress    apIP(42, 42, 42, 1);  // Static IP Address

//===============================================================
// This routine is executed when you open its IP in browser
//===============================================================
void handleRoot() {
  server.sendHeader("Location", "/index.html",true);   //Redirect to our html web page
  server.send(302, "text/plane","");
}

void handleWebRequests(){
  if(loadFromSpiffs(server.uri())) return;
  String message = "File Not Detected\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " NAME:"+server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  Serial.println(message);
}

// Function can't be blocking -- will crash the ESP8266 (needs work)
void handleLotsOfData() {
  if (!f) {
    f = SPIFFS.open("/data.txt","r");
  }
  if (f) {
    Serial.println("File opened");
    char buff[100];
    size_t buffSize = 100;
    if (f.available()) {
      f.readBytes(buff, buffSize);
    } else {
      f.close();
      Serial.println("File closed.");
    }
    //Serial.print(payload);
    server.send(200, "text/plane", buff); //Send ADC value only to client ajax request
  } else {
      Serial.println("Unable to access file");
  }   
}

void handleData() {
  Serial.println("Data request recieved.");
    Serial.println("Sending data.");
    Serial.println(victronValues[SOC]);
    server.send(200, "text/plane", victronValues[SOC]); //Send data value only to client request
}

void handleTestData() {
    Serial.println("Sending test data.");  
    Serial.println();
    char buff[16];
    char * packet = ltoa(millis(), buff, 10);
    Serial.println(packet);
    server.send(200, "text/plane", packet); //Send ADC value only to client ajax request
}

void handleSentVar() {
  //if (server.hasArg("cmd")) { // this is the variable sent from the client
    String argument = server.arg("cmd");
    server.send(200, "text/html", "Data received");
    Serial.print("Data received: ");
    Serial.println(argument);
    checkFileContents("/data.txt");
  //}
}

//==============================================================
//                  SETUP
//==============================================================
void setup(void){
  Serial.begin(19200);
    // Victron VE.direct stuff
  victronSerial.begin(19200);

  Serial.println("Program begin.");
  
  // Setup FileSystem 
  SPIFFS.begin();
  Serial.println("File system initialized.");
  f = SPIFFS.open("/data.txt","a");
  if (f) {
    Serial.println("File opened for data storage");
  } else {
    Serial.println("Unable to open file, will not save data.");
  }
  
  // Setup Access Point
  WiFi.softAPConfig(apIP,apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  Serial.println("Access point started");

  // Setup server responses
  server.on("/", handleRoot);      //Which routine to handle at root location. This is display page
  server.on("/readLotsOfData",handleLotsOfData); // request for a large payload of data
  server.on("/readData",handleData); // for a single value of data
  server.on("/readTestData",handleTestData); // for a single value of data
  server.on("/cmd",handleSentVar); // for commands
  
  server.onNotFound([]() {         // If the client requests any URI
    handleWebRequests();           // send it if it exists
  });
  server.begin();                  //Start server
  Serial.println("HTTP server started");

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
}
//==============================================================
//                     LOOP
//==============================================================
void loop(void){
  server.handleClient();          //Handle client requests

  if (bVictronConnected) {
    if ((millis() - lastDataCheck) > checkEvery) {
      RecvWithEndMarker();            //Check and collect data from VE.direct (Victron)
      HandleNewData();                //Handle data from VE.direct (Victron)
    }
  }
}

//==============================================================
//                     Support Functions
//==============================================================

bool loadFromSpiffs(String path){
  String dataType = "text/plain";
  if(path.endsWith("/")) path += "index.htm";
 
  if(path.endsWith(".src")) path = path.substring(0, path.lastIndexOf("."));
  else if(path.endsWith(".html")) dataType = "text/html";
  else if(path.endsWith(".htm")) dataType = "text/html";
  else if(path.endsWith(".css")) dataType = "text/css";
  else if(path.endsWith(".js")) dataType = "application/javascript";
  else if(path.endsWith(".png")) dataType = "image/png";
  else if(path.endsWith(".gif")) dataType = "image/gif";
  else if(path.endsWith(".jpg")) dataType = "image/jpeg";
  else if(path.endsWith(".ico")) dataType = "image/x-icon";
  else if(path.endsWith(".xml")) dataType = "text/xml";
  else if(path.endsWith(".pdf")) dataType = "application/pdf";
  else if(path.endsWith(".zip")) dataType = "application/zip";
  else if(path.endsWith(".txt")) dataType = "text/html";
  File dataFile = SPIFFS.open(path.c_str(), "r");
  if (server.hasArg("download")) dataType = "application/octet-stream";
  if (server.streamFile(dataFile, dataType) != dataFile.size()) {
  }
 
  dataFile.close();
  return true;
}

// Serial Handling for VE.direct (Victron)
// ---
// This block handles the serial reception of the data in a 
// non blocking way. It checks the Serial line for characters and 
// parses them in fields. If a block of data is send, which always ends
// with "Checksum" field, the whole block is checked and if deemed correct
// copied to the 'value' array. 

void RecvWithEndMarker() {
    static byte ndx = 0;
    char endMarker = '\n';
    char rc;

    while (victronSerial.available() > 0 && new_data == false) {
        rc = victronSerial.read();
        //Serial.print(rc);
        if (rc != endMarker) {
            receivedChars[ndx] = rc;
            ndx++;
            if (ndx >= buffsize) {
                ndx = buffsize - 1;
            }
        }
        else {
            receivedChars[ndx] = '\0'; // terminate the string
            ndx = 0;
            new_data = true;
        }
        yield();
    }
}

void HandleNewData() {
    // We have gotten a field of data 
    if (new_data == true) {
        //Copy it to the temp array because parseData will alter it.
        //Serial.println("str copy..");
        strcpy(tempChars, receivedChars);
        //Serial.println(tempChars); // barolfe -- debug
        //Serial.println("Parse data..");
        ParseData();
        saveData();
        new_data = false;
    }
}

void saveData() {
  // Check if enough time has elapsed
  if ((millis() - lastDataStoreTime) > storeEvery) {
    // Store data
    if (!f) {
      f = SPIFFS.open("/data.txt","a");
    }
    Serial.println(victronValues[SOC]);
    if (f) {
      f.println(victronValues[SOC]); // save the state of charge (SOC)
      Serial.println("Data saved.");
      f.close();
      blinkLED(2);
    } else {
      Serial.println("Unable to save data");
      bError = true;
    }
    //checkFileContents("/data.txt");
    lastDataStoreTime = millis();
  }

  
}

void checkFileContents(String filename) {
  if (f) { 
    f.close();
    f = SPIFFS.open(filename,"r");
  }
  if (!f) {
    Serial.println("Opening file for reading...");
    f = SPIFFS.open(filename,"r");
    if (f) {
      Serial.println("File opened.");
    } else {
      Serial.println("File open failed.");
    }
  }
  if (f) {
    Serial.println("File contents:");
    while (f.available()) {
      Serial.println(f.readStringUntil('\n'));
      yield();
    }
    f.close();
  }
}

void ParseData() {
    //Serial.println("Parsing data");
    char * strtokIndx; // this is used by strtok() as an index
    strtokIndx = strtok(tempChars,"\t");      // get the first part - the label

    // We only want blocks that begin with "PID"
    //Serial.println("Check block");
    //Serial.println(strtokIndx);
    //Serial.println("good");

    // This check against NULL is necessary, as sometimes strtok returns null
    // especially when something interrupts the data parsing/serial connection
    // Without this, the program will crash on the strcmp call.
    if (strtokIndx != NULL) {
      if (strcmp(strtokIndx,"H1") == 0) { 
        bSkip = true;
      } else if (strcmp(strtokIndx,"PID") == 0) {
        bSkip = false;
      }
  
      if (!bSkip) {
        // The last field of a block is always the Checksum
        if (strcmp(strtokIndx, "Checksum") == 0) {
            blockend = true;
        }
        strcpy(recv_label[blockindex], strtokIndx); // copy it to label
    
        // Now get the value
        // Why does this work?
        strtokIndx = strtok(NULL, "\r");   // This continues where the previous call left off until '/r'.
        if (strtokIndx != NULL) {          // We need to check here if we don't receive NULL.
            strcpy(recv_value[blockindex], strtokIndx);
        }
        blockindex++;
      
        if (blockend) {
           //Serial.println("Blockend reached");
          
            // We got a whole block into the received data.
            // Check if the data received is not corrupted.
            // Sum off all received bytes should be 0;
            byte checksum = 0;
            for (int x = 0; x < blockindex; x++) {
                // Loop over the labels and value gotten and add them.
                // Using a byte so the the % 256 is integrated. 
                char *v = recv_value[x];
                char *l = recv_label[x];
                //Serial.println(recv_label);
                while (*v) {
                    checksum += *v;
                    v++;
                }
                while (*l) {
                    checksum+= *l;
                    l++;
                }
                // Because we strip the new line(10), the carriage return(13) and 
                // the horizontal tab(9) we add them here again.  
                checksum += 32;
            }
            // Checksum should be 0, so if !0 we have correct data.
            
            if (!checksum) {
            //if (true) {
                // Since we are getting blocks that are part of a 
                // keyword chain, but are not certain where it starts
                // we look for the corresponding label. This loop has a trick
                // that will start searching for the next label at the start of the last
                // hit, which should optimize it. 
                int start = 0;
                for (int i = 0; i < blockindex; i++) {
                  for (int j = start; (j - start) < num_keywords; j++) {
                    if (strcmp(recv_label[i], keywords[j % num_keywords]) == 0) {
                      // found the label, copy it to the value array
                      strcpy(victronValues[j], recv_value[i]);
                      start = (j + 1) % num_keywords; // start searching the next one at this hit +1
                      break;
                    }
                  }
                }
            }
            
            // Reset the block index, and make sure we clear blockend.
            blockindex = 0;
            blockend = false;
            //PrintValues();
            // Reset the last-check clock so that the code will check again when ready
            lastDataCheck = millis();
            Serial.println(victronValues[SOC]);
            blinkLED(1);
            //saveData();
        }
      } // end if (!bskip)
    } else {  // end if (strtokIndx != NULL) 
      Serial.println("NULL returned by strtok()");
    }
}

// Crude blocking LED blink implementation
void blinkLED(int num) {
  for (int i = 0; i < num; i++) {
    digitalWrite(LED_BUILTIN, LOW);
    delay (300);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    yield();
  }
}

void PrintValues() {
    for (int i = 0; i < num_keywords; i++){
        Serial.print(keywords[i]);
        Serial.print(",");
        Serial.println(victronValues[i]);
    }
}
