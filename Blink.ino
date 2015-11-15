/*
 */

#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/interrupt.h>


// assymetric blink intervall for breathing mode
#define BREATH_IN_US 2000
#define BREATH_OUT_US 2000

// assymetric blink intervall for flashing mode
#define FLASH_ON_US 1000
#define FLASH_OFF_US 1000

// go sleep in any case after...
#define AUTO_OFF_AFTER_HOURS 3

// go to standby if no brake/movment after...
#define AUTO_OFF_NO_MOVE_MIN 15

// duration of light full on after brake in ms
#define BRAKE_LIGHT_DURATION_MS 3000

// dimm light for all modes when no brake, int between 0-255
#define MAX_LIGHT_NO_BRAKE 150

// minimum light when flashing/breathing, int between 0-255
#define MIN_LIGHT_NO_BRAKE 0

#define LED_PIN 0


#define BUTTON_PIN 1


#define BRAKE_PIN 2


/*
 * TODOs
 *  - sleep mode
 *  - modes: static, flashing, breathing
 *  - modes into eeprom?
 *  - auto on, needs second interrupt pin?
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
  set_sleep_mode (SLEEP_MODE_PWR_DOWN); 
  GIMSK = _BV(INT0);      // enable INT0 
  sleep_enable();
  sleep_cpu ();
  sleep_disable();
}

//volatile int value = 0;
//ISR(PCINT0_vect)
//{
//    value += 1;             // Increment volatile variable
//}


void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BRAKE_PIN, INPUT_PULLUP);

  attachInterrupt(0, Int0ISR, LOW);
}

void Int0ISR() { 
  GIMSK = 0;  // disable INT0         
}

void loop() {
  byte light_val;
  // != instead of <= becaus of overflow
  for (light_val = MIN_LIGHT_NO_BRAKE; light_val != MAX_LIGHT_NO_BRAKE; light_val++) {
    analogWrite(LED_PIN, light_val);
    delayMicroseconds(BREATH_IN_US);
    //brake();
    if (digitalRead(BUTTON_PIN) == LOW)
      go_sleep();
  }
  for (light_val = MAX_LIGHT_NO_BRAKE; light_val != MIN_LIGHT_NO_BRAKE; light_val--) {
    analogWrite(LED_PIN, light_val);
    delayMicroseconds(BREATH_OUT_US);
    //brake();
    if (digitalRead(BUTTON_PIN) == LOW)
      go_sleep();
  }
  
}
