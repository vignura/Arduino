#define PWM_PIN         9
#define TIMER_COUNT                     5000
#define DUTY_CYCLE_COUNT                125

#define NO_CLK_SRC                      (0)
#define PRESCALR_1                      (_BV(CS10))
#define PRESCALR_8                      (_BV(CS11))
#define PRESCALR_64                     (_BV(CS11) | _BV(CS10))
#define PRESCALR_256                    (_BV(CS12))
#define PRESCALR_1024                   (_BV(CS11) | _BV(CS10))

void setup() {
    pinMode(PWM_PIN, OUTPUT);
    // setup_pwm();
}

void loop() {

    digitalWrite(PWM_PIN, LOW);
    delay(5);
    
    setup_pwm();
    delay(60);
    disable_pwm();
    
    // digitalWrite(PWM_PIN, LOW);
    // delay(10);
    digitalWrite(PWM_PIN, HIGH);
    
    delay(10 * 1000);

    // gen_square_wave(100, PWM_PIN);
}

void gen_square_wave(float freq, uint8_t pin)
{
    float period_ms = (1000.0f / freq);

    while(1)
    {
        digitalWrite(pin, HIGH);
        delay(uint32_t(period_ms / 2.0f));
        digitalWrite(pin, LOW);
        delay(uint32_t(period_ms / 2.0f));
    }
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