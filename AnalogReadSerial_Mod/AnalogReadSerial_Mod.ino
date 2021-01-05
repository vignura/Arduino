/*
  AnalogReadSerial

  Reads an analog input on pin 0, prints the result to the Serial Monitor.
  Graphical representation is available using Serial Plotter (Tools > Serial Plotter menu).
  Attach the center pin of a potentiometer to pin A0, and the outside pins to +5V and ground.

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/AnalogReadSerial
*/

#define SAMPLING_PIN            A0
#define TRIGGER_LEVEL           900
#define SAMPLE_SIZE             256
// Ref: https://www.arduino.cc/reference/en/language/functions/analog-io/analogread/
#define ANALOG_READ_TIME_US     100
#define SAMPLING_DELAY_US       (2 * ANALOG_READ_TIME_US)

int val[SAMPLE_SIZE] = {0};

// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(115200);
}

// the loop routine runs over and over again forever:
void loop() {
  
  int ch = 0;
  
  if(Serial.available())
  {
    ch = Serial.read();
  }

  if(ch == '\n')
  {
    capture();    
  }
}

void capture()
{
  int i = 0;

  if(analogRead(SAMPLING_PIN) < TRIGGER_LEVEL)
  {
    for(i = 0; i < SAMPLE_SIZE; i++)
    {
      val[i] = analogRead(SAMPLING_PIN);    
      delayMicroseconds(SAMPLING_DELAY_US - ANALOG_READ_TIME_US);
    }
    
    for(i = 0; i < SAMPLE_SIZE; i++)
    {
      Serial.println(val[i]);
    }
  }
}