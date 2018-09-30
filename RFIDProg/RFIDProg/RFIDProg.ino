#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>

#define SS_PIN            10
#define RST_PIN           9
#define MAX_UIDS_ALLOWED  0x02

const int UIDStartAddr = 0x8;
const int UIDSizeBytes = 0x4;

// initialize the library with the numbers of the interface pins
const int rs = 3, en = 4, d4 = 5, d5 = 6, d6 = 7, d7 = 8;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

byte value;
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

void setup() {

  Serial.begin(9600);
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522

  Serial.println("Put your card to the reader...");
  Serial.println();
  lcd.begin(16, 2);
}

void loop() {

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
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  content.toUpperCase();

  Serial.println(content);
  Serial.println(content.substring(1));
  Serial.println(content.substring(2));
  Serial.println(mfrc522.uid.size);
  
  
}
