int sensePin = 12;  // bubble sense pin
int buttonPin = 8;  // reset button / arm button
int flashPin = 9;   // pin that controls the flash units
int cameraPin = 10; // Camera shutter pin (not using right now)
int popPin = 4;     // pin that controls servo / nichrome
int servoPin = 5;

int cnt = 0;
int buttonCnt = 0;
int timeDelay = 0;
int times[2] = {0, 0};
int downPosition = 170;
int upPosition   = 90;

bool bNotHit = true;
bool bArmed  = false;

#include <Servo.h>
Servo servo; 

void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
  pinMode(buttonPin, INPUT);
  pinMode(flashPin, OUTPUT);
  pinMode(cameraPin, OUTPUT);
  pinMode(sensePin, OUTPUT);
  pinMode(popPin, OUTPUT);

  digitalWrite(popPin, LOW); // IMPORTANT
  
  digitalWrite(sensePin, LOW); // charge sense pin
  delay(1000);
  pinMode(sensePin, INPUT); // switch to high-impedence

  servo.attach(servoPin);
  executePop();
  delay(500);
  resetPop();
  
  delay(5000);

  Serial.println("ready");
}

void loop() {
  // put your main code here, to run repeatedly:
  if (bNotHit) {
    if (digitalRead(sensePin)) {
      cnt++;
      if (cnt > 10) {
        Serial.println("Contact");
        Serial.println("Armed!");
        delay(2000);
        cnt = 0;
        bArmed  = true;
        bNotHit = false;
        prepCamera(); // now we wait
        //executePop();
      }
    } else { 
      cnt = 0;
    }
  }

  if (bArmed) {
    if (!digitalRead(sensePin)) {
      cnt++;
      if (cnt > 1) {
        triggerFlash(timeDelay);
        Serial.println("Break!");
        bArmed = false;
        delay(500);
        //resetPop();
      }
    } else {
      cnt = 0;
    }
  }

  if (checkButton()) {
    Serial.println("Reset!");
    resetPop();
    delay(500);
    bNotHit = true;
    cnt = 0;
  }

  // Check for an input
  if (Serial.available() > 0) {
    //cnt = 0;
    char buff[32];
    int returned = Serial.readBytesUntil('\n', buff, 32);
    buff[31] = 0;
    char *values = strtok(buff, ",");
    // Check data and convert to int
    if (returned == 0) {
      Serial.println("no valid data sent");
    } else {
      Serial.println("data recieved");
      int i = 0;
      while (values != NULL) {
        int val = atoi(values);
        Serial.println(val);
        times[i] = val;
        i++;
        values = strtok(NULL, ",");
      }
      // check if a valid number of inputs was found
      if (i == 2) {
        timeDelay = times[0];
      }
      
    }
  } // if serial

}

void resetContact() {
  
}

bool checkButton() {
  if (digitalRead(buttonPin)) {
    buttonCnt++;
  } else {
    buttonCnt = 0;
  }
  if (buttonCnt > 10) {
    buttonCnt = 0;
    return true;
  } else { 
    return false;
  }
}

// Function to trigger flashes
void triggerFlash(int triggerTime, int triggerTimeus) {
  if (triggerTime > 0) {
    delayExact(triggerTime, triggerTimeus);
  }
  digitalWrite(flashPin, HIGH);
  delay(20); 
  digitalWrite(flashPin, LOW);
}

void triggerFlash(int triggerTime) {
  triggerFlash(triggerTime, 0);
}

void delayExact(int timeMs, int timeUs) {
  if (timeMs > 0) {
    delay(timeMs);
  }
  if (timeUs > 0) {
    delayMicroseconds(timeUs);
  }
}

// Function to trigger camera
void triggerCamera(int triggerTime) {
  if (triggerTime > 0) {
    delayExact(triggerTime, 0);
  }
  digitalWrite(cameraPin, HIGH);
  delay(10); 
  digitalWrite(cameraPin, LOW);
}

void prepCamera() {
  digitalWrite(cameraPin,HIGH);
  delay(100);
  digitalWrite(cameraPin,LOW);
  delay(10);
}

void executePop() {
  servo.write(downPosition);
  //servo.write(positionPop);
}

void resetPop() {
  servo.write(upPosition);
}



