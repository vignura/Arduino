#define REF_OUT_PIN						9
#define INPUT_PIN						2
#define SAMPLE_COUNT					120
#define PULSES_PER_CYCLE 				1 /* 1 for PWM and 2 for AC */
#define STATE_CHANGE_PER_PULSE 			2
#define STATE_CHANGE_PER_CYCLE			(PULSES_PER_CYCLE * STATE_CHANGE_PER_PULSE)

#define TIMER_COUNT						2478	/* 100 Hz*/
#define DUTY_CYCLE_COUNT				(TIMER_COUNT / 2)	/* 50 % ON */
#define NO_CLK_SRC                      (0)
#define PRESCALR_1                      (_BV(CS10))
#define PRESCALR_8                      (_BV(CS11))
#define PRESCALR_64                     (_BV(CS11) | _BV(CS10))
#define PRESCALR_256                    (_BV(CS12))
#define PRESCALR_1024                   (_BV(CS11) | _BV(CS10))
#define EXT_CLK_SRC_RISING              (_BV(CS12) | _BV(CS11))
#define EXT_CLK_SRC_FALING              (_BV(CS12) | _BV(CS11) | _BV(CS10))

char Msg[64];

void setup()
{
	pinMode(INPUT_PIN, INPUT);
	pinMode(REF_OUT_PIN, OUTPUT);
	setup_pwm();
	Serial.begin(115200);
	sprintf(Msg, "frequency counter");
	Serial.println(Msg);
}

void loop()
{
	/* compute and print frequency */
	float freq = compute_line_frequency(SAMPLE_COUNT);
	
	sprintf(Msg, "frequency:");
	Serial.print(Msg);
	Serial.print(freq, 3);
	sprintf(Msg, " Hz");
	Serial.println(Msg);
	// delay(1 * 1000);
}

float compute_line_frequency(uint16_t sample_count)
{
	uint16_t change_count = 0;
	uint16_t cur_state = LOW;
	uint16_t pre_state = LOW;
	uint32_t start_time_us = 0;
	uint32_t time_diff_us = 0;

	while(1)
	{
		cur_state = digitalRead(INPUT_PIN);

		if(cur_state != pre_state)
		{
			if(change_count == 0)
			{
				start_time_us = micros();
			}
			else if(change_count == (sample_count -1))
			{
				time_diff_us = micros() - start_time_us;
				break;
			}

			change_count++;
		}

		pre_state = cur_state;
	}

	// sprintf(Msg, "Sample count: %d\t cycles: %d\ttime: %ld us", sample_count, (sample_count / STATE_CHANGE_PER_CYCLE), time_diff_us);
	// Serial.println(Msg);

	return (( (((float)sample_count / (float)STATE_CHANGE_PER_CYCLE) -0.5f) * 1000000.0f) / (float)(time_diff_us)); 
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