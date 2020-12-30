#include <SoftwareSerial.h>

#define SERIAL_BUAD_RATE            9600
#define SOFT_SERIAL_BUAD_RATE       1200
#define SERIAL_BUS_PIN							2
#define MODE_TIMER_SEC							20
#define MODE_SWITCH_DELAY_SEC				3

#define RX_MODE											0
#define TX_MODE											1

SoftwareSerial SSerial(SERIAL_BUS_PIN, SERIAL_BUS_PIN); // rx, tx
int mode = 0;
unsigned long time = 0;
char message[128];

void setup() {
  
  Serial.begin(SERIAL_BUAD_RATE);
  SSerial.begin(SOFT_SERIAL_BUAD_RATE);
  SSerial.setRX(SERIAL_BUS_PIN);
  sprintf(message, "Switching to %s mode for %d sec\nconnect pin %d to %d\nenter some text", mode ? "TX" : "RX", MODE_TIMER_SEC, 0, SERIAL_BUS_PIN);
  Serial.println(message);

}

void loop() {
  
  //Terminal();
  BusTest();

}

void Terminal()
{
  char data = 0;
  
  // read data from serial port and send it to software serial port
  if(Serial.available())
  {
    data = Serial.read();
    //sprintf(message, "WR:%c\n", data);
    //Serial.print(message);
    SSerial.write(data);
    //digitalWrite(LED_BUILTIN, HIGH);
  }
  
  // read data from software serial port and send it to serial port
  if(SSerial.available())
  {
    data = SSerial.read();
    //sprintf(message, "WR:%c\n", data);
    //Serial.print(message);
    Serial.write(data);
    //digitalWrite(LED_BUILTIN, HIGH);
  }
}

void BusTest()
{
	if((((millis() / 1000) % MODE_TIMER_SEC) == 0) && ((millis() / 1000) - time) > 1)
	{
		time = (millis() / 1000);

		if(mode == RX_MODE)
		{
			mode = TX_MODE;
			sprintf(message, "Switching to %s mode for %d sec\nconnect pin %d to %d", mode ? "TX" : "RX", MODE_TIMER_SEC, 1, SERIAL_BUS_PIN);
  		Serial.println(message);
			SSerial.setTX(SERIAL_BUS_PIN);
			Serial.end();
			pinMode(1, INPUT);
			delay(MODE_SWITCH_DELAY_SEC * 1000);
		}
		else
		{
			mode = RX_MODE;
			Serial.begin(SERIAL_BUAD_RATE);
			sprintf(message, "Switching to %s mode for %d sec\nconnect pin %d to %d\nenter some text", mode ? "TX" : "RX", MODE_TIMER_SEC, 0, SERIAL_BUS_PIN);
  		Serial.println(message);
			SSerial.setRX(SERIAL_BUS_PIN);
			delay(MODE_SWITCH_DELAY_SEC * 1000);
		}
	}

	if(mode == RX_MODE)
	{
		if(SSerial.available())
	  {
	    Serial.write(SSerial.read());
	  }
	}
	else{

	    //SSerial.println("TX Test");
      SSerial.write(0xAA);
	    delay(1000);
	}
}
