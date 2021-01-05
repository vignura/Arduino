#define ZC_PIN			2/* AC opto feedback */
#define PWM_PIN			9
#define PWM_PIN_1		10
#define SAMPLE_COUNT	120

#define PULSES_PER_CYCLE 				2
#define STATE_CHANGE_PER_PULSE 			2
#define STATE_CHANGE_PER_CYCLE			(PULSES_PER_CYCLE * STATE_CHANGE_PER_PULSE)
#define FREQ_DIVISOR					100UL
#define TRIG_PULSE_LENGTH_US			100
#define DUTY_CYCLE_PERCENTAGE			50
#define DUTY_CHANGE_PERIOD_SEC			2
#define DUTY_CHANGE_PERCENTAGE_STEP		2
#define TIMER_COUNT						2474	/* 101.05 Hz*/
#define DUTY_CYCLE_COUNT				25		/* 100 us*/

#define NO_CLK_SRC                      (0)
#define PRESCALR_1                      (_BV(CS10))
#define PRESCALR_8                      (_BV(CS11))
#define PRESCALR_64                     (_BV(CS11) | _BV(CS10))
#define PRESCALR_256                    (_BV(CS12))
#define PRESCALR_1024                   (_BV(CS11) | _BV(CS10))
#define EXT_CLK_SRC_RISING              (_BV(CS12) | _BV(CS11))
#define EXT_CLK_SRC_FALING              (_BV(CS12) | _BV(CS11) | _BV(CS10))

char Msg[64];

void setup() {
 	
 	pinMode(ZC_PIN, INPUT);
 	pinMode(PWM_PIN, OUTPUT);
 	pinMode(PWM_PIN_1, OUTPUT);
 	disable_pwm();
	Serial.begin(115200);
	
	sprintf(Msg, "Triac Drive");
	Serial.println(Msg);
	//trigger_triac_pwm_100Hz(10);
	setup_pwm();
}

void loop() {

	// uint32_t freq = 0;
	// uint32_t time_sec = 0;
	// int16_t duty_cycle = DUTY_CHANGE_PERCENTAGE_STEP;

	// /* compute and print frequency */
	// freq = compute_line_frequency(SAMPLE_COUNT);
	// sprintf(Msg, "frequency: %d.%d%d Hz", (freq / FREQ_DIVISOR), ((freq / 10) % 10), (freq % 10));
	// Serial.println(Msg);
	// sprintf(Msg, "Duty cyle: %d %%", duty_cycle);
	// Serial.println(Msg);

	// while(1)
	// {	
	// 	if((((millis() / 1000) % DUTY_CHANGE_PERIOD_SEC) == 0) && ((millis() / 1000) - time_sec) > 1)
	// 	{
	// 		// duty_cycle += DUTY_CHANGE_PERCENTAGE_STEP;
	// 		duty_cycle = 5;
	// 		if(duty_cycle > 100)
	// 		{
	// 			duty_cycle = DUTY_CHANGE_PERCENTAGE_STEP;
	// 		}
	// 		sprintf(Msg, "Duty cyle: %d %%", duty_cycle);
	// 		Serial.println(Msg);				
	// 		time_sec = (millis() / 1000);
	// 		// trigger_triac_pwm_100Hz(duty_cycle);
	// 		// trigger_triac_pwm_100Hz(30);
	// 	}

	// 	// trigger_triac(duty_cycle, ((float)freq / (float)FREQ_DIVISOR));
	// }
}

uint16_t compute_line_frequency(uint16_t sample_count)
{
	uint16_t change_count = 0;
	uint16_t cur_state = LOW;
	uint16_t pre_state = LOW;
	uint32_t start_time_ms = 0;
	uint32_t end_time_ms = 0;

	start_time_ms = millis();

	while(change_count < sample_count)
	{
		cur_state = digitalRead(ZC_PIN);

		if(cur_state != pre_state)
		{
			change_count++;
		}

		pre_state = cur_state;
	}

	end_time_ms = millis();

	return ((sample_count / STATE_CHANGE_PER_CYCLE) * 1000UL * FREQ_DIVISOR) / (end_time_ms - start_time_ms); 
}

void trigger_triac(float duty_cycle, uint16_t frequency)
{
	uint16_t cur_state = LOW;
	uint16_t pre_state = LOW;
	float trg_delay_us = ((1.0f / (2.0f * (float)frequency)) * (float)1000000.0f * (1.0f - (duty_cycle / 100.0f)));

	cur_state = digitalRead(ZC_PIN);
	if(cur_state != pre_state)
	{
		delayMicroseconds(trg_delay_us);
		digitalWrite(PWM_PIN, HIGH);
		delayMicroseconds(TRIG_PULSE_LENGTH_US);
		digitalWrite(PWM_PIN, LOW);
	}
}

void trigger_triac_pwm_100Hz(float duty_cycle)
{
	float trg_delay_us = ((float)10000.0f * (1.0f - (duty_cycle / 100.0f)));

	while(1)
	{
		if(digitalRead(ZC_PIN) == HIGH)
		{
			break;
		}
	}

	// delayMicroseconds((uint32_t)trg_delay_us);
	// delayMicroseconds((8000UL));
	setup_pwm();
}


void setup_pwm()
{
    // clear the counter
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1  = 0;
    
    // non-inverted PWM on ch A and ch B
    TCCR1A = _BV(COM1A1) | _BV(COM1B1);
    // mode 14: fast PWM, TOP = ICR1
    TCCR1A |= _BV(WGM11);          
    TCCR1B |= (_BV(WGM12) |_BV(WGM13));
    
    // set prescalar
    TCCR1B |= PRESCALR_64;

    ICR1 = (TIMER_COUNT -1);
    OCR1A = DUTY_CYCLE_COUNT;
}

void disable_pwm()
{
	TCCR1A = 0;
    TCCR1B = 0;
    TCNT1  = 0;
}