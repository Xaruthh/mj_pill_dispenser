#include <stdio.h>
#include <stdbool.h>
#include "functions.h"
#include "pico/time.h"
#include "pico/stdio.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"


int main(void) {
    stdio_init_all();

    uint32_t last_dispense_time = to_ms_since_boot(get_absolute_time()); // Capture current time in ms during boot used later to dispense every 30s
    int state = 1;
    int steps_per_rev = 0;
    int dispenses_done = 0;
    bool calib_status = false;

    // Setup middle button
    gpio_init(SW_1);
    gpio_set_dir(SW_1, GPIO_IN);

    // Setup GPIO pins
    gpio_init(OPTO_FORK);
    gpio_set_dir(OPTO_FORK, GPIO_IN);
    gpio_pull_up(OPTO_FORK);

    gpio_init(COIL_A);
    gpio_set_dir(COIL_A, GPIO_OUT);

    gpio_init(COIL_B);
    gpio_set_dir(COIL_B, GPIO_OUT);

    gpio_init(COIL_C);
    gpio_set_dir(COIL_C, GPIO_OUT);

    gpio_init(COIL_D);
    gpio_set_dir(COIL_D, GPIO_OUT);

    // Setup adc for piezo
    adc_init();
    adc_gpio_init(PIEZO_SENSOR);
    adc_select_input(ADC_1);

    // Setup LEDs
    pwm_led(LED_1);
    pwm_led(LED_2);
    pwm_led(LED_3);

    while (true) {
        // Switch case for the main loop
        switch (state) {
            case 1:
                printf("-- Welcome to the pill dispenser --\n");
                printf("Press the middle button (SW_1) to first calibrate the system, please.\n");
                idle_blink();
                if (!button_pressed(SW_1)) {
                    state = 2;
                    while (!button_pressed(SW_1))
                        sleep_ms(DEBOUNCE_MEDIUM);
                }
                break;
            case 2:
                printf("System is being calibrated, please hold on...\n");
                calibrate_system(&steps_per_rev, &calib_status);
                state = 3;
                break;
            case 3:
                printf("System has been calibrated, press middle button (SW_1) to start dispensing pills every 30 seconds.\n");
                set_brightness(BRIGHTNESS);
                if (!button_pressed(SW_1)) {
                    last_dispense_time = to_ms_since_boot(get_absolute_time());
                    dispenses_done = 0;
                    state = 4;
                    while (!button_pressed(SW_1))
                        sleep_ms(DEBOUNCE_MEDIUM);
                }
                break;
            case 4:
                if (absolute_time_diff_us(last_dispense_time,get_absolute_time()) >= DISPENSE_DELAY * 1000) { // = 30 seconds delay
                    run_system(1, &steps_per_rev, &calib_status);
                    if (!pill_dispensed()) {
                        blink_5_times();
                        printf("No pill drop was detected, ensure you have loaded the dispenser.\n");
                    }
                    printf("A pill was successfully dispensed\n");
                    dispenses_done++;
                    last_dispense_time = get_absolute_time();
                }
                if (dispenses_done >= 7) {
                    printf("All possible pills have been dispensed, restarting the cycle...\n");
                    state = 1;
                }
                break;
        }
    }
}
