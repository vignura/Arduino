//RFID
#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>

#define SS_PIN 10
#define RST_PIN 9
#define EARSE 1
#define RELAY 2 //Relay pin

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(3,4,5, 6, 7,8);

// make sure both addresses are seperated by atleast 4 Bytes
int tdyaddr = 0;
int totaddr = 4;

byte value;
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

void setup() 
{
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  pinMode(RELAY, OUTPUT);
  Serial.println("Put your card to the reader...");
  Serial.println();
  lcd.begin(16, 2);
}

void loop() 
{
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }

  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  
  if (content.substring(1) == "D0 48 CF 57"||content.substring(1) == "3B C5 A3 79") //change here the UID of the card/cards that you want to give access
  { 
    // EEPROM.write(tdyaddr,EEPROM.read(tdyaddr)+1);
    // EEPROM.write(totaddr,EEPROM.read(totaddr)+1);
    EEPROMWritelong(tdyaddr, (EEPROMReadlong(tdyaddr) + 1));
    EEPROMWritelong(totaddr, (EEPROMReadlong(totaddr) + 1));

    lcd.setCursor(0,0);
    lcd.print("TDY CNT ");
    lcd.setCursor(0,10);
    lcd.print(EEPROMReadlong(tdyaddr));
    lcd.setCursor(1,0);
    lcd.print("TOT CNT ");
    lcd.setCursor(1,10);
    lcd.print(EEPROMReadlong(totaddr));
    Serial.println("Authorized access");
    Serial.println();
    digitalWrite(RELAY, HIGH);
    
    for(int i=0;i<15;i++)
    {
      for(int j=0;j<60;j++)
      {
        delay(1000);
      }
    }

    digitalWrite(RELAY,LOW);

  }
  else
  {
    Serial.println(" Access denied");
  }

  if(digitalRead(EARSE) == 0)
  {
    EEPROMWritelong(tdyaddr,0);
  }

}

void EEPROMWritelong(int address, long value)
{
  //Decomposition from a long to 4 bytes by using bitshift.
  //One = Most significant -> Four = Least significant byte
  byte four = (value & 0xFF);
  byte three = ((value >> 8) & 0xFF);
  byte two = ((value >> 16) & 0xFF);
  byte one = ((value >> 24) & 0xFF);

  //Write the 4 bytes into the eeprom memory.
  EEPROM.write(address, four);
  EEPROM.write(address + 1, three);
  EEPROM.write(address + 2, two);
  EEPROM.write(address + 3, one);
}

long EEPROMReadlong(long address)
{
  //Read the 4 bytes from the eeprom memory.
  long four = EEPROM.read(address);
  long three = EEPROM.read(address + 1);
  long two = EEPROM.read(address + 2);
  long one = EEPROM.read(address + 3);

  //Return the recomposed long by using bitshift.
  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}
