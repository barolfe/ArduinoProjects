/*
    Victron.Arduino-ESP8266
    A:Pim Rutgers
    E:pim@physee.eu

    Code to grab data from the VE.Direct-Protocol on Arduino / ESP8266.
    Tested on NodeMCU v1.0

    The fields of the serial commands are configured in "config.h"

*/
#include "FS.h"
#include <SoftwareSerial.h>
#include "config.h"

// Serial variables
#define rxPin D7
#define txPin D8                                    // TX Not used
SoftwareSerial victronSerial(rxPin, txPin);         // RX, TX Using Software Serial so we can use the hardware serial to check the ouput
                                                    // via the USB serial provided by the NodeMCU.
char receivedChars[buffsize];                       // an array to store the received data
char tempChars[buffsize];                           // an array to manipulate the received data
char recv_label[num_keywords][label_bytes]  = {0};  // {0} tells the compiler to initalize it with 0. 
char recv_value[num_keywords][value_bytes]  = {0};  // That does not mean it is filled with 0's
char victronValues[num_keywords][value_bytes]       = {0};  // The array that holds the verified data
static byte blockindex = 0;
bool new_data = false;
bool blockend = false;
bool bSkip    = false;

// Stuff for file handling
unsigned long lastDataStoreTime = 0;
unsigned long lastDataCheck = 0;                
unsigned long storeEvery = 5000;                  // time interval in ms in which to store data
//File f;
File data;

void setup() {
    // Open serial communications and wait for port to open:
    Serial.begin(19200);
    victronSerial.begin(19200);
    SPIFFS.begin();

    pinMode(LED_BUILTIN, OUTPUT); // enable built-in LED for debug
    digitalWrite(LED_BUILTIN, LOW);
    
    delay(500);

    //Temporary to emulate data stream
    data = SPIFFS.open("/test.txt","r");
    if (data) {
      Serial.println("File opened");
    } else {
      Serial.println("Unable to open file");
    }
    
    /*
    f = SPIFFS.open("/data.txt","r");
    if (!f) {
      Serial.println("File does not exist, creating..");
      f = SPIFFS.open("/data.txt","w");
      f.println("START");
      f.close();
      Serial.println("File created");
    }
    */
}

void loop() {
    // Receive information on Serial from MPPT
    //if ((millis() - lastDataCheck) > storeEvery) {
      RecvWithEndMarker();
      HandleNewData();
    //} 
}

// Serial Handling
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
      //while (data.available() > 0 && new_data == false) {
          rc = victronSerial.read();
          //rc = data.read();
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
          strcpy(tempChars, receivedChars); // receivedChars contains a string of characters read in from serial
          //Serial.println(tempChars);
          ParseData();
          new_data = false;
      }
}

/*
void saveData() {
  // Check if enough time has elapsed
    // Store data
    if ((millis() - lastDataStoreTime) > storeEvery) {
      f = SPIFFS.open("/data.txt","a");
      Serial.println(victronValues[SOC]);
      if (f) {
        f.println(victronValues[SOC]); // save the state of charge (SOC)
        Serial.println("Data saved.");
        f.close();
      } else {
        Serial.println("Error: Save file not available");
      }
      lastDataStoreTime = millis();
    }

  //checkFileContents("/data.txt");
}
*/
/*
void checkFileContents(String filename) {
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
      Serial.print(f.read());
      yield();
    }
  }
}*/


void ParseData() {
    //Serial.println("Parsing data");
    char * strtokIndx; // this is used by strtok() as an index
    strtokIndx = strtok(tempChars,"\t");      // get the first part - the label

    if (strcmp(strtokIndx,"H1") == 0) { 
      bSkip = true;
    } else if (strcmp(strtokIndx,"PID") == 0) {
      bSkip = false;
    }
  
    if (!bSkip) {
      //Serial.println(strtokIndx);
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
          Serial.println(checksum);
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
          PrintValues();
          // Reset the last-check clock so that the code will check again when ready
          //lastDataCheck = millis();
          //Serial.println(victronValues[SOC]);
          //saveData();
      }
    } // end if (!bskip)
}

void PrintEverySecond() {
    static unsigned long prev_millis;
    if (millis() - prev_millis > 1000) {
        PrintValues();
        prev_millis = millis();
    }
}


void PrintValues() {
    for (int i = 0; i < num_keywords; i++){
        Serial.print(keywords[i]);
        Serial.print(",");
        Serial.println(victronValues[i]);
    }
}
