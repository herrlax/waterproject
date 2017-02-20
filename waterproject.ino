#include <SoftwareSerial.h>
#include <avr/pgmspace.h>

// rfid tags..
const char* tag_0 = "27009CD4A0CF"; 
const char* tag_1 = "2100834E01ED";

SoftwareSerial rfidReader(8,9); 
String tagString;
char tagNumber[14];
boolean receivedTag;

int currentDelay = 0;


// led outputs
const int led_water1  = 1;
const int led_water2  = 2;
const int led_map1    = 3;
const int led_map2    = 4;
const int led_map3    = 5;

// motor pump outputs
const int pumpOutput  = 6; 
const int motor       = 7;

unsigned long currentMillis;
unsigned long previousMillis;
unsigned long duration;
unsigned long second = 1000; // one second in millis
unsigned long pumpstart ;

int counter = 0;

void setup() {
  pinMode(pumpOutput, OUTPUT);
  pinMode(motor, OUTPUT);
  pinMode(led_water1, OUTPUT);
  pinMode(led_water2, OUTPUT);
  pinMode(led_map1, OUTPUT);
  pinMode(led_map2, OUTPUT);
  pinMode(led_map3, OUTPUT);
  duration  = millis();
  pumpstart = millis() ;
  digitalWrite(LED_BUILTIN, LOW);

  // starts reading of rfid..
  Serial.begin(9600);
  rfidReader.begin(9600); // the RDM6300 runs at 9600bps
  Serial.println("\n\n\nRFID Reader...ready!");
}

void loop() {

  receivedTag = false;
  
  while (rfidReader.available()) {
    int BytesRead = rfidReader.readBytesUntil(3, tagNumber, 15); //EOT (3) is the last character in tag 
    receivedTag = true;
  }  
 
  if (receivedTag){
    tagString = tagNumber;
    Serial.println();
    
    int delayTime = 0;
    delayTime = determineDelay(tagNumber);
    Serial.println("delayTime: ");
    Serial.println(delayTime);
    
    Serial.println("currentDelay: ");
    Serial.println(currentDelay);
    
    if(delayTime == currentDelay) {
      return;
    }
    
    
    pump(delayTime - currentDelay);

    currentDelay = abs(delayTime);
  }
}

int determineDelay(const char *tagNum) {
      
    if(strcmp(tagNum+1, tag_0) == 0) {
      return 1000;
      Serial.println("FOUND");
    }
    
    if(strcmp(tagNum+1, tag_1) == 0) {
      return 2000;
      Serial.println("FOUND");
    }
      
    return 5000;
}

// run if an rfid has been read 
void pump(int duration) {
  
  Serial.println();
  Serial.print("Received in pump:");

  // remove water..
  if(duration < 0) {
    Serial.println();
    Serial.print("remove water: ");
  } else if (duration > 0) {
    Serial.println();
    Serial.print("add water: ");
  }
  
  Serial.println(abs(duration));
  rfidReader.flush();
}

