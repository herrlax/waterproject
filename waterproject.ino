#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#include <Servo.h>
#include <SoftwareSerial.h>
#include <avr/pgmspace.h>

#define COLORLED A0
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, COLORLED, NEO_GRB + NEO_KHZ800);

struct delayPair {
    int water;
    int motor;
};

// rfid tags..
const char beef[]    = "43000970A49E";
const char mango[]   = "2100834E01ED";
const char olive[]   = "4100468935BB";
const char chicken[] = "4100427798EC";
const char coffee[]  = "41004285F274";
const char banana[]  = "410043429BDB";
const char beer[]    = "210082C63B5E";
const char tomato[]  = "210082CC1778";

SoftwareSerial rfidReader(2,3);
String tagString;
char tagNumber[14];
boolean receivedTag;

int currentDelay_w = 0;
int currentDelay_m = 0;


// led outputs
const int led_usa      = 4;  // beef
const int led_ecuador  = 5;  // banana
const int led_thailand = 6;  // mango
const int led_china    = 7;  // chicken, tomatoe
const int led_germany  = 8;  // beer
const int led_brazil   = 9;  // coffee
const int led_spain    = 10; // olive

const int leds[] = { led_usa,
                     led_ecuador,
                     led_ecuador,
                     led_thailand,
                     led_china,
                     led_germany,
                     led_brazil,
                     led_spain };

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
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  pinMode(pumpOutput, OUTPUT);
  pinMode(magnetOutput, OUTPUT);
  pinMode(motorOutput, OUTPUT);
  pinMode(led_usa, OUTPUT);
  pinMode(led_ecuador, OUTPUT);
  pinMode(led_thailand, OUTPUT);
  pinMode(led_china, OUTPUT);
  pinMode(led_germany, OUTPUT);
  pinMode(led_brazil, OUTPUT);
  pinMode(led_spain, OUTPUT);
    
  digitalWrite(pumpOutput, LOW); 
  digitalWrite(led_usa, HIGH);

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

    uint32_t c;

    if(delayTime_w > 15000) { // red
      c = strip.Color(255, 0, 0);
      
    } else if (delayTime_w > 8000) {  // orange
      c = strip.Color(255, 100, delayTime_w/500);
      
    } else if (delayTime_w > 5000) {  // yellow
      c = strip.Color(255, 255, delayTime_w/500);
      
    } else { // blue
      c = strip.Color(delayTime_w/500, delayTime_w/100, 255/(delayTime_w/500));
    }
    

    for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
    }

    pump((delayTime_w - currentDelay_w), (delayTime_m - currentDelay_m));
    currentDelay_w = abs(delayTime_w);
    currentDelay_m = abs(delayTime_m);
    lastMillis = millis();
    

  } 
}

// returns motor delay time depending on tagnumber
delayPair determineDelay(String tagNum) {
    
    rfidReader.flush();

    turnOffLeds(); // turns off all leds
    const int hoes = 1500; // time it takes for the water to flow through the hoes

    if(tagNum.substring(1,13)==beef) {
      digitalWrite(led_usa, HIGH);
      return {17200+hoes, (3570*1.2)};
    }

    if(tagNum.substring(1,13)==mango) {
      digitalWrite(led_thailand, HIGH);
      return {8240+hoes, (1730*1.2)};
    }

    if(tagNum.substring(1,13)==olive) {
      digitalWrite(led_spain, HIGH);
      return {5200+hoes, (1070*1.2)};
    }

     if(tagNum.substring(1,13)==chicken) {
      digitalWrite(led_china, HIGH);
      return {4800+hoes, (1010*1.2)};
    }

     if(tagNum.substring(1,13)==coffee) {
      digitalWrite(led_brazil, HIGH);
      return {3200+hoes, (650*1.2)};
    }
     
     if(tagNum.substring(1,13)==banana) {
      digitalWrite(led_ecuador, HIGH);
      return {2320+hoes, (480*1.2)};
    }

     if(tagNum.substring(1,13)==beer) {
      digitalWrite(led_germany, HIGH);
      return {1120+hoes, (240*1.2)};
    }
     
     if(tagNum.substring(1,13)==tomato) {
      digitalWrite(led_china, HIGH);
      return {560+hoes, (110*1.2)};
    }

    
    
    // tag not recognized
    return {currentDelay_w, currentDelay_m};
}

// turns off all leds
void turnOffLeds() {
  for(int i = 0; i < sizeof(leds); i++) {
    digitalWrite(leds[i], LOW);
  }
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
    Serial.println("REMOVE WATER!!!");
    digitalWrite(magnetOutput, HIGH); 
    digitalWrite(pumpOutput, LOW); 
  } else {
    digitalWrite(magnetOutput, LOW);
    digitalWrite(pumpOutput, HIGH); 
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

