//https://www.circuitbasics.com/how-to-setup-and-program-rotary-encoders-on-the-arduino/
int clkPin = 3;
int dtPin = 4;
int switchPin = 12;

int count = 0;
int clkPinLast = LOW;
int clkPinCurrent = LOW;


void setup() {
  pinMode(clkPin, INPUT);
  pinMode(dtPin, INPUT);
  pinMode(switchPin, INPUT_PULLUP);
  Serial.begin(9600);
}

void loop() {
  int switchState = digitalRead(switchPin);

  if (switchState == LOW) {
    count = 0;
  }

  clkPinCurrent = digitalRead(clkPin);

  if ((clkPinLast == LOW) && (clkPinCurrent == HIGH)) {
    if (digitalRead(dtPin) == HIGH) {
      count--;
    }
    else {
      count++;
    }

    Serial.println(count);
  }

  clkPinLast = clkPinCurrent;
}
