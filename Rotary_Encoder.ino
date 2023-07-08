//Arduino/ArduinoRotaryEncoder/ArduinoRotaryEncoder.ino
#define ENC_A 2
#define ENC_B 3
volatile int counter = 0;

void setup() {

  // Set encoder pins and attach interrupts
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENC_A), read_encoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_B), read_encoder, CHANGE);

  // Start the serial monitor to show output
  Serial.begin(115200);
}

void loop() {
  static int lastCounter = 0;

  // If count has changed print the new value to serial
  if(counter != lastCounter){
    Serial.println(counter);
    lastCounter = counter;
  }
}

void read_encoder() {
  static uint8_t old_AB = 3;  // Lookup table index
  static int8_t encval = 0;   // Encoder value  
  static const int8_t enc_states[]  = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0}; // Lookup table
  old_AB <<=2;  
  if (digitalRead(ENC_A)) old_AB |= 0x02; // Add current state of pin A
  if (digitalRead(ENC_B)) old_AB |= 0x01; // Add current state of pin B
  
  encval += enc_states[( old_AB & 0x0f )];
  if( encval > 3 ) {        // Four steps forward
    int changevalue = 1;
    if(counter<30){
    counter = counter + changevalue;              // Update counter
    encval = 0;}
  }

  else if( encval < -3 ) {        // Four steps backward
    int changevalue = -1;
    if(counter>0){
    counter = counter + changevalue;              // Update counter
    encval = 0;}
  }
} 
