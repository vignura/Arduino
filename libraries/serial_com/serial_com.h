#ifndef _SERIAL_COMM_H_
#define _SERIAL_COMM_H_

#include <Arduino.h>
#include <stdint.h>
#include <SoftwareSerial.h>
//#include <Serial.h>

#define SERIAL_READ_DELAY_MS    2

#define MAX_DATA_SIZE           16
#define SERIAL_HEADER           0xAA
/* sizeof header + src_addr + dst_addr + data_size + checksum */
#define SERIAL_MIN_PKT_SIZE     7
#define SERIAL_MAX_PKT_SIZE     (SERIAL_MIN_PKT_SIZE + MAX_DATA_SIZE)

#define SERIAL_DEVICE_BOUND         1
#define SERIAL_DEVICE_NOT_BOUND     0
#define SERIAL_MAX_RECV_TIMEOUT_MS  (0xFFFF)

#define SERIAL_SUCCESS              0
#define SERIAL_FAILURE              -1

#define SERIAL_ERR_BASE                   (-128)
#define SERIAL_ERR_NULL_PTR               (SERIAL_ERR_BASE + 1)
#define SERIAL_ERR_INVALID_HEADER         (SERIAL_ERR_BASE + 2)
#define SERIAL_ERR_INVALID_CHKSUM         (SERIAL_ERR_BASE + 3)
#define SERIAL_ERR_DST_ADDR               (SERIAL_ERR_BASE + 4)
#define SERIAL_ERR_SRC_ADDR               (SERIAL_ERR_BASE + 5)
#define SERIAL_ERR_INVALID_DATA_SIZE      (SERIAL_ERR_BASE + 6)
#define SERIAL_ERR_INVALID_PKT_SIZE       (SERIAL_ERR_BASE + 7)
#define SERIAL_ERR_RECV_TIMEOUT           (SERIAL_ERR_BASE + 8)
#define SERIAL_ERR_DEV_NOT_BOUND          (SERIAL_ERR_BASE + 9)


typedef struct serial_packet{

  uint8_t header;
  uint8_t src_addr[2];
  uint8_t dst_addr[2];
  uint8_t data_size;
  uint8_t data[MAX_DATA_SIZE];
  uint8_t checksum;

}serial_packet;


/***********************************************************************************************/
int8_t set_dev_addr(uint8_t address[2]);
int8_t set_bind_addr(uint8_t address[2]);
int8_t validate_packet(serial_packet *packet);
int8_t validate_src_addr(serial_packet *packet, uint8_t src_addr[2]);
uint8_t compute_checksum(serial_packet *packet);
void frame_packet(serial_packet* packet, uint8_t *data_buf, uint8_t size);
/***********************************************************************************************/

/***********************************************************************************************/
extern static uint8_t bind_addr[2];
extern static uint8_t is_bound;
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
* \brief      :: This function receives a packet from serial bus within given timeout
* \param[in]  :: SPort, packet, timeout_ms
* \return     :: received packet size or error code
*/
/***********************************************************************************************/
template <typename T>
int8_t recv_packet(T &SPort, serial_packet *packet, uint16_t timeout_ms)
{
  int8_t iIndex = 0;
  uint16_t count = 0;
  char *buff = (char*)packet;

  // read data from serial port
  while(1)
  {
    while(iIndex < (buf_size + SERIAL_MIN_PKT_SIZE))
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

    if(iIndex == (buf_size + SERIAL_MIN_PKT_SIZE))
    {
      break;
    }

    if(count >= (timeout_ms / SERIAL_READ_DELAY_MS))
    {
      return SERIAL_ERR_RECV_TIMEOUT;
    }

    count++;
  }

  /* if the packet size is less than min packet size or greater than max packet size,
     then it is malformed packet which we can discard. */
  if((iIndex < SERIAL_MIN_PKT_SIZE) || (iIndex > SERIAL_MAX_PKT_SIZE))
  {
    return SERIAL_ERR_INVALID_PKT_SIZE;
  }

  /* In a properly formed packet, checksum will be the last byte that we receive, 
  our serial_packet's size can vary from minimum of SERIAL_MIN_PKT_SIZE (ie, no data)
  to SERIAL_MAX_PKT_SIZE (ie, full data). Hence if the received packet size (iIndex)
  is less than SERIAL_MAX_PKT_SIZE, then weneed to copy the last byte to checksum
  field of serial_packet. */

  if(iIndex < SERIAL_MAX_PKT_SIZE)
  {
    packet->checksum = buff[iIndex];
  }

  /* validate the packet */
  ret = validate_packet(&packet, bind_addr);
  if(ret != SERIAL_SUCCESS)
  {
    return ret;
  }

  return iIndex;
}

/***********************************************************************************************/
/*! 
* \fn         :: recv()
* \author     :: Vignesh S
* \date       :: 27-DEC-2020
* \brief      :: This function receives a packet from serial bus. If the device is bound to
*                receive form particular address, then packets from only that source will be
*                recived else, packets from all srouces will be received. use set_bind_addr 
*                to bind to a source address.
* \param[in]  :: SPort, data_buf, buf_size, src_addr, timeout_ms
* \return     :: received packet size or error code
*/
/***********************************************************************************************/
template <typename T>
int8_t recv(T &SPort, uint8_t* data_buf, uint8_t buf_size, uint16_t timeout_ms)
{
  int8_t ret = 0;
  serial_packet packet = {0};
 
  // validate input parameters
  if(data_buf == NULL)
  {
    return SERIAL_ERR_NULL_PTR;
  }

  if((buf_size > MAX_DATA_SIZE) || (buf_size == 0))
  {
    return SERIAL_ERR_INVALID_DATA_SIZE;
  }

  if(is_bound != SERIAL_DEVICE_BOUND)
  {
    return SERIAL_ERR_DEV_NOT_BOUND;
  }

  // receive the packet
  ret = recv_packet(SPort, &packet, timeout_ms);
  if(ret < SERIAL_SUCCESS)
  {
    return ret;
  }
  
  // validate the bind address
  ret = validate_src_addr(&packet, bind_addr);
  if(ret != SERIAL_SUCCESS)
  {
    return ret;
  }

  return ret;
}

/***********************************************************************************************/
/*! 
* \fn         :: recv_no_timeout()
* \author     :: Vignesh S
* \date       :: 27-DEC-2020
* \brief      :: It is recv with no timeout so, it will indefinitely wait for a packet
*                and only return on receiving a packet.
* \param[in]  :: SPort, data_buf, buf_size
* \return     :: received packet size or error code
*/
/***********************************************************************************************/
template <typename T>
int8_t recv_no_timeout(T &SPort, uint8_t* data_buf, uint8_t buf_size)
{
  int8_t ret = 0;

  while(1)
  {
    ret = recv(SPort, data_buf, buf_size, SERIAL_MAX_RECV_TIMEOUT_MS);
    if(ret != (buf_size + SERIAL_MIN_PKT_SIZE))
    {
      if(ret == SERIAL_ERR_RECV_TIMEOUT)
      {
        // ignore timeout error
      }
      else
      {
        return ret;
      }  
    }
    else
    {
      break;
    }
  }

  return ret;
}

/***********************************************************************************************/
/*! 
* \fn         :: recvfrom()
* \author     :: Vignesh S
* \date       :: 27-DEC-2020
* \brief      :: It receives packet only from the requested source address. Do not use
*                set_bind_addr to bind to particular address while using this function
* \param[in]  :: SPort, data_buf, buf_size
* \return     :: received packet size or error code
*/
/***********************************************************************************************/
template <typename T>
int8_t recvfrom(T &SPort, uint8_t src_addr[2], uint8_t* data_buf, uint8_t buf_size, uint16_t timeout_ms)
{
  int8_t ret = 0;
  serial_packet packet = {0};
 
  // validate input parameters
  if(data_buf == NULL)
  {
    return SERIAL_ERR_NULL_PTR;
  }

  if((buf_size > MAX_DATA_SIZE) || (buf_size == 0))
  {
    return SERIAL_ERR_INVALID_DATA_SIZE;
  }

  // receive the packet
  ret = recv_packet(SPort, &packet, timeout_ms);
  if(ret < SERIAL_SUCCESS)
  {
    return ret;
  }
  
  // validate the bind address
  ret = validate_src_addr(&packet, src_addr);
  if(ret != SERIAL_SUCCESS)
  {
    return ret;
  }

  return ret;
}

/***********************************************************************************************/
/*! 
* \fn         :: send_packet()
* \author     :: Vignesh S
* \date       :: 27-DEC-2020
* \brief      :: This function sends data to serial port from packet and returns the number of
*                bytes sent.
* \param[in]  :: SPort, data_buf, data_buf
* \return     :: received packet size or error code
*/
/***********************************************************************************************/
template <typename T>
int8_t send_packet(T &SPort, serial_packet *packet)
{
  int iIndex = 0;
  char *buff = (char*)packet;

  for (iIndex = 0; iIndex < (SERIAL_MIN_PKT_SIZE + packet->data_size); ++iIndex)
  {
    SPort.print(buff[iIndex]);
  }

  return iIndex;
}

/***********************************************************************************************/
/*! 
* \fn         :: send()
* \author     :: Vignesh S
* \date       :: 27-DEC-2020
* \brief      :: This function sends data to serial port from packet and returns the number of
*                bytes sent.
* \param[in]  :: SPort, data_buf, data_size
* \return     :: sent packet size or error code
*/
/***********************************************************************************************/
int8_t send(T &SPort, uint8_t *data_buf, uint8_t data_size)
{
  serial_packet packet = {0};

  // validate input parameters
  if(data_buf == NULL)
  {
    return SERIAL_ERR_NULL_PTR;
  }

  if((buf_size > MAX_DATA_SIZE) || (buf_size == 0))
  {
    return SERIAL_ERR_INVALID_DATA_SIZE;
  }

  if(is_bound != SERIAL_DEVICE_BOUND)
  {
    return SERIAL_ERR_DEV_NOT_BOUND;
  }

  // frame packet before sending
  frame_packet(&packet, data_buf, data_size, bind_addr);

  return send_packet(SPort, packet);
}

/***********************************************************************************************/
/*! 
* \fn         :: sendto()
* \author     :: Vignesh S
* \date       :: 27-DEC-2020
* \brief      :: This function sends data to serial port from packet and returns the number of
*                bytes sent.
* \param[in]  :: SPort, data_buf, data_size
* \return     :: sent packet size or error code
*/
/***********************************************************************************************/
int8_t sendto(T &SPort, uint8_t dst_addr[2], uint8_t *data_buf, uint8_t data_size)
{
  serial_packet packet = {0};

  // validate input parameters
  if(data_buf == NULL)
  {
    return SERIAL_ERR_NULL_PTR;
  }

  if((buf_size > MAX_DATA_SIZE) || (buf_size == 0))
  {
    return SERIAL_ERR_INVALID_DATA_SIZE;
  }

  // frame packet before sending
  frame_packet(&packet, data_buf, data_size, dst_addr);

  return send_packet(SPort, packet);
}

/***********************************************************************************************/
/*! 
* \fn         :: transmit()
* \author     :: Vignesh S
* \date       :: 27-DEC-2020
* \brief      :: This function transmits a command through a serial and receives the response 
* \param[in]  :: SPort, tx_data, tx_data_len, rx_data, rx_data_len, timeout_ms
* \return     :: success or error code
*/
/***********************************************************************************************/
template <typename T>
int8_t transmit(T &SPort, uint8_t* tx_data, uint8_t tx_data_len, uint8_t* rx_data, uint8_t rx_data_len,  uint16_t timeout_ms)
{
  int8_t ret = 0;
  
  ret = send(SPort, tx_data, tx_data_len);
  if(ret != (tx_data_len + SERIAL_MIN_PKT_SIZE))
  {
   return ret; 
  }
  
  ret = recv(SPort, rx_data, rx_data_len, timeout_ms);
  if(ret != (rx_data_len + SERIAL_MIN_PKT_SIZE))
  {
    return ret;
  }

  return SERIAL_SUCCESS;
}

/***********************************************************************************************/
/*! 
* \fn         :: transmit()
* \author     :: Vignesh S
* \date       :: 27-DEC-2020
* \brief      :: This function transmits a command through a serial and receives the response 
* \param[in]  :: SPort, dst_addr, tx_data, tx_data_len, rx_data, rx_data_len, timeout_ms
* \return     :: success or error code
*/
/***********************************************************************************************/
template <typename T>
int8_t transmit_to(T &SPort, uint8_t dst_addr[2], uint8_t* tx_data, uint8_t tx_data_len, uint8_t* rx_data, uint8_t rx_data_len,  uint16_t timeout_ms)
{
  int8_t ret = 0;
  
  ret = sendto(SPort, dst_addr, tx_data, tx_data_len);
  if(ret != (tx_data_len + SERIAL_MIN_PKT_SIZE))
  {
   return ret; 
  }
  
  ret = recvfrom(SPort, dst_addr, rx_data, rx_data_len, timeout_ms);
  if(ret != (rx_data_len + SERIAL_MIN_PKT_SIZE))
  {
    return ret;
  }

  return SERIAL_SUCCESS;
}

/***********************************************************************************************/

#endif // _SERIAL_COMM_H_