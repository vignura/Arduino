#ifndef _SERIAL_COMM_H_
#define _SERIAL_COMM_H_

#include <Arduino.h>
#include <stdint.h>
#include <SoftwareSerial.h>
//#include <Serial.h>

#define SERIAL_READ_DELAY_MS    2

#define MAX_DATA_SIZE           16
#define SERIAL_HEADER_SIZE      6
#define SERIAL_CMD_HEADER       0xAA
#define SERIAL_RES_HEADER       0xEE


typedef struct serial_packet{

  uint8_t header;
  uint8_t address[2];
  uint8_t cmdID;
  uint8_t data_size;
  uint8_t checksum;
  uint8_t data[MAX_DATA_SIZE];

}serial_packet;


/***********************************************************************************************/
bool is_valid_packet(serial_packet *packet, uint8_t address[2]);
uint8_t compute_checksum(serial_packet *packet);
/***********************************************************************************************/

/***********************************************************************************************/
/***********************************************************************************************/
/*! 
* \fn         :: RecvCmd()
* \author     :: Vignesh S
* \date       :: 07-DEC-2018
* \brief      :: This function cheks HC-05 serial port for received data and if data is available
*                then reads it, the number of bytes read is returned.
* \param[in]  :: SPort, pBuff, iBuflen
* \return     :: iIndex
*/
/***********************************************************************************************/
template <typename T>
int RecvCmd(T &SPort, char *pBuff, int iBuflen)
{
  int iIndex = 0;

  if(pBuff == NULL)
  {
    return -1;
  }
  
  while(iIndex < iBuflen)
  {
    delay(SERIAL_READ_DELAY_MS);
    
    if(SPort.available())
    {
      pBuff[iIndex] = SPort.read();
      iIndex++;
    }
    else
    {
      break;
    }
  }

  return iIndex;
}

/***********************************************************************************************/
/*! 
* \fn         :: recv_packet()
* \author     :: Vignesh S
* \date       :: 27-DEC-2020
* \brief      :: This function reads data from serial port into packet and returns the number of
*                bytes read.
* \param[in]  :: SPort, packet
* \return     :: iIndex
*/
/***********************************************************************************************/
template <typename T>
int recv_packet(T &SPort, serial_packet *packet)
{
  int iIndex = 0;
  char *buff = (char*)packet;
 
  while(iIndex < sizeof(serial_packet))
  {
    delay(SERIAL_READ_DELAY_MS);
    
    if(SPort.available())
    {
      buff[iIndex] = SPort.read();
      iIndex++;
    }
    else
    {
      break;
    }
  }

  return iIndex;
}

/***********************************************************************************************/
/*! 
* \fn         :: send_packet()
* \author     :: Vignesh S
* \date       :: 27-DEC-2020
* \brief      :: This function sends data to serial port from packet and returns the number of
*                bytes sent.
* \param[in]  :: SPort, packet
* \return     :: iIndex
*/
/***********************************************************************************************/
template <typename T>
int send_packet(T &SPort, serial_packet *packet)
{
  int iIndex = 0;
  char *buff = (char*)packet;

  for (iIndex = 0; iIndex < (SERIAL_HEADER_SIZE + packet->data_size); ++iIndex)
  {
    SPort.print(buff[iIndex]);
  }

  return iIndex;
}

/***********************************************************************************************/
/*! 
* \fn         :: transmit_command()
* \author     :: Vignesh S
* \date       :: 27-DEC-2020
* \brief      :: This function transmits a command through a serial and receives the response 
* \param[in]  :: SPort, cmd_packet, res_packet
* \return     :: iIndex
*/
/***********************************************************************************************/
template <typename T>
int transmit_command(T &SPort, serial_packet *cmd_packet, serial_packet *res_packet, int timeout_ms)
{
  int iRecvBytes = 0;
  int count = 0;

  send_packet(SPort, cmd_packet);

  while((iRecvBytes < SERIAL_HEADER_SIZE) && (count < (timeout_ms / SERIAL_READ_DELAY_MS)))
  {
    iRecvBytes = recv_packet(SPort, res_packet);
    count++;
  }

  return iRecvBytes;
}
/***********************************************************************************************/

#endif // _SERIAL_COMM_H_