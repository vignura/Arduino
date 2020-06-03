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
	bool m_isActiveLow;

	uint8_t m_State;
	uint8_t m_RelayPin;

	uint32_t m_OnTimeSec;
	uint32_t m_StartTime_ms;

public:
	Relay(uint8_t RelayPin);
	Relay(uint8_t RelayPin, uint8_t State);
	Relay(uint8_t RelayPin, bool isActiveLow);
	Relay(uint8_t RelayPin, uint8_t State, bool isActiveLow);

	uint8_t setActiveLow(bool isActiveLow);
	uint8_t getState();
	void setState(uint8_t State);
	void ToggleState();

	void setTimer(uint32_t OnTimeSec);
	void stopTimer();
	void TimerTask();
};

#endif // _RELAY_H_