#include "serial_com.h"

static uint8_t dev_addr[2];
static uint8_t bind_addr[2];
static uint8_t is_bound;

/***********************************************************************************************/
/*! 
* \fn         :: set_dev_addr()
* \author     :: Vignesh S
* \date       :: 29-DEC-2020
* \brief      :: This function sets the address of the serial device. The address will be used
*                for receving packets and sending packets
* \param[in]  :: address
* \param[out] :: None
* \return     :: SERIAL_SUCCESS or error code 
*/
/***********************************************************************************************/
int8_t set_dev_addr(uint8_t address[2])
{
  if(address == NULL)
  {
    return SERIAL_ERR_NULL_PTR;
  }
  
  dev_addr[0] = address[0];
  dev_addr[1] = address[1];

  return SERIAL_SUCCESS;
}

/***********************************************************************************************/
/*! 
* \fn         :: set_address()
* \author     :: Vignesh S
* \date       :: 29-DEC-2020
* \brief      :: This function sets the address of the serial device. The address will be used
*                for receving packets and sending packets
* \param[in]  :: address
* \param[out] :: None
* \return     :: SERIAL_SUCCESS or error code 
*/
/***********************************************************************************************/
int8_t set_bind_addr(uint8_t address[2])
{
  if(address == NULL)
  {
    return SERIAL_ERR_NULL_PTR;
  }
  
  bind_addr[0] = address[0];
  bind_addr[1] = address[1];
  is_bound = SERIAL_DEVICE_BOUND;

  return SERIAL_SUCCESS;
}

/***********************************************************************************************/
/*! 
* \fn         :: validate_packet()
* \author     :: Vignesh S
* \date       :: 27-DEC-2020
* \brief      :: This function validates the received packet's header, checksum, and
*                destination address.
* \param[in]  :: packet, packet_size
* \param[out] :: out_iCmdID
* \return     :: SERIAL_SUCCESS or error code
*/
/***********************************************************************************************/
int8_t validate_packet(serial_packet *packet)
{
  uint8_t checksum = 0;

  // validate header
  if(packet->header != SERIAL_HEADER)
  {
    return SERIAL_ERR_INVALID_HEADER;
  }
  
  // validate checksum 
  if(compute_checksum(packet) != packet->checksum)
  {
    return SERIAL_ERR_INVALID_CHKSUM;
  }

  // validate destination address
  if((packet->dst_addr[0] != dev_addr[0]) || (packet->dst_addr[1] != dev_addr[1]))
  {
    return SERIAL_ERR_DST_ADDR;
  }

  // validate data size
  if(data_size > MAX_DATA_SIZE)
  {
    return SERIAL_ERR_INVALID_DATA_SIZE;
  }

  return SERIAL_SUCCESS;
}

/***********************************************************************************************/
/*! 
* \fn         :: validate_src_addr()
* \author     :: Vignesh S
* \date       :: 27-DEC-2020
* \brief      :: This function validates the received packet's source address.
* \param[in]  :: packet, src_addr
* \param[out] :: None
* \return     :: SERIAL_SUCCESS or error code
*/
/***********************************************************************************************/
int8_t validate_src_addr(serial_packet *packet, uint8_t src_addr[2])
{
  // if device is bound to bind address, then we receive only form 
  // that source address 
  if((packet->src_addr[0] != src_addr[0]) || (packet->src_addr[1] != src_addr[1]))
  {
    return SERIAL_ERR_SRC_ADDR;
  }
  
  return SERIAL_SUCCESS; 
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
  uint8_t *data = (uint8_t *)packet;

  for(int i = 0; i < (packet->data_size + SERIAL_MIN_PKT_SIZE); ++i)
  {
    /* code */
    checksum ^= data[i];
  }

  return checksum;
}

/***********************************************************************************************/
/*! 
* \fn         :: frame_packet()
* \author     :: Vignesh S
* \date       :: 27-DEC-2020
* \brief      :: This function frames a serial packet
* \param[in]  :: packet, data_buf, data_size, dst_addr
* \param[out] :: none
* \return     :: checksum
*/
/***********************************************************************************************/
void frame_packet(serial_packet* packet, uint8_t *data_buf, uint8_t data_size, uint8_t dst_addr[2])
{
  packet->header = SERIAL_HEADER;
  packet->src_addr[0] = dev_addr[0]; 
  packet->src_addr[1] = dev_addr[1];
  packet->dst_addr[0] = dst_addr[0];
  packet->dst_addr[1] = dst_addr[1];
  packet->data_size = data_size;

  // copy the data
  for(int i = 0; i < data_size; i++)
  {
    packet->data[i] = data_buf[i];
  }

  packet->checksum = compute_checksum(packet);
}