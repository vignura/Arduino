#define ZC_PIN			2/* AC opto feedback */
#define PWM_PIN			3
#define SAMPLE_COUNT	120

#define PULSES_PER_CYCLE 				2
#define STATE_CHANGE_PER_PULSE 			2
#define STATE_CHANGE_PER_CYCLE			(PULSES_PER_CYCLE * STATE_CHANGE_PER_PULSE)
#define FREQ_DIVISOR					100UL
#define TRIG_PULSE_LENGTH_US			100
#define DUTY_CYCLE_PERCENTAGE			50
#define DUTY_CHANGE_PERIOD_SEC			10
#define DUTY_CHANGE_PERCENTAGE_STEP		5
#define TIMER_COUNT						104

char Msg[64];

void setup() {
 	
 	pinMode(ZC_PIN, INPUT);
 	pinMode(PWM_PIN, OUTPUT);
 	setup_pwm_50Hz();

	Serial.begin(115200);
	
	sprintf(Msg, "Triac Drive");
	Serial.println(Msg);
}

void loop() {

	uint32_t freq = 0;
	uint32_t time_sec = 0;
	int16_t duty_cycle = DUTY_CHANGE_PERCENTAGE_STEP;

	/* compute and print frequency */
	freq = compute_line_frequency(SAMPLE_COUNT);
	sprintf(Msg, "frequency: %d.%d%d Hz", (freq / FREQ_DIVISOR), ((freq / 10) % 10), (freq % 10));
	Serial.println(Msg);
	sprintf(Msg, "Duty cyle: %d %%", duty_cycle);
	Serial.println(Msg);

	while(1)
	{	
		if((((millis() / 1000) % DUTY_CHANGE_PERIOD_SEC) == 0) && ((millis() / 1000) - time_sec) > 1)
		{
			duty_cycle += DUTY_CHANGE_PERCENTAGE_STEP;
			if(duty_cycle > 100)
			{
				duty_cycle = DUTY_CHANGE_PERCENTAGE_STEP;
			}
			sprintf(Msg, "Duty cyle: %d %%", duty_cycle);
			Serial.println(Msg);				
			time_sec = (millis() / 1000);
		}

		trigger_triac(duty_cycle, ((float)freq / (float)FREQ_DIVISOR));
	}
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

void trigger_triac_pwm(float duty_cycle)
{
	uint16_t cur_state = LOW;
	uint16_t pre_state = LOW;
	float trg_delay_us = ((float)20000.0f * (1.0f - (duty_cycle / TIMER_COUNT)));

	cur_state = digitalRead(ZC_PIN);
	if(cur_state != pre_state)
	{
		delayMicroseconds(trg_delay_us);
	}
}


void setup_pwm_50Hz()
{
	// timer 2
    TCCR2A = 0;
    TCCR2B = 0;
    TCNT2  = 0;
    TCCR2A = _BV(COM2B1)  // non-inverted PWM on ch. B
        | _BV(WGM20);  // mode 5: ph. correct PWM, TOP = OCR2A
    TCCR2B = _BV(WGM22)   // ditto
        | _BV(CS22) | _BV(CS21);   // prescaler = 256
    OCR2A  = 104;
}