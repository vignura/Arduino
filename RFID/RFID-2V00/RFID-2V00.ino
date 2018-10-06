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

#define RELAY_ON_TIME_SEC           (15 * 60UL)
#define MAX_UIDS_ALLOWED            0x02
#define LONG_PRESS_DELAY_COUNT      0xC0  // 7.2 seconds delay

#define RELAY_STATE_ON              0x01
#define RELAY_STATE_OFF             0x02
#define RELAY_STATE_PAUSED          0x03

#define RFID_NOT_DETECTED           0x01
#define RFID_DETECTED               0x02

// make sure both addresses are seperated by atleast 4 Bytes
int tdyaddr = 0;
int totaddr = 4;
unsigned long ulTodayCount = 0;
unsigned long ulTotalCount = 0;

unsigned long ulStartTime = 0;
unsigned long ulStopTime  = 0;
unsigned long ulRunTime   = 0;

int iPressCount = 0;
const int UIDStartAddr = 0x0C;
int iRelayState = RELAY_STATE_OFF;
int iRFIDState = RFID_NOT_DETECTED;
MFRC522::Uid g_arrUID[MAX_UIDS_ALLOWED] = {0};

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
  UpdateLCD(iRelayState);
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
*                3) If a valid RFID is detected by the MFRC522 RFID reader when the Relay is 
*                   off, then the relay is turned on for a fixed time (ie 15 minutes) and 
*                   the lcd updated is updated with today and total counts. After fixed time,
*                   the relay is turned off. 
*                4) If a valid RFID is detected by the MFRC522 RFID reader and the Relay is
*                   is already on, then it is turned off and loop goes to paused state and 
*                   lcd is updated with "Paused.." text.
*                5) If a valid RFID is detected by the MFRC522 RFID reader and the Relay is
*                   is paused, then it is turned on and loop goes back to on state and 
*                   lcd is updated with counts. 
*                6) If a invalid RFID is detected, then "Access Denied" is displayed in lcd
* \param[in]  :: None
* \param[out] :: None
* \return     :: None
*/
/***********************************************************************************************/
void loop() 
{ 
  // get the start time 
  ulStartTime = millis();

  // set RIFD state
  iRFIDState = RFID_NOT_DETECTED;

  // reset today count
  if(digitalRead(EARSE) == 0)
  {
    if(iPressCount == 0)
    {
      ulTodayCount = 0;
      EEPROM.put(tdyaddr, ulTodayCount);
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
  }

  // Look for new cards
  if (mfrc522.PICC_IsNewCardPresent()) 
  {
    // Select one of the cards
    if (mfrc522.PICC_ReadCardSerial()) 
    {
      iRFIDState = RFID_DETECTED;
    }
    else
    {
      iRFIDState = RFID_NOT_DETECTED;
    }
  } 
  
  //print_UID(&mfrc522);
  
  if(iRFIDState == RFID_DETECTED)
  {
    // control the motor state if a valid UID is detected
    if (IsUIDValid(&mfrc522.uid))
    { 
      if(iRelayState == RELAY_STATE_OFF)
      {
        digitalWrite(RELAY, HIGH);  
        iRelayState = RELAY_STATE_ON;

        // increment the counts
        ulTodayCount++;
        ulTotalCount++;
        
        // store the change to EEPROM
        EEPROM.put(totaddr, ulTotalCount);
        EEPROM.put(tdyaddr, ulTodayCount);
      }
      else if(iRelayState == RELAY_STATE_ON)
      { 
        digitalWrite(RELAY,LOW);
        iRelayState = RELAY_STATE_PAUSED;
      }
      else if(iRelayState == RELAY_STATE_PAUSED)
      {
        digitalWrite(RELAY,HIGH);
        iRelayState = RELAY_STATE_ON; 
      }
      else
      {
        // invalid relay state;
        iRelayState = -1;
      }
    }
    else
    {
      // Serial.println(" Access denied");
      lcd.clear();
      lcd.setCursor(1,0);
      lcd.print("Access Denied");
      delay(2000);
    }
  }

  UpdateLCD(iRelayState);

  // added for  
  if(iRFIDState == RFID_DETECTED)
  {
    delay(2000);
  }

  if(iRelayState == RELAY_STATE_ON)
  {
    ulStopTime = millis();
    // possible overflow
    ulRunTime += (ulStopTime -ulStartTime);

    if((ulRunTime / 1000UL) >= RELAY_ON_TIME_SEC)
    {
     digitalWrite(RELAY, LOW);
     iRelayState = RELAY_STATE_OFF;
     ulRunTime = 0; 
    }
  }
} 

/***********************************************************************************************/
/*! 
* \fn         :: UpdateLCD()
* \author     :: Vignesh S
* \date       :: 01-OCT-2018
* \brief      :: This function updates the LCD display according to iRelayState parameter
* \param[in]  :: iRelayState
* \param[out] :: None
* \return     :: None
*/
/***********************************************************************************************/
void UpdateLCD(int iRelayState)
{
    switch(iRelayState)
    { 
      case RELAY_STATE_OFF:
      case RELAY_STATE_ON:    
        
        // update Today count
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("TDY CNT:");
        lcd.setCursor(9,0);
        lcd.print(ulTodayCount);

        // update Total count
        lcd.setCursor(0,1);
        lcd.print("TOT CNT:");
        lcd.setCursor(9,1);
        lcd.print(ulTotalCount);

        break;

      case RELAY_STATE_PAUSED:

        lcd.clear();
        lcd.setCursor(0,0);  
        lcd.print("    PAUSED..    ");

        break;

      default:    
        lcd.clear();
        lcd.setCursor(0,0);  
        lcd.print("    INVALID     ");
        lcd.setCursor(0,1);  
        lcd.print("  RELAY STATE   ");      
    }
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
