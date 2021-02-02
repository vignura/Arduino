#include <Relay.h>

#define VERSION			"0.1"
#define RELAY_PIN		1
#define NC_PIN			2  /* green */
#define NO_PIN			3  /* red */

#define RELAY_DEFAULT_STATE		RELAY_OFF

#define ENABLE_DEBUG
#define MSG_BUFFER_SIZE		64
char Msg[MSG_BUFFER_SIZE];

volatile int NCPinIntrCnt;
volatile int NOPinIntrCnt;

Relay Rly(RELAY_PIN, false);

void setup() {
  
	pinMode(NC_PIN, INPUT);
	pinMode(NO_PIN, INPUT);
	Rly.setState(RELAY_DEFAULT_STATE);
	
	Serial.begin(9600);
	sprintf(Msg, "Industrial Swtich Controller %s", VERSION);
	Serial.println(Msg);

	attachInterrupt(digitalPinToInterrupt(NC_PIN), NC_isr, CHANGE);
  attachInterrupt(digitalPinToInterrupt(NO_PIN), NO_isr, CHANGE);
}

void loop() {

	/* first check NC Pin */
	if(NCPinIntrCnt > 0)
	{
		Rly.setState(RELAY_ON);
		
		#ifdef ENABLE_DEBUG
			sprintf(Msg, "Turning Relay ON");
			Serial.println(Msg);			
		#endif

		// reset the count
		NCPinIntrCnt = 0;
	}

	/* then check NO Pin */
	if(NOPinIntrCnt > 0)
	{
		Rly.setState(RELAY_OFF);
		
		#ifdef ENABLE_DEBUG
			sprintf(Msg, "Turning Relay OFF");
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