/***********************************************************************************************/
/*
 *  \file       : RFID.ino
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

// pins
#define SS_PIN        10
#define RST_PIN       9
#define EARSE         2
#define RELAY         1 

#define MOTOR_ON_TIME_SEC           (15 * 60UL)
#define MAX_UIDS_ALLOWED            0x02
#define LONG_PRESS_DELAY_COUNT      0xF6  // 7.2 seconds delay

// make sure both addresses are seperated by atleast 4 Bytes
int tdyaddr = 0;
int totaddr = 4;
int iPressCount = 0;
unsigned long ulTodayCount = 0;
unsigned long ulTotalCount = 0;
const int UIDStartAddr = 0x0C;
MFRC522::Uid g_arrUID[MAX_UIDS_ALLOWED];

// initialize the library with the numbers of the interface pins
const int rs = 3, en = 4, d4 = 5, d5 = 6, d6 = 7, d7 = 8;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Create MFRC522 instance.
MFRC522 mfrc522(SS_PIN, RST_PIN);

/***********************************************************************************************/
/*! 
* \fn         :: setup()
* \author     :: Vignesh S
* \date       :: 01-OCT-2018
* \brief      :: This function performs the following initializations
*                1) Sets the Relay pins as output and sets its state to low
                 2) Initializes SPI
                 3) Initializes MFRC522 RFID reader
                 4) Initializes 16X2 LCD display
                 5) Initializes today count and total count from EEPROM
                 6) Loads the stored UIDs from EEPROM
                 7) LCD displays is updated with counts
* \param[in]  :: None
* \param[out] :: None
* \return     :: None
*/
/***********************************************************************************************/
void setup() 
{
  // set the relay pin as output, set its state to low
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, LOW);

  // Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  lcd.begin(16, 2);

  // Serial.println("Put your card to the reader...");
  // Serial.println();
  
  EEPROM.get(totaddr, ulTotalCount);
  EEPROM.get(tdyaddr, ulTodayCount);
  loadUID();
  UpdateLCD();
}

/***********************************************************************************************/
/*! 
* \fn         :: loop()
* \author     :: Vignesh S
* \date       :: 01-OCT-2018
* \brief      :: This functions is called repeatedly from a infinite for loop in main().
*                It does the following functions
*                1) When Erase button is pressed, today count is reset
*                2) When Erase button is pressed for long time (ie > 7.2 Sec) total count
*                   is cleared
*                3) If a valid RFID is detected by the MFRC522 RFID reader, then relay is turned
*                   on for a fixed time (ie 15 minutes) and the lcd updated is updated with 
*                   today and total counts. After fixed time, relay is  turned off. 
*                4) If a invalid RFID is detected, then "Access Denied" is displayed in lcd
* \param[in]  :: None
* \param[out] :: None
* \return     :: None
*/
/***********************************************************************************************/
void loop() 
{ 
  // reset today count
  if(digitalRead(EARSE) == 0)
  {
    if(iPressCount == 0)
    {
      ulTodayCount = 0;
      EEPROM.put(tdyaddr, ulTodayCount);
      UpdateLCD();
    }
    
    iPressCount++;
  }
  else
  {
    iPressCount = 0;
  }

  // reset the total count
  if(iPressCount >= LONG_PRESS_DELAY_COUNT)
  {
    ulTotalCount = 0;
    EEPROM.put(totaddr, ulTotalCount);
    iPressCount = 0;
    UpdateLCD();
  }

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
 
  //print_UID(&mfrc522);
  
  if (IsUIDValid(&mfrc522.uid)) //change here the UID of the card/cards that you want to give access
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
    delay(2000);
    UpdateLCD();
  }
} 

/***********************************************************************************************/
/*! 
* \fn         :: UpdateLCD()
* \author     :: Vignesh S
* \date       :: 01-OCT-2018
* \brief      :: This function updates the LCD display with today count and total count
* \param[in]  :: None
* \param[out] :: None
* \return     :: None
*/
/***********************************************************************************************/
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

/***********************************************************************************************/
/*! 
* \fn         :: loadUID()
* \author     :: Vignesh S
* \date       :: 01-OCT-2018
* \brief      :: This function loads the stored UIDs from EEPROM into global structure 
* \global     ::  
* \param[in]  :: None
* \param[out] :: None
* \return     :: None
*/
/***********************************************************************************************/
void loadUID()
{
  for(int i = 0; i < MAX_UIDS_ALLOWED; i++)
  {
    EEPROM.get(((i * sizeof(MFRC522::Uid)) + UIDStartAddr), g_arrUID[i]);
    //print_UID(&g_arrUID[i]);
    //lcd.setCursor(0,1);
    //lcd.print(i + 1);
    //Serial.println(" loaded"); 
    //delay(2000);
  }
}

/***********************************************************************************************/
/*! 
* \fn         :: delay_sec()
* \author     :: Vignesh S
* \date       :: 01-OCT-2018
* \brief      :: This function provides delay in seconds
* \param[in]  :: ulTimeSec
* \param[out] :: None
* \return     :: None
*/
/***********************************************************************************************/
void delay_sec(unsigned long ulTimeSec)
{ 
  unsigned long i = 0;
  
  for(i = 0; i < ulTimeSec; i++)
  {
    delay(1000);
  }
}

/***********************************************************************************************/
/*! 
* \fn         :: IsUIDValid()
* \author     :: Vignesh S
* \date       :: 01-OCT-2018
* \brief      :: This function validates the passed uid by comparing it with global array
*                (g_arrUID) and returns true if a match is found else false is returned
* \param[in]  :: uid
* \param[out] :: None
* \return     :: true; false
*/
/***********************************************************************************************/
bool IsUIDValid(MFRC522::Uid *uid)
{
  int i = 0;
  int j = 0;
  
  for(i = 0; i < MAX_UIDS_ALLOWED; i++)
  {
    // validate size
    if(UIDCmp(uid, &g_arrUID[i]))
    {
      return true;
    }
  }
  
  return false;
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
