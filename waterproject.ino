#include <Servo.h>
#include <SoftwareSerial.h>
#include <avr/pgmspace.h>

struct delayPair {
    int water;
    int motor;
};

// rfid tags..
const char beef[] = "43000970A49E";
const char mango[] = "2100834E01ED";
const char olive[] = "4100468935BB";
const char chicken[] = "4100427798EC";
const char coffee[] = "41004285F274";
const char banana[] = "410043429BDB";
const char beer[] = "210082C63B5E";
const char tomato[] = "210082CC1778";

SoftwareSerial rfidReader(2,3);
String tagString;
char tagNumber[14];
boolean receivedTag;

int currentDelay_w = 0;
int currentDelay_m = 0;


// led outputs
const int led_map4    = 4;
const int led_map5    = 5;
const int led_map6    = 6;
const int led_map7    = 7;
const int led_map8    = 8;
const int led_map9    = 9;
const int led_map10   = 10;

Servo servoMotor;
// outputs for motor and pump..
const int pumpOutput   = 11; 
const int magnetOutput = 12; // electromagnet removing water from tank
const int motorOutput  = 13;

unsigned long currentMillis;
unsigned long lastMillis;
unsigned long duration;
unsigned long second = 1000; // one second in millis
unsigned long pumpstart ;
unsigned long resetTime = 10000 ;
int counter = 0;

void setup() {
  currentMillis = millis(); 
  pinMode(pumpOutput, OUTPUT);
  pinMode(magnetOutput, OUTPUT);
  pinMode(motorOutput, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  // starts reading of rfid..
  Serial.begin(9600);
  rfidReader.begin(9600); // the RDM6300 runs at 9600bps
  Serial.println("\n\n\nRFID Reader...ready!");
}

void loop() {
  receivedTag=false;
  
  while (rfidReader.available()){
    int BytesRead = rfidReader.readBytesUntil(3, tagNumber, 15);//EOT (3) is the last character in tag 
    receivedTag=true;
  }
 // currentMillis = millis();
  
  if (receivedTag){ 
       
    tagString = tagNumber;
    Serial.println();
    Serial.print("Tag Number: ");
    Serial.println(tagString);
    
    int delayTime_w = 0; // delay time for filling the water
    int delayTime_m = 0; // delay time for the motor to control the tactile indicator
    
    delayPair delayTime = determineDelay(tagString);
    delayTime_w = delayTime.water;
    delayTime_m = delayTime.motor;
    
    Serial.println("delayTime_w: ");
    Serial.println(delayTime_w);
    Serial.println("delayTime_m: ");
    Serial.println(delayTime_m);
    
    Serial.println("currentDelay_w: ");
    Serial.println(currentDelay_w);
    Serial.println("currentDelay_m: ");
    Serial.println(currentDelay_m);

    // water level unchanged..
    if(delayTime_w == currentDelay_w) {
      return;
    }

    pump((delayTime_w - currentDelay_w), (delayTime_m - currentDelay_m));
    currentDelay_w = abs(delayTime_w);
    currentDelay_m = abs(delayTime_m);
    lastMillis = millis();
    

  } else if(millis() - lastMillis  >= resetTime) {

    if(currentDelay_w = 0) {
      return;
    }
  
    pump(-currentDelay_w, -currentDelay_m);
    currentDelay_w = 0;
    currentDelay_m = 0;
    
  }
}

// returns motor delay time depending on tagnumber
delayPair determineDelay(String tagNum) {
    
    rfidReader.flush();

    if(tagNum.substring(1,13)==beef) {
      return {17200, 3570};
    }

    if(tagNum.substring(1,13)==mango) {
      return {8240, 1073};
    }

    if(tagNum.substring(1,13)==olive) {
      return {5200, 1070};
    }

     if(tagNum.substring(1,13)==chicken) {
      return {4800, 1010};
    }

     if(tagNum.substring(1,13)==coffee) {
      return {3200, 650};
    }
     
     if(tagNum.substring(1,13)==banana) {
      return {2320, 480};
    }

     if(tagNum.substring(1,13)==beer) {
      return {1120, 240};
    }
     
     if(tagNum.substring(1,13)==tomato) {
      return {560, 110};
    }

    
    
    // tag not recognized
    return {currentDelay_w, currentDelay_m};
}

// pumps water changes the water indicator ..
void pump(int duration_w, int duration_m) {
  int servoAngle = -180;
  
  if(duration_m < 0) {
    servoAngle *= -1; // reverse direction of servo
  }

  servoMotor.attach(motorOutput);
  // sets angle of servomotor..
  servoMotor.write(servoAngle);
  
  // keep both water pump and motor on for their shared time,
  // asuming the motor always should be on for a shorter amount of
  // time than the waterpump

  if(duration_w < 0) {
    digitalWrite(magnetOutput, HIGH); // remove water..
  } else {
    digitalWrite(pumpOutput, HIGH);   // add water..
  }
  
  delay(abs(duration_m));

  // turn the motor for the indicator off, and keep the pump on..
  servoMotor.detach();
  
  delay(abs(duration_w - duration_m));

  // turn off the waterpump
  digitalWrite(magnetOutput, LOW);
  digitalWrite(pumpOutput, LOW);

  Serial.println("all done");
}

