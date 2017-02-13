// outputs
const int pump      = 6; 
const int motor     = 7;
const int led_water = 1;
const int led_map   = 2;

unsigned long currentMillis;
unsigned long previousMillis;
unsigned long duration;
unsigned long hour = 3600000UL; // one hour
unsigned long duration2 ;
unsigned long pumpstart ;

int counter = 0;

void setup() {
  pinMode (pump, OUTPUT) ;
  pinMode (motor, OUTPUT) ;
  duration = millis();
}

void loop() {
  // put your main code here, to run repeatedly:

}
