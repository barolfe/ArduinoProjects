#include <NewPing.h>

/*
 * created by Rui Santos, https://randomnerdtutorials.com
 * 
 * Complete Guide for Ultrasonic Sensor HC-SR04
 *
    Ultrasonic sensor Pins:
        VCC: +5VDC
        Trig : Trigger (INPUT) - Pin11
        Echo: Echo (OUTPUT) - Pin 12
        GND: GND
 */

int led1    = 9;     //pwm for led 1
//int led2    = 3;     //pwm for led 2
unsigned long brightness = 0;
int led1Brightness = 0;
int maxBrightness = 150;
long lastTime, thisTime;
long loopTime = 50; // ms, how often main code should execute

int trigPin = 2;    // Trigger
int echoPin = 3;    // Echo
long cm, inches;
long duration;

//NewPing sonar(trigPin, echoPin, 500);
 
void setup() {
  //Serial Port begin
  Serial.begin (115200);
  //Define inputs and outputs
  pinMode(led1, OUTPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  thisTime = millis();
  lastTime = millis();
}
 
void loop() {
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:

  if ((millis() - lastTime) > loopTime) {
    thisTime = millis();
    lastTime = thisTime;
   

    // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
    digitalWrite(trigPin, LOW);
    delayMicroseconds(5);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    pinMode(echoPin, INPUT);
    duration = checkPulseLength();
    
    // Read the signal from the sensor: a HIGH pulse whose
    // duration is the time (in microseconds) from the sending
    // of the ping to the reception of its echo off of an object.
    //pinMode(echoPin, INPUT);
    //duration = pulseIn(echoPin, HIGH);

    
   
    // Convert the time into a distance
    cm = (duration/2) / 29.1;     // Divide by 29.1 or multiply by 0.0343

    //cm = sonar.ping_cm();

    Serial.print(brightness);
    Serial.print("us, ");
    Serial.print(cm);
    Serial.print("cm");
    Serial.println();

    if (cm < 40) {
      brightness = ((40.0 - cm) / 40.0)*150.0`;
    } else {
      brightness = 0;
    }
  
    analogWrite(led1, brightness);
    

  /*
    if (cm < 40) {
      activateLEDs();
    } else {
      deactivateLEDs();
    }
    */
  }
  
  //delay(250);
}

unsigned long checkPulseLength() {
  bool notDone = true;
  unsigned long pulseStart, pulseEnd;
  int oldState = 0;
  long n = 0;
  while (notDone) {
    if ((digitalRead(echoPin) == HIGH) && (oldState == 0)) {
      oldState = 1; 
      pulseStart = micros();
    } else if ((digitalRead(echoPin) == LOW) && (oldState == 1)) {
      oldState = 0;
      notDone = false;
      pulseEnd = micros();
      Serial.println("done");
    }
    n = n + 1;
    if (n >  1000) {
      pulseEnd = micros();
      break;
    }
  }  
  return (pulseEnd - pulseStart);
}

void activateLEDs() {
  if (led1Brightness < maxBrightness) {
    led1Brightness = led1Brightness + 1;
    analogWrite(led1, led1Brightness);
  }
}

void deactivateLEDs() {
  if (led1Brightness > 0) {
    led1Brightness = led1Brightness - 1;
    analogWrite(led1, led1Brightness);
  }
}
