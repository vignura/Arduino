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
int RecvCmd(SoftwareSerial &SSPort, char *pBuff, int iBuflen);
int RecvCmd(HardwareSerial &SSPort, char *pBuff, int iBuflen);
int recv_packet(SoftwareSerial &SSPort, serial_packet *packet);
int send_packet(SoftwareSerial &SSPort, serial_packet *packet);
int transmit_command(SoftwareSerial &SSPort, serial_packet *cmd_packet, serial_packet *res_packet, int timeout_ms);
bool is_valid_packet(serial_packet *packet, uint8_t address[2]);
uint8_t compute_checksum(serial_packet *packet);
/***********************************************************************************************/

#endif // _SERIAL_COMM_H_