/***********************************************************************************************/
/*
 *  \file       : RFIDProg.ino
 *  \date       : 30-SEP-2018 
 *  \author     : Vignesh S 
 *  \email      : vignura@gmail.com
 *  \copyright  : All Rights are Reserved 
/*
/***********************************************************************************************/
#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>
#include <LiquidCrystal.h>

#define SS_PIN            10
#define RST_PIN           9
#define MAX_UIDS_ALLOWED  0x02

// initialize the library with the numbers of the interface pins
const int rs = 3, en = 4, d4 = 5, d5 = 6, d6 = 7, d7 = 8;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
MFRC522::Uid uid = {0};
const int UIDStartAddr = 0x0C;

/***********************************************************************************************/
/*! 
* \fn         :: setup()
* \author     :: Vignesh S
* \date       :: 01-OCT-2018
* \brief      :: This function performs the following initializations
*                1) Initializes Serial port
                 2) Initializes SPI
                 3) Initializes MFRC522 RFID reader
                 4) Initializes 16X2 LCD display
* \param[in]  :: None
* \param[out] :: None
* \return     :: None
*/
/***********************************************************************************************/
void setup() {

  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  Serial.println("Put your card to the reader...");
  Serial.println();
  lcd.begin(16, 2);

  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("LOOKING...");
}

/***********************************************************************************************/
/*! 
* \fn         :: loop()
* \author     :: Vignesh S
* \date       :: 01-OCT-2018
* \brief      :: This functions is called repeatedly from a infinite for loop in main().
*                It does the following functions
*                1) If a RFID is detected by the MFRC522 RFID reader, it is stored to EEPROM
*                   it can store MAX_UIDS_ALLOWED number of UIDs of RFIDs into EEPROM
* \param[in]  :: None
* \param[out] :: None
* \return     :: None
*/
/***********************************************************************************************/
void loop() {

  int iStoreAddr = 0;
  byte byUIDCount = 0;
 
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
  print_UID(&mfrc522.uid);
  delay(2000);

  iStoreAddr = (UIDStartAddr + (byUIDCount * sizeof(MFRC522::Uid)));
  
  // store UID to EEPROM
  EEPROM.put(iStoreAddr, mfrc522.uid);
  
  // print the stored address
  Serial.print("\tstored to EEPROM at ");
  Serial.println(iStoreAddr, HEX);

  // read back and verify
  Serial.print("Read Back: ");
  lcd.setCursor(0, 1);
  lcd.print("Read Back: ");

  EEPROM.get(iStoreAddr, uid);
  
  if(UIDCmp(&uid, &mfrc522.uid))
  {
    Serial.print(" OK");
    lcd.setCursor(11, 1);
    lcd.print(" OK");
  }
  else
  {
    Serial.print(" FAIL");
    lcd.setCursor(11, 1);
    lcd.print(" FAIL");
  }
  
  byUIDCount++;
  if(byUIDCount >=  MAX_UIDS_ALLOWED)
  {
    byUIDCount = 0;
  }

  delay(2000UL);
  Serial.println("Put your card to the reader...");
  lcd.clear();
  lcd.setCursor(1,0);
  lcd.print("LOOKING...");
}

/***********************************************************************************************/
/*! 
* \fn         :: print_UID()
* \author     :: Vignesh S
* \date       :: 01-OCT-2018
* \brief      :: This function prints the uid into lcd display and serial port
* \param[in]  :: uid
* \param[out] :: None
* \return     :: None
*/
/***********************************************************************************************/
void print_UID(MFRC522::Uid *uid)
{
  String UID = "";
  Serial.print("UID tag :");
  for (byte i = 0; i < uid->size; i++) 
  {
     UID.concat(String(uid->uidByte[i] < 0x10 ? " 0" : " "));
     UID.concat(String(uid->uidByte[i], HEX));
  }

  Serial.print(UID);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("UID:");
  lcd.print(UID);
}

/***********************************************************************************************/
/*! 
* \fn         :: UIDCmp()
* \author     :: Vignesh S
* \date       :: 01-OCT-2018
* \brief      :: This function compares two uids and returns true if they are equal else returns
*                false
* \param[in]  :: uid1
* \param[in]  :: uid2
* \param[out] :: None
* \return     :: true;false
*/
/***********************************************************************************************/
bool UIDCmp(MFRC522::Uid *uid1, MFRC522::Uid *uid2)
{
  int i = 0;
  // validate size
  if(uid1->size == uid2->size)
  {
    for(i = 0; i < uid1->size; i++)
    {
      if(uid1->uidByte[i] != uid2->uidByte[i])
      {
        break;
      }
    }

    if(i == uid1->size)
    {
      return true;
    }
  }
  
  return false;
}
