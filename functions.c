#include "functions.h"
#include <stdbool.h>
#include <string.h>
#include "pico/stdlib.h"

// all the includes here ^

// MISSING PIEZO SENSOR SHIT

//
// Functions below
//


// *** CALIBRATE STUFF ETC. ***
void check_status(int steps, bool calib_status) {
}

void run_once(void) {

}

void gpio_activate(const int *values) {

}

void run_system(int times, int *steps_per_rev, bool *calib_status) {

}

void calibrate_system(int *steps_per_rev, bool *calib_status) {

}


// *** LED INIT ETC. BELOW ***
void pwm_led(uint pin) {

}

void slice_and_channel_helper(int pin, int level) {

}

void set_brightness(int level) {

}

bool button_pressed(int pin) {

}

int idle_blink(void) {

}

int blink_5_times(void) {

}

