#include "serial_com.h"

/***********************************************************************************************/
/*! 
* \fn         :: is_valid_packet()
* \author     :: Vignesh S
* \date       :: 27-DEC-2020
* \brief      :: This function validates the received packet and return true if it a valid 
*                command else returns false.
* \param[in]  :: packet, packet_size
* \param[out] :: out_iCmdID
* \return     :: true or false
*/
/***********************************************************************************************/
bool is_valid_packet(serial_packet *packet, uint8_t address[2])
{
  uint8_t checksum = 0;

  // validate header
  if((packet->header != SERIAL_CMD_HEADER) && (packet->header != SERIAL_RES_HEADER))
  {
    return false;
  }
  
  // validate checksum 
  if(compute_checksum(packet) != packet->checksum)
  {
    return false;
  }

  // validate address
  if((packet->address[0] != address[0]) || (packet->address[1] != address[1]))
  {
    return false;
  }

  return true;
}

/***********************************************************************************************/
/*! 
* \fn         :: compute_checksum()
* \author     :: Vignesh S
* \date       :: 27-DEC-2020
* \brief      :: This function computes checksum for the given packet
* \param[in]  :: packet
* \param[out] :: none
* \return     :: checksum
*/
/***********************************************************************************************/
uint8_t compute_checksum(serial_packet *packet)
{
  uint8_t checksum = 0;

  // compute checksum
  checksum ^= packet->header;
  checksum ^= packet->address[0];
  checksum ^= packet->address[1];
  checksum ^= packet->cmdID;

  for (int i = 0; i < ((packet->data_size) - SERIAL_HEADER_SIZE); ++i)
  {
    /* code */
    checksum ^= packet->data[i];
  }

  return checksum;
}