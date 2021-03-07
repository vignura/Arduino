#include <Relay.h>

#define VERSION			"0.1"
#define RELAY_PIN		1
#define NC_PIN			2  /* red */
#define NO_PIN			3  /* green	 */

#define RELAY_DEFAULT_STATE			RELAY_OFF
#define NO_SWITCH_DEBOUNCE_COUNT	0
#define NC_SWITCH_DEBOUNCE_COUNT	20

#define ENABLE_DEBUG
#define MSG_BUFFER_SIZE		64
char Msg[MSG_BUFFER_SIZE];

volatile int NCPinIntrCnt;
volatile int NOPinIntrCnt;

Relay Rly(RELAY_PIN, false);

void setup() {
  
  	/* always connect the NC pin to switch and switch to 5V 
  	   and connect NO pin to switch to and switch to ground */
	pinMode(NC_PIN, INPUT);
	pinMode(NO_PIN, INPUT_PULLUP);
	Rly.setState(RELAY_DEFAULT_STATE);
	
	Serial.begin(9600);
	sprintf(Msg, "Industrial Swtich Controller %s", VERSION);
	Serial.println(Msg);

	attachInterrupt(digitalPinToInterrupt(NC_PIN), NC_isr, CHANGE);
	attachInterrupt(digitalPinToInterrupt(NO_PIN), NO_isr, CHANGE);
}

void loop() {

	/* first check NC Pin */
	if(NCPinIntrCnt > NC_SWITCH_DEBOUNCE_COUNT)
	{
		Rly.setState(RELAY_OFF);
		
		#ifdef ENABLE_DEBUG
			sprintf(Msg, "Turning Relay OFF [%d]", NCPinIntrCnt);
			Serial.println(Msg);			
		#endif

		// reset the count
		NCPinIntrCnt = 0;
	}

	/* then check NO Pin */
	if(NOPinIntrCnt > NO_SWITCH_DEBOUNCE_COUNT)
	{
		Rly.setState(RELAY_ON);
		
		#ifdef ENABLE_DEBUG
			sprintf(Msg, "Turning Relay ON [%d]", NOPinIntrCnt);
			Serial.println(Msg);			
		#endif	

		// reset the count
		NOPinIntrCnt = 0;
	}

	/* 50 ms delay for stability */
	delay(50);
}

void NC_isr(){
	NCPinIntrCnt++;
}

void NO_isr(){
	NOPinIntrCnt++;
}