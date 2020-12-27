#ifndef _CMD_PROCESS_H_
#define _CMD_PROCESS_H_

/* command ID */
#define CMD_TEST_ID             1
#define CMD_RLY_ON_ID           2
#define CMD_RLY_ON_TIMER_ID     3
#define CMD_RLY_OFF_ID          4
#define CMD_RLY_OFF_ALL_ID      5
#define MAX_CMD_ID              (CMD_RLY_OFF_ALL_ID)

// CMD_RLY_ON_ID
typedef struct relay_on{
  
  uint8_t relay_num;

}relay_on;

// CMD_RLY_ON_TIMER_ID
typedef struct relay_on_timer{
  
  uint8_t relay_num;  
  uint32_t on_time_sec;

}relay_on_timer;

// CMD_RLY_OFF_ID
typedef struct relay_off{
  
  uint8_t relay_num;
  
}relay_off;

typedef union data_packets{

  uint16_t ptr;
  relay_on  *Prelay_on;
  relay_on_timer *Prelay_on_timer;
  relay_off *Prelay_off;

}data_packets;

#endif // _CMD_PROCESS_H_