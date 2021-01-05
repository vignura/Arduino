#ifndef _SWITCH_H_
#define _SWITCH_H_

#include <stdint.h>
#include <Arduino.h>

// #define _DEBUG_
#define ENABLE_DEBOUNCE
#define DEBUG_MSG_SIZE		64

/* default values */
#define SWITCH_OFF					0
#define SWITCH_ON					1
#define SWITCH_DEBOUNCE_TIME_MS		50

class Switch
{
	bool m_state_changed;
	uint8_t m_pin;
	uint8_t m_state;
	uint8_t m_tmp_state;
	uint32_t m_start_time_ms;
	uint32_t m_debounce_time_ms;
public:
	Switch(uint8_t m_pin, uint8_t state);
	Switch::Switch(uint8_t pin, uint8_t state, uint32_t debounce_time_ms);
	uint8_t getPin();
	uint8_t getState();
	bool isStateChanged();
	void SwitchTask();
	~Switch();
};

#endif // _SWITCH_H_