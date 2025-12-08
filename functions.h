#ifndef MJ_PILL_DISPENSER_FUNCTIONS_H
#define MJ_PILL_DISPENSER_FUNCTIONS_H

// Header file for pill dispenser project
// Contains pin definitions, constants, and function prototypes

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
#define BRIGHTNESS 50
#define MINIMUM_BRIGHTNESS 0
#define LED_1 22
#define LED_2 21
#define LED_3 20

// General macros
#define SPIN_THRICE 3
#define DEBOUNCE_HIGH 100
#define DEBOUNCE_MEDIUM 50
#define DEBOUNCE_LOW 25
#define SHORT_BLINK_DELAY 200
#define LONG_BLINK_DELAY 500
#define ADC_1 1
#define DISPENSE_DELAY 2000
#define STEP_DELAY 2
#define TIMEOUT 1000
#define DISPENSE_SLEEP 200



//
// Function prototypes below
//
// *** CALIBRATE STUFF ETC. ***
void run_one_step(void);

void gpio_activate(const int *values);

void run_system(int times, int *steps_per_rev);

int calibrate_system(int *steps_per_rev);

void piezo_callback(uint gpio, uint32_t events);

bool pill_dispensed(void);

void align_system(int alignment_steps);


// *** LED INIT ETC. ***
void pwm_led(uint pin);

void slice_and_channel_helper(int pin, int level);

void set_brightness(int level);

bool button_pressed(int pin);

void idle_blink(void);

void blink_5_times(void);

#endif //MJ_PILL_DISPENSER_FUNCTIONS_H