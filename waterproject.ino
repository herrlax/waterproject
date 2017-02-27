#include <Servo.h>
#include <SoftwareSerial.h>
#include <avr/pgmspace.h>

struct delayPair {
    int water;
    int motor;
};

// rfid tags..
const char tag_0_A[] = "3D0047B0F53F";
const char tag_0_B[] = "2100834E01ED";
const char tag_0_C[] = "3C00CF0448BF";
const char tag_1_A[] = "27001396EB49";
const char tag_1_B[] = "27009CD4A0CF";
const char tag_1_C[] = "0415D8761CA3";
const char tag_2_A[] = "210082C63B5E";
const char tag_2_B[] = "210082CC1778";
const char tag_2_C[] = "430003D2B527";

SoftwareSerial rfidReader(8,9);
String tagString;
char tagNumber[14];
boolean receivedTag;

int currentDelay_w = 0;
int currentDelay_m = 0;


// led outputs
const int led_water1  = 1;
const int led_water2  = 2;
const int led_map1    = 3;
const int led_map2    = 4;
const int led_map3    = 5;

Servo servoMotor;
// outputs for motor and pump..
const int pumpOutput   = 6; 
const int magnetOutput = 10; // electromagnet removing water from tank
const int motorOutput  = 7;

unsigned long currentMillis;
unsigned long previousMillis;
unsigned long duration;
unsigned long second = 1000; // one second in millis
unsigned long pumpstart ;

int counter = 0;

void setup() {
  pinMode(pumpOutput, OUTPUT);
  pinMode(magnetOutput, OUTPUT);
  pinMode(motorOutput, OUTPUT);
  pinMode(led_water1, OUTPUT);
  pinMode(led_water2, OUTPUT);
  pinMode(led_map1, OUTPUT);
  pinMode(led_map2, OUTPUT);
  pinMode(led_map3, OUTPUT);
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
  }
}

// returns motor delay time depending on tagnumber
delayPair determineDelay(String tagNum) {
    
    rfidReader.flush();

    if(tagNum.substring(1,13)==tag_0_A) {
      return {7000, 3000};
    }

    if(tagNum.substring(1,13)==tag_0_B) {
      return {5000, 1000};
    }

    if(tagNum.substring(1,13)==tag_0_C) {
      return {1000, 500};
    }

    // tag not recognized
    return {currentDelay_w, currentDelay_m};
}

// pumps water changes the water indicator ..
void pump(int duration_w, int duration_m) {
  int servoAngle = -180;
  
  if(duration_w < 0) {
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
}

