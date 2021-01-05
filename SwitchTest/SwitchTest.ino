#include "Switch.h"

#define SWITCH_1_PIN		2
#define SWITCH_2_PIN		3
#define SWITCH_3_PIN		4
#define MAX_SWITCHES		3

#define LOOP_DELAY_MS				200
#define SWITCH_DEBOUNCE_TIME_MS		50
uint8_t switch_pin[MAX_SWITCHES] = {SWITCH_1_PIN, SWITCH_2_PIN, SWITCH_3_PIN};
Switch* swtch[MAX_SWITCHES];
char Msg[64];


void setup()
{
	Serial.begin(115200);
	Switch_init();
	sprintf(Msg, "Switch Test");
	Serial.println(Msg);
}

void Switch_init()
{
	uint8_t i = 0;
	
	for(i = 0; i < MAX_SWITCHES; i++)
	{
		swtch[i] = new Switch(switch_pin[i], SWITCH_OFF, SWITCH_DEBOUNCE_TIME_MS);
	}
}

void loop()
{
	uint8_t i = 0;
	uint8_t state = 0;
	
	/* simulating other tasks with delay */
	// delay(LOOP_DELAY_MS);

	for(i = 0; i < MAX_SWITCHES; i++)
	{
		swtch[i]->SwitchTask();
		if(swtch[i]->isStateChanged())
		{
			state = swtch[i]->getState();
			// (state == SWITCH_OFF) ? sprintf(Msg, "Switch %d is OFF", i) : sprintf(Msg, "Switch %d is ON", i);
			sprintf(Msg, "Switch %d is %s", (i+1), (state == SWITCH_OFF)? "OFF": "ON");
			Serial.println(Msg);		
		}
	}
}