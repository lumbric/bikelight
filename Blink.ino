/*
 */

#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/interrupt.h>


// assymetric blink intervall for breathing mode
#define BREATH_IN_MS 1000
#define BREATH_OUT_MS 1800

// assymetric blink intervall for flashing mode
#define FLASH_ON_MS 1000
#define FLASH_OFF_MS 1000

// go sleep in any case after...
#define AUTO_OFF_AFTER_MIN 180

// go to standby if no brake/movment after...
#define AUTO_OFF_NO_MOVE_MIN 10

// duration of light full on after brake in ms
#define BRAKE_LIGHT_DURATION_MS 3000

// dimm light for all modes when no brake, int between 0-255
#define MAX_LIGHT_NO_BRAKE 200

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




void go_sleep() {
    // TODO fade out led here

    analogWrite(LED_PIN, 0);
    delay(1000);  // no idea why if/why necessary

    attachInterrupt(0, interrupt_handler, LOW);
    delay(100);  // no idea why if/why necessary
    
    // Choose our preferred sleep mode:
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);

    // Set sleep enable (SE) bit:
    sleep_enable();

    analogWrite(LED_PIN, 0);
    // Put the device to sleep:
    sleep_mode();

    // Upon waking up, sketch continues from this point.
    sleep_disable();
} 


void interrupt_handler() {
    detachInterrupt(0);
}


// TODO not implemented yet
enum Blinkmodes {
    constant,
    flash,
    breath,
    flash_twice,
};



class Led {
    public:
        Led() :
            blinkmode(breath),
            brake_start_ms(0)
        {
            start_ms = millis();
        };

        void update() {
            unsigned long t = millis();

            if (brake_start_ms) {
                if (brake_start_ms + BRAKE_LIGHT_DURATION_MS > t) {
                    // brake light already on
                    return;
                }
                else {
                    brake_start_ms = 0;
                    start_ms = t - BREATH_IN_MS;
                }
            }
            
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

        void brake() {
            brake_start_ms = millis();
            analogWrite(LED_PIN, 255);
        };

        setBlinkMode(Blinkmodes new_mode) {
            blinkmode = new_mode;
            start_ms = millis();;
        }


    private:
        byte brightness;
        unsigned long start_ms;       // start of period
        unsigned long brake_start_ms; // start of brake light
        Blinkmodes blinkmode;

};

Led led;

unsigned long alive_since_ms = 0; 
unsigned long last_movement_ms = 0; 


void setup() {
    pinMode(LED_PIN, OUTPUT);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(BRAKE_PIN, INPUT_PULLUP);

    last_movement_ms = alive_since_ms = millis();
}


void loop() {
    // Note: if this loop gets too slow, we need interrupts for buttons

    // necessary for button debouncing
    bool just_woke_up = millis() - alive_since_ms < 1000;

    led.update();

    if(!just_woke_up && digitalRead(BRAKE_PIN) == LOW) {
        last_movement_ms = millis();
        led.brake();
    }

    if (!just_woke_up && (digitalRead(BUTTON_PIN) == LOW) ||
            (millis() - alive_since_ms > AUTO_OFF_AFTER_MIN * 60UL * 1000UL) ||
            (millis() - last_movement_ms > AUTO_OFF_NO_MOVE_MIN * 60UL * 1000UL)) {
        go_sleep();
        alive_since_ms = millis();
        // TODO make led fade in
    }
}
