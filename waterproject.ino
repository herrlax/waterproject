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
}

void loop() {

  // read rfid
  // pump(value from tag);
}

// run if an rfid has been read 
void pump(int duration) {
  
  digitalWrite(pumpOutput, HIGH);  
  delay(duration);                
  digitalWrite(pumpOutput, LOW);   
}

