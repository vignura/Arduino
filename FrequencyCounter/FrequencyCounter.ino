#define INPUT_PIN						2
#define SAMPLE_COUNT					10
#define PULSES_PER_CYCLE 				1 /* 1 for PWM and 2 for AC */
#define STATE_CHANGE_PER_PULSE 			2
#define STATE_CHANGE_PER_CYCLE			(PULSES_PER_CYCLE * STATE_CHANGE_PER_PULSE)

char Msg[64];

void setup()
{
	pinMode(INPUT_PIN, INPUT);
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


	delay(2 * 1000);
}

float compute_line_frequency(uint16_t sample_count)
{
	uint16_t change_count = 0;
	uint16_t cur_state = LOW;
	uint16_t pre_state = LOW;
	uint32_t start_time_ms = 0;
	uint32_t time_diff_ms = 0;

	start_time_ms = millis();

	while(change_count < sample_count)
	{
		cur_state = digitalRead(INPUT_PIN);

		if(cur_state != pre_state)
		{
			change_count++;
		}

		pre_state = cur_state;
	}

	time_diff_ms = millis() - start_time_ms;
	//sprintf(Msg, "Sample count: %d\t cycles: %d\ttime: %d ms", sample_count, (sample_count / STATE_CHANGE_PER_CYCLE), time_diff_ms);
	//Serial.println(Msg);

	return (((float)(sample_count / STATE_CHANGE_PER_CYCLE) * 1000.0f) / (float)(time_diff_ms)); 
}