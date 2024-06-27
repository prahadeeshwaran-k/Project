#include "WiFiSetup.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  vTaskDelay(500);
  WiFiSetup();
}

void loop() {
  // put your main code here, to run repeatedly:
  vTaskDelay(50000);
}
