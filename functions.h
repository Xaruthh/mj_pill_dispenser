#ifndef MJ_PILL_DISPENSER_FUNCTIONS_H
#define MJ_PILL_DISPENSER_FUNCTIONS_H

#include <stdbool.h>
#include <stdio.h>
#include <pico/types.h>

// Defines below
// Button, coils & sensor macros
#define SW_1 8
#define OPTO_FORK 28
#define PIEZO_SENSOR 27
#define COIL_A 2
#define COIL_B 3
#define COIL_C 6
#define COIL_D 13

// LED macros
#define DIVIDER 125
#define WRAP 999
#define BRIGHTNESS 200
#define MINIMUM_BRIGHTNESS 0
#define LED_1 22
#define LED_2 21
#define LED_3 20

// Debounce macros
#define DEBOUNCE_HIGH 100
#define DEBOUNCE_MEDIUM 50
#define DEBOUNCE_LOW 25

#define SPIN_THRICE 3
#define STEP_DELAY 2
#define LONG_BLINK_DELAY 500
#define SHORT_BLINK_DELAY 150

#define DISPENSE_DELAY 30000

#define CALIB_OFFSET 83

// MISSING PIEZO SENSOR SHIT

//
// Function prototypes below
//
// *** CALIBRATE STUFF ETC. ***
// void check_status(int steps, bool calib_status);

void run_one_step(void);
void gpio_activate(const int *values);

void run_system(int times, int *steps_per_rev);
void calibrate_system(int *steps_per_rev);
void align_system(int steps_per_rev);

void first_dispense(int *dispenses_done, int *steps_per_rev);

bool pill_dispensed(void);

// *** LED INIT ETC. BELOW ***
void pwm_led(uint pin);
void set_brightness(int level);

void slice_and_channel_helper(int pin, int level);

bool button_pressed(int pin);

void idle_blink(void);
void blink_5_times(void);

#endif //MJ_PILL_DISPENSER_FUNCTIONS_H