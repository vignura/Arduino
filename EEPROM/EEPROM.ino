#include <EEPROM.h>

unsigned long ulVal1 = 1;
unsigned long ulVal2 = 1;

void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("Write Val: ");
  Serial.println(ulVal1, HEX);
  Serial.print("Read Val: ");
  Serial.println(EEPROM.get(0, ulVal2), HEX);
  Serial.println("");
  ulVal1 *= 3;
  EEPROM.put(0, ulVal1);
  
  delay(1000);
}
