#include <stdio.h>
#include <stdbool.h>
#include "functions.h"
#include "pico/time.h"
#include "pico/stdio.h"
#include "hardware/gpio.h"



// Main control loop for the pill dispenser
// Handles state transitions: welcome -> calibration -> ready -> dispensing
int main(void) {
    stdio_init_all();

    int state = 1;
    int steps_per_rev = 0;
    int dispenses_done = 0;
    bool printed = false; // To avoid flooding the terminal with prints...

    absolute_time_t last_dispense_time = get_absolute_time();// Capture current time in ms during boot used later to dispense every 30s

    // Configure button and sensor inputs, and motor coil outputs below
    // Setup middle button
    gpio_init(SW_1);
    gpio_set_dir(SW_1, GPIO_IN);
    gpio_pull_up(SW_1);
    sleep_ms(DEBOUNCE_MEDIUM);

    // Setup GPIO pins
    gpio_init(OPTO_FORK);
    gpio_set_dir(OPTO_FORK, GPIO_IN);
    gpio_pull_up(OPTO_FORK);

    // Setup PIEZO
    gpio_init(PIEZO_SENSOR);
    gpio_set_dir(PIEZO_SENSOR, GPIO_IN);
    gpio_pull_up(PIEZO_SENSOR);

    gpio_init(COIL_A);
    gpio_set_dir(COIL_A, GPIO_OUT);

    gpio_init(COIL_B);
    gpio_set_dir(COIL_B, GPIO_OUT);

    gpio_init(COIL_C);
    gpio_set_dir(COIL_C, GPIO_OUT);

    gpio_init(COIL_D);
    gpio_set_dir(COIL_D, GPIO_OUT);

    // Setup LEDs
    pwm_led(LED_1);
    pwm_led(LED_2);
    pwm_led(LED_3);

    // Welcome message
    printf("--| Welcome to the pill dispenser |--\n");

    while (true) {
        // Switch case statemachine for the main loop
        switch (state) {
            // Idle state: blink LEDs until user presses the button
            case 1:
                // Print instructions and check flag so it doesn't flood the terminal
                if (!printed) {
                    printf("\nOnly place pills after calibration! Press the middle button (SW_1) to first calibrate the system.\n");
                    printed = true;
                }
                idle_blink(); // Idle blinking (non blocking)
                if (!button_pressed(SW_1)) {
                    state = 2;
                    printed = false; // Reset flag for next state
                    while (!button_pressed(SW_1))
                        sleep_ms(DEBOUNCE_MEDIUM);
                }
                break;

            case 2: // Calibration: rotate wheel until sensor aligns with drop tube
                if (!printed) {
                    printf("System is being calibrated, please wait...\n");
                    printed = true;
                }

                calibrate_system(&steps_per_rev); // calibrate the system and go to state 3
                align_system(steps_per_rev);

                state = 3;
                printed = false;
                break;
            case 3:
                if (!printed) {
                    printf("System has been calibrated, add your desired pills to the dispenser compartments.\n");
                    printf("Press the middle button (SW_1) to start dispensing pills every 30 seconds.\n");
                    printed = true;
                }
                set_brightness(BRIGHTNESS); // Turn LEDs on
                if (!button_pressed(SW_1)) {
                    dispenses_done = 0;
                    first_dispense(&dispenses_done, &steps_per_rev);
                    last_dispense_time = get_absolute_time(); // Start timer from button press
                    state = 4;
                    printed = false;
                    while (!button_pressed(SW_1))
                        sleep_ms(DEBOUNCE_MEDIUM);
                }
                break;
            case 4:
                // Dispensing loop: every 30s rotate wheel and check piezo sensor
                // If no pill detected, blink LEDs 5 times as a warning
                if (!printed) {
                    printf("Dispensing pills every 30 seconds...\n");
                    printed = true;
                }
                if (absolute_time_diff_us(last_dispense_time, get_absolute_time()) >= DISPENSE_DELAY * 1000) {
                    // = 30 seconds delay
                    run_system(1, &steps_per_rev); // Run once
                    if (!pill_dispensed()) {
                        // If no pill was detected give a warning and blink LEDs 5 times
                        printf("No pill drop was detected, ensure you have loaded the dispenser.\n");
                        blink_5_times();
                    } else {
                        printf("A pill was successfully dispensed.\n");
                    }
                    dispenses_done++;
                    last_dispense_time = get_absolute_time();
                }
                // Go back to beginning after dispensing all possible pill slots
                if (dispenses_done >= 7) { // 7 = Max available slots
                    printf("All possible pills have been dispensed, restarting the cycle...\n");
                    state = 1;
                    printed = false;
                }
                break;
        }
    }
}