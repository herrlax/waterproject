#include <Servo.h>
#include <SoftwareSerial.h>
#include <avr/pgmspace.h>


// rfid tags..
const char tag_0[] = "27009CD4A0CF";
const char tag_1[] = "2100834E01ED";

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
const int pumpOutput  = 6; 
const int motorOutput = 7;

unsigned long currentMillis;
unsigned long previousMillis;
unsigned long duration;
unsigned long second = 1000; // one second in millis
unsigned long pumpstart ;

int counter = 0;

void setup() {
  pinMode(pumpOutput, OUTPUT);
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
    
    delayTime_w = determineDelayW(tagString);
    delayTime_m = determineDelayM(tagString);
    
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

// returns water delay time depending on tagnumber
int determineDelayW(String tagNum) {
    
    Serial.println("-------------");
    Serial.println(tagNum.substring(1,13));
    Serial.println("-------------");
    rfidReader.flush();

    if(tagNum.substring(1,13)==tag_0) {
      return 5000;
    }

    if(tagNum.substring(1,13)==tag_1) {
      return 7000;
    }

    // tag not recognized
    return currentDelay_w;
}


// returns motor delay time depending on tagnumber
int determineDelayM(String tagNum) {
    
    Serial.println("-------------");
    Serial.println(tagNum.substring(1,13));
    Serial.println("-------------");
    rfidReader.flush();

    if(tagNum.substring(1,13)==tag_0) {
      return 3000;
    }

    if(tagNum.substring(1,13)==tag_1) {
      return 5000;
    }

    // tag not recognized
    return currentDelay_m;
}

// pumps water changes the water indicator ..
void pump(int duration_w, int duration_m) {
  
  Serial.println();
  Serial.println("duration_w: ");
  Serial.println(duration_w);
  
  Serial.println("duration_m: ");
  Serial.println(duration_m);

  int servoAngle = -180;
  
  if(duration_w < 0) {
    servoAngle *= -1; // reverse direction of servo
    Serial.print("remove water: ");
    Serial.println(abs(duration_w));
    Serial.print("lower the indicator: ");
    Serial.println(abs(duration_m));
  } else if (duration_w > 0) {
    Serial.print("add water: ");
    Serial.println(abs(duration_w));
    Serial.print("raise the indicator: ");
    Serial.println(abs(duration_m));
  }

  servoMotor.attach(motorOutput);
  // sets angle of servomotor..
  servoMotor.write(servoAngle);
  
  Serial.println();

  // keep both water pump and motor on for their shared time,
  // asuming the motor always should be on for a shorter amount of
  // time than the waterpump
  //digitalWrite(pumpOutput, HIGH);
  //digitalWrite(motorOutput, HIGH);
  Serial.println("POWER PUMP..");
  Serial.println("POWER MOTOR..");
  delay(abs(duration_m));

  // turn the motor for the indicator off, and keep the pump on..
  servoMotor.detach();
  //digitalWrite(motorOutput, LOW);
  Serial.println("POWER PUMP..");
  delay(duration_w - duration_m);

  // turn off the waterpump too..
  //digitalWrite(pumpOutput, LOW); 
  
  Serial.println("BOTH OFF PUMP AND MOTOR OFF..");
  
  Serial.println();

  rfidReader.flush();
}

