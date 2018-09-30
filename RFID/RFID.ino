//RFID
#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>

#define SS_PIN 10
#define RST_PIN 9
#define EARSE 2
#define RELAY 1 //Relay pin
#define MOTOR_ON_TIME_SEC   (10 * 60UL)

// initialize the library with the numbers of the interface pins
const int rs = 3, en = 4, d4 = 5, d5 = 6, d6 = 7, d7 = 8;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// make sure both addresses are seperated by atleast 4 Bytes
int tdyaddr = 0;
int totaddr = 4;
unsigned long ulTodayCount = 0;
unsigned long ulTotalCount = 0;

byte value;
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

void setup() 
{
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, LOW);

  //Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522

  // Serial.println("Put your card to the reader...");
  // Serial.println();
  lcd.begin(16, 2);

  EEPROM.get(tdyaddr, ulTodayCount);
  EEPROM.get(totaddr, ulTotalCount);
}

void loop() 
{
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("LOOKING...");
  
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    // return;
  }

  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    // return;
  }
  
  //Show UID on serial monitor
  // Serial.print("UID tag :");
  String content= "";
  byte letter;
  
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     // Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     // Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();

  if (content.substring(1) == "90 CC 92 A3"||content.substring(1) == "9C 50 B2 79") //change here the UID of the card/cards that you want to give access
  { 
    // increment the counts
    ulTodayCount++;
    ulTotalCount++;
    // store the change to EEPROM
    EEPROM.put(totaddr, ulTotalCount);
    EEPROM.put(tdyaddr, ulTodayCount);
    UpdateLCD();
    
    digitalWrite(RELAY, HIGH);
    delay_sec(MOTOR_ON_TIME_SEC);
    digitalWrite(RELAY,LOW);
  }
  else
  {
    // Serial.println(" Access denied");
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("Access Denied");
  }

  if(digitalRead(EARSE) == 0)
  {
    ulTodayCount = 0;
    EEPROM.put(tdyaddr, ulTodayCount);
  }
} 

void UpdateLCD()
{
    // update Today count
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("TDY CNT: ");
    lcd.setCursor(10,0);
    lcd.print(ulTodayCount);
    
    // update Total count
    lcd.setCursor(0,1);
    lcd.print("TOT CNT: ");
    lcd.setCursor(10,1);
    lcd.print(ulTotalCount);
}

void delay_sec(unsigned long ulTimeSec)
{
  unsigned long i = 0;
  
  for(i = 0; i < ulTimeSec; i++)
  {
    delay(1000);
  }
}
