/*
 */

#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/interrupt.h>


// assymetric blink intervall for breathing mode
#define BREATH_IN_MS 4800
#define BREATH_OUT_MS 2000

// assymetric blink intervall for flashing mode
#define FLASH_ON_US 1000
#define FLASH_OFF_US 1000

// go sleep in any case after...
#define AUTO_OFF_AFTER_MIN 180

// go to standby if no brake/movment after...
#define AUTO_OFF_NO_MOVE_MIN 10

// duration of light full on after brake in ms
#define BRAKE_LIGHT_DURATION_MS 3000

// dimm light for all modes when no brake, int between 0-255
#define MAX_LIGHT_NO_BRAKE 255

// minimum light when flashing/breathing, int between 0-255
#define MIN_LIGHT_NO_BRAKE 30

// PINs
#define LED_PIN 0
#define BRAKE_PIN 1
// this is the interrupt pin to wake up...
#define BUTTON_PIN 2


/*
 * TODOs
 *  - sleep mode
 *  - modes: static, flashing, breathing
 *  - modes into eeprom?
 *  - auto on, needs second interrupt pin?
 *  - hold button long for on/off
 *  - config
 *    - brightness
 *    - blink mode
 *    - timeouts?
 */

byte light_mode = 0;


bool brake() {
    bool brake_on = false;
    while (digitalRead(BRAKE_PIN) == LOW) {
        analogWrite(LED_PIN, 255);
        delay(BRAKE_LIGHT_DURATION_MS);
        brake_on = true;
    }
    return brake_on;
}


void run_light() {
    switch (light_mode) {
    case 0:
        break;
    }
}


void switch_mode(int mode) {
    light_mode = mode;
}


void go_sleep() {
    // TODO fade out led here
    analogWrite(LED_PIN, 0);
    delay(2000);
    analogWrite(LED_PIN, 255);
    delay(100);
    analogWrite(LED_PIN, 0);

    GIMSK |= _BV(PCIE);                     // Enable Pin Change Interrupts
    PCMSK |= _BV(PCINT0);                   // Use PB3 as interrupt pin
    ADCSRA &= ~_BV(ADEN);                   // ADC off
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // replaces above statement

    sleep_enable();                         // Sets the Sleep Enable bit in the MCUCR Register (SE BIT)
    sei();                                  // Enable interrupts
    sleep_cpu();                            // sleep

    cli();                                  // Disable interrupts
    PCMSK &= ~_BV(PCINT0);                  // Turn off PB3 as interrupt pin
    sleep_disable();                        // Clear SE bit
    ADCSRA |= _BV(ADEN);                    // ADC on

    sei();                                  // Enable interrupts
} 


ISR(PCINT0_vect) {
    // This is called when the interrupt occurs, but I don't need to do anything in it
}


class Led {
    public:
        Led() {};
        void update() {
            unsigned long t = millis();
            
            if (t > start_ms + BREATH_IN_MS + BREATH_OUT_MS) {
                // period over, new start
                start_ms = t;
            }

            if (t < start_ms + BREATH_IN_MS) { 
                brightness = (double) (MAX_LIGHT_NO_BRAKE - MIN_LIGHT_NO_BRAKE)
                    * (t - start_ms) / BREATH_IN_MS + MIN_LIGHT_NO_BRAKE; 
            } else {
                // FIXME do not use double, use int or so - can this overflow?
                brightness = (double) (MIN_LIGHT_NO_BRAKE - MAX_LIGHT_NO_BRAKE)
                    * (t - BREATH_IN_MS - start_ms) / BREATH_OUT_MS 
                    + MAX_LIGHT_NO_BRAKE; 
            }

            analogWrite(LED_PIN, brightness);
        };
    private:
        byte brightness;
        unsigned long start_ms;  // start of period
};


Led led;

void setup() {
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(BRAKE_PIN, INPUT_PULLUP);

    // attachInterrupt(0, pin2_isr, LOW);
}


void loop() {
    // bool just_woke_up = millis() < 1000;
    led.update();
    brake();

    if (digitalRead(BUTTON_PIN) == LOW)
        go_sleep();

    // auto off
    if (millis() > AUTO_OFF_AFTER_MIN * 60UL * 1000UL)
        go_sleep();
}
