#include "Switch.h"

extern HardwareSerial Serial;
#ifdef _DEBUG_
static char DMsg[DEBUG_MSG_SIZE] = {0}; 
#endif

Switch::Switch(uint8_t pin, uint8_t state)
{
	m_pin = pin;
	m_state = state;
	m_state_changed = false;
	m_tmp_state = 0;
	m_debounce_time_ms = SWITCH_DEBOUNCE_TIME_MS;
	m_start_time_ms = 0;

	pinMode(m_pin, INPUT);

	#ifdef _DEBUG_
      sprintf(DMsg, "pin: %d state: %d", pin, state);
      Serial.println(DMsg);
    #endif
}

Switch::Switch(uint8_t pin, uint8_t state, uint32_t debounce_time_ms)
{
	m_pin = pin;
	m_state = state;
	m_debounce_time_ms = debounce_time_ms;

	m_state_changed = false;
	m_tmp_state = 0;
	m_start_time_ms = 0;

	pinMode(m_pin, INPUT);

	#ifdef _DEBUG_
      sprintf(DMsg, "pin: %d state: %d debounce time: %lu ms", pin, state, m_debounce_time_ms);
      Serial.println(DMsg);
    #endif
}

uint8_t Switch::getPin()
{
	return m_pin;
}

uint8_t Switch::getState()
{
	m_state_changed = false;
	return m_state;
}

bool Switch::isStateChanged()
{
	return m_state_changed;
}

void Switch::SwitchTask()
{
	uint32_t time_ms = millis();

#ifdef ENABLE_DEBOUNCE
	if(m_start_time_ms == 0)
	{
		m_tmp_state = digitalRead(m_pin);
		if(m_tmp_state != m_state)
		{
			m_start_time_ms = time_ms;
			#ifdef _DEBUG_
		      sprintf(DMsg, "[%d] debounce: %lu ms start: %lu ms time: %lu elapsed: %lu ms", m_pin, m_debounce_time_ms, m_start_time_ms, time_ms, (time_ms - m_start_time_ms));
		      Serial.println(DMsg);
		    #endif
		}
	}
	else if((time_ms - m_start_time_ms) < m_debounce_time_ms)
	{
		#ifdef _DEBUG_
	      sprintf(DMsg, "[%d] debounce: %lu ms start: %lu ms time: %lu elapsed: %lu ms", m_pin, m_debounce_time_ms, m_start_time_ms, time_ms, (time_ms - m_start_time_ms));
	      Serial.println(DMsg);
	    #endif

		if(digitalRead(m_pin) != m_tmp_state)
		{
			m_start_time_ms = 0;

			#ifdef _DEBUG_
		      sprintf(DMsg, "[%d] reseting time", m_pin);
		      Serial.println(DMsg);
		    #endif
		}
	}
	else
	{
		#ifdef _DEBUG_
		  sprintf(DMsg, "[%d] debounce: %lu ms start: %lu ms time: %lu elapsed: %lu ms", m_pin, m_debounce_time_ms, m_start_time_ms, time_ms, (time_ms - m_start_time_ms));
		  Serial.println(DMsg);
		#endif

		if(digitalRead(m_pin) == m_tmp_state)
		{
			m_state = m_tmp_state;
			m_state_changed = true;
			#ifdef _DEBUG_
		      sprintf(DMsg, "[%d] state changed", m_pin);
		      Serial.println(DMsg);
		    #endif
		}
		m_start_time_ms = 0;

		#ifdef _DEBUG_
	      sprintf(DMsg, "[%d] reseting time", m_pin);
	      Serial.println(DMsg);
	    #endif
	}
#else
	m_tmp_state = digitalRead(m_pin);
	if(m_tmp_state != m_state)
	{
		m_state = m_tmp_state;
		m_state_changed = true;
	}
#endif

}

Switch::~Switch()
{

}