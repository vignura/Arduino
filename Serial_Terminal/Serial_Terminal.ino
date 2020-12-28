#include <SoftwareSerial.h>

#define HC05_BUAD_RATE                      9600
#define DEBUG_BUAD_RATE                     9600

SoftwareSerial SS_Bluetooth(3, 2); // rx, tx
char message[32];

void setup() {
  
  Serial.begin(DEBUG_BUAD_RATE);
  SS_Bluetooth.begin(HC05_BUAD_RATE);

}

void loop() {
  
  BuletoothTerminal();

}

void BuletoothTerminal()
{
  char data = 0;
  // if data is available in debug, send it to bluetooth 
  if(Serial.available())
  {
    data = Serial.read();
    //sprintf(message, "WR:%c\n", data);
    //Serial.print(message);
    SS_Bluetooth.write(data);
    //digitalWrite(LED_BUILTIN, HIGH);
  }
  
  // if data is available in bluetooth, send it to debug
  if(SS_Bluetooth.available())
  {
    data = SS_Bluetooth.read();
    //Serial.write(data);
    sprintf(message, "RD:%c\n", data);
    Serial.print(message);
    // echo
    //SS_Bluetooth.write(SS_Bluetooth.read());
    //digitalWrite(LED_BUILTIN, LOW);
  }
}
