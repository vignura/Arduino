#ifndef _RELAY_H_
#define _RELAY_H_

#include <stdint.h>
#include <Arduino.h>

#define RELAY_SUCCESS					0
#define RELAY_FAILURE					-1

#define RELAY_OFF						0x01
#define RELAY_ON						0x02

class Relay
{
	bool m_IsTimerSet;

	uint8_t m_State;
	uint8_t m_RelayPin;

	uint32_t m_OnTimeSec;
	uint32_t m_StartTime_ms;

public:
	Relay(uint8_t RelayPin);
	Relay(uint8_t RelayPin, uint8_t State);

	void setState(uint8_t State);
	uint8_t getState();

	void setTimer(uint32_t OnTimeSec);
	void stopTimer();
	void TimerTask();
};

#endif // _RELAY_H_