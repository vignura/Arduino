#include "serial_com.h"

static uint8_t dev_addr[2];
static uint8_t bind_addr[2];
static uint8_t is_bound;

#ifdef _DEBUG_
static char DMsg[DEBUG_MSG_SIZE] = {0}; 
#endif
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
  checksum = compute_checksum(packet);
  if(checksum != packet->checksum)
  {
    #ifdef _DEBUG_
      sprintf(DMsg, "Ex: 0x%x Ob: 0x%x", checksum, packet->checksum);
      Serial.println(DMsg);
    #endif
    return SERIAL_ERR_INVALID_CHKSUM;
  }

  // validate destination address
  if((packet->dst_addr[0] != dev_addr[0]) || (packet->dst_addr[1] != dev_addr[1]))
  {
    return SERIAL_ERR_DST_ADDR;
  }

  // validate data size
  if((packet->pkt_size < SERIAL_MIN_PKT_SIZE) || (packet->pkt_size > SERIAL_MAX_PKT_SIZE))
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

  if((src_addr[0] == SERIAL_ADDR_ANY_LSB) && (src_addr[1] == SERIAL_ADDR_ANY_MSB))
  {
    // no source address validation is required for this
  }
  else
  {
    if((packet->src_addr[0] != src_addr[0]) || (packet->src_addr[1] != src_addr[1]))
    {
      return SERIAL_ERR_SRC_ADDR;
    }
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
  uint8_t i = 0;
  uint8_t checksum = 0;
  uint8_t* data = (uint8_t*)packet;

  /* -1 for excluding the checksum byte */
  for(i = 0; i < (packet->pkt_size -1); i++)
  {
    checksum ^= data[i];
    #ifdef _DEBUG_
      sprintf(DMsg, "CK [%d] 0x%x 0x%x", i, checksum, data[i]);
      Serial.println(DMsg);
    #endif
  }

  return checksum;
}

/***********************************************************************************************/
/*! 
* \fn         :: frame_packet()
* \author     :: Vignesh S
* \date       :: 27-DEC-2020
* \brief      :: This function frames a serial packet
* \param[in]  :: packet, data_buf, buf_size, dst_addr
* \param[out] :: none
* \return     :: checksum
*/
/***********************************************************************************************/
void frame_packet(serial_packet* packet, uint8_t *data_buf, uint8_t buf_size, uint8_t dst_addr[2])
{
  uint8_t checksum = 0;
  packet->header = SERIAL_HEADER;
  packet->pkt_size = SERIAL_MIN_PKT_SIZE + buf_size;
  packet->src_addr[0] = dev_addr[0]; 
  packet->src_addr[1] = dev_addr[1];
  packet->dst_addr[0] = dst_addr[0];
  packet->dst_addr[1] = dst_addr[1];

  // copy the data
  for(int i = 0; i < buf_size; i++)
  {
    packet->data[i] = data_buf[i];
  }

  checksum = compute_checksum(packet);
  packet->checksum = checksum;
}

/***********************************************************************************************/
/*! 
* \fn         :: str_to_addr()
* \author     :: Vignesh S
* \date       :: 30-DEC-2020
* \brief      :: This function extracts address from the sent string
* \param[in]  :: string
* \param[out] :: addr
* \return     :: SERIAL_SUCCESS or SERIAL_FAILURE
*/
/***********************************************************************************************/
int8_t str_to_addr(char* string, uint8_t addr[2])
{
  int8_t ret = 0;

  if((string == NULL) || (addr == NULL))
  {
    return SERIAL_ERR_NULL_PTR;
  }

  ret = sscanf(string, "%d.%d", &addr[0], &addr[1]);
  if(ret != 2)
  {
    return SERIAL_FAILURE;
  }
  
  return SERIAL_SUCCESS;
}

/***********************************************************************************************/
/*! 
* \fn         :: get_err_str()
* \author     :: Vignesh S
* \date       :: 30-DEC-2020
* \brief      :: This function return the error string corresponding error code
* \param[in]  :: err, str_buf, buf_size
* \param[out] :: str_buf
* \return     :: SERIAL_SUCCESS or error code
*/
/***********************************************************************************************/
int8_t get_err_str(int8_t err, char* str_buf, uint8_t buf_size)
{
  if(str_buf == NULL)
  {
    return SERIAL_ERR_NULL_PTR;
  }

  switch(err)
  {
    case SERIAL_ERR_NULL_PTR:
      snprintf(str_buf, buf_size, "ERR %d: null pointer", err);
    break;

    case SERIAL_ERR_INVALID_HEADER:
      snprintf(str_buf, buf_size, "ERR %d: invalid header", err);
    break;

    case SERIAL_ERR_INVALID_CHKSUM:
      snprintf(str_buf, buf_size, "ERR %d: invalid checksum", err);
    break;
    
    case SERIAL_ERR_DST_ADDR:
      snprintf(str_buf, buf_size, "ERR %d: invalid destination", err);
    break;

    case SERIAL_ERR_SRC_ADDR:
      snprintf(str_buf, buf_size, "ERR %d: invalid source", err);
    break;

    case SERIAL_ERR_INVALID_DATA_SIZE:
      snprintf(str_buf, buf_size, "ERR %d: invalid data size", err);
    break;

    case SERIAL_ERR_INVALID_PKT_SIZE:
      snprintf(str_buf, buf_size, "ERR %d: invalid packet size", err);
    break;
    
    case SERIAL_ERR_RECV_TIMEOUT:
      snprintf(str_buf, buf_size, "ERR %d: receive timeout", err);
    break;

    case SERIAL_ERR_DEV_NOT_BOUND:
      snprintf(str_buf, buf_size, "ERR %d: device not bound", err);
    break;

    default:
      snprintf(str_buf, buf_size, "ERR %d: unknown error", err);
    break;    
  }

  return SERIAL_SUCCESS;
}