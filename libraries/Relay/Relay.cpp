#include "Relay.h"

Relay::Relay(uint8_t RelayPin)
{
	m_IsTimerSet = false;
	m_isActiveLow = false;
	m_OnTimeSec = 0;
	m_StartTime_ms = 0;

	m_RelayPin = RelayPin;
	pinMode(m_RelayPin, OUTPUT);
	setState(RELAY_OFF);
}

Relay::Relay(uint8_t RelayPin, uint8_t State)
{
	m_IsTimerSet = false;
	m_isActiveLow = false;
	m_OnTimeSec = 0;
	m_StartTime_ms = 0;

	m_RelayPin = RelayPin;
	pinMode(m_RelayPin, OUTPUT);
	setState(State);
}

Relay::Relay(uint8_t RelayPin, bool isActiveLow)
{
	m_IsTimerSet = false;
	m_isActiveLow = isActiveLow;
	m_OnTimeSec = 0;
	m_StartTime_ms = 0;

	m_RelayPin = RelayPin;
	pinMode(m_RelayPin, OUTPUT);
	setState(RELAY_OFF);
}

Relay::Relay(uint8_t RelayPin, uint8_t State, bool isActiveLow)
{
	m_IsTimerSet = false;
	m_isActiveLow = isActiveLow;
	m_OnTimeSec = 0;
	m_StartTime_ms = 0;
	
	m_RelayPin = RelayPin;
	pinMode(m_RelayPin, OUTPUT);
	setState(State);
}

void Relay::setState(uint8_t State)
{
	switch(State)
	{
		case RELAY_OFF:
			m_State = RELAY_OFF;
			if(m_isActiveLow)
			{	
				digitalWrite(m_RelayPin, HIGH);
			}
			else
			{
				digitalWrite(m_RelayPin, LOW);	
			}
		break;

		case RELAY_ON:
			m_State = RELAY_ON;
			if(m_isActiveLow)
			{	
				digitalWrite(m_RelayPin, LOW);
			}
			else
			{
				digitalWrite(m_RelayPin, HIGH);	
			}
		break;

		default:
			m_State = RELAY_OFF;
	}
}

void Relay::ToggleState()
{
	if(m_State == RELAY_ON)
	{
		setState(RELAY_OFF);
	}
	else
	{
		setState(RELAY_ON);	
	}
}

uint8_t Relay::setActiveLow(bool isActiveLow)
{
	m_isActiveLow = isActiveLow;
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