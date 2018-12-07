#include "Relay.h"

Relay::Relay(uint8_t RelayPin)
{
	m_RelayPin = RelayPin;
	pinMode(m_RelayPin, OUTPUT);
	setState(RELAY_OFF);

	m_IsTimerSet = false;
	m_OnTimeSec = 0;
	m_StartTime_ms = 0;
}

Relay::Relay(uint8_t RelayPin, uint8_t State)
{
	m_RelayPin = RelayPin;
	pinMode(m_RelayPin, OUTPUT);
	setState(State);

	m_IsTimerSet = false;
	m_OnTimeSec = 0;
	m_StartTime_ms = 0;
}

void Relay::setState(uint8_t State)
{
	switch(State)
	{
		case RELAY_OFF:
			m_State = RELAY_OFF;
			digitalWrite(m_RelayPin, LOW);
		break;

		case RELAY_ON:
			m_State = RELAY_ON;
			digitalWrite(m_RelayPin, HIGH);
		break;

		default:
			m_State = RELAY_OFF;
	}
}

uint8_t Relay::getState()
{
	return m_State;
}

void Relay::setTimer(uint32_t OnTimeSec)
{
	m_OnTimeSec = OnTimeSec;

	// get the start time 
	m_StartTime_ms = millis();

	// turn on the relay
	setState(RELAY_ON);
	m_IsTimerSet = true;
}

void Relay::stopTimer()
{
	m_OnTimeSec = 0;
	m_StartTime_ms = 0;

	// turn off the relay	
	setState(RELAY_OFF);
	m_IsTimerSet = false;
}

void Relay::TimerTask()
{
	uint32_t CurTime_ms = 0;

	if(m_IsTimerSet == true)
	{
		if(m_State == RELAY_ON)
		{
			// get current time
			CurTime_ms = millis();
			if(((CurTime_ms - m_StartTime_ms) / 1000) >= m_OnTimeSec)
			{
				stopTimer();
			}
		}
		else
		{
			stopTimer();
		}
	}
}