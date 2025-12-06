#include "functions.h"
#include <stdbool.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

// all the includes here ^


//
// Helper functions for motor control, calibration, LED handling, and sensor checks below
//

// *** CALIBRATE STUFF ETC. ***
void run_one_step(void) {
    // Move one step -- using half stepping
    const int sequence[8][4] = {
        // 8 x 4 array for coils
        {1, 0, 0, 0},
        {1, 1, 0, 0},
        {0, 1, 0, 0},
        {0, 1, 1, 0},
        {0, 0, 1, 0},
        {0, 0, 1, 1},
        {0, 0, 0, 1},
        {1, 0, 0, 1}
    };

    static int step = 0; // Remember current step across calls
    gpio_activate(sequence[step]); // Energize the coils in a sequence
    step++;
    if (step == 8) {
        // Max 8 steps per sequence
        step = 0; // Go back to 0 after 8
    }
    sleep_ms(STEP_DELAY); // Delay between steps
}

void gpio_activate(const int *values) {
    gpio_put(COIL_A, values[0]);
    gpio_put(COIL_B, values[1]);
    gpio_put(COIL_C, values[2]);
    gpio_put(COIL_D, values[3]);
}

void run_system(int times, int *steps_per_rev) {
    int total_steps;

    total_steps = *steps_per_rev / 8 * times; // 8 = steps per sequence
    printf("Running %d x 1/8 revolution = %d steps ... \n", times, total_steps);

    for (int i = 0; i < total_steps; i++) {
        run_one_step(); // Turn one step
    }
}

// Measure steps per revolution by detecting falling edges from opto sensor
// Average over 3 spins for accuracy
void calibrate_system(int *steps_per_rev) {
    int total_steps = 0;

    bool old = gpio_get(OPTO_FORK);
    bool new;

    // Wait for first falling edge
    while (true) {
        run_one_step();
        new = gpio_get(OPTO_FORK);

        if (old == 1 && new == 0) {
            break;
        }
        old = new;
    }

    // Count steps until next falling edge
    for (int i = 0; i < SPIN_THRICE; i++) {
        int step_counter = 0;
        old = gpio_get(OPTO_FORK);

        while (true) {
            run_one_step();
            step_counter++;
            new = gpio_get(OPTO_FORK);

            if (old == 1 && new == 0) {
                break;
            }
            old = new;
        }
        total_steps += step_counter;
        printf("Measurement %d: %d steps \n", i + 1, step_counter);
    }

    // Average the measurements
    *steps_per_rev = total_steps / SPIN_THRICE;

    printf("Calibration is done! Steps per revolution = %d\n", *steps_per_rev);
}

// Read piezo sensor via ADC; threshold determines if pill drop was detected
bool pill_dispensed(void) {
    uint16_t result = adc_read(); // Read piezo value
    if (result > PILL_THRESHOLD) {
        return false; // Pill not detected
    }
    return true; // Pill detected
}

// After moving approximate steps, fine-tune until sensor edge is detected
void align_system(void) {
    for (int i = 0; i < 180; i++) {
        run_one_step();
    }
}

// *** LED INIT ETC. BELOW ***
void pwm_led(uint pin) {
    uint slice = pwm_gpio_to_slice_num(pin); // Get slice number
    uint channel = pwm_gpio_to_channel(pin); // Get channel number
    pwm_set_enabled(slice, false); // Disable PWM
    pwm_config config = pwm_get_default_config(); // Default settings for PWM
    pwm_config_set_clkdiv_int(&config, DIVIDER); // Set divider for PWM
    pwm_config_set_wrap(&config, WRAP); // Set wrap (TOP)
    pwm_init(slice, &config, false); // Call init with the slice and config and set start to false
    pwm_set_chan_level(slice, channel, MINIMUM_BRIGHTNESS); // Adjust the LED brightness
    gpio_set_function(pin, GPIO_FUNC_PWM); // Select PWM mode for your pin
    pwm_set_enabled(slice, true); // Start PWM
}

void slice_and_channel_helper(int pin, int level) {
    uint slice = pwm_gpio_to_slice_num(pin); // Get slice number
    uint channel = pwm_gpio_to_channel(pin); // Get channel number
    pwm_set_chan_level(slice, channel, level); // Adjust the LED brightness
}

void set_brightness(int level) {
    slice_and_channel_helper(LED_1, level); // LED 1
    slice_and_channel_helper(LED_2, level); // LED 2
    slice_and_channel_helper(LED_3, level); // LED 3
}

bool button_pressed(int pin) {
    return gpio_get(pin); // Check if button pressed
}

// Non-blocking LED blink to avoid system sleeping when button is pressed
void idle_blink(void) {
    // Using static here so it remembers the state of the bool and last_toggle
    static bool led_state = false;
    static uint32_t last_toggle = 0; // Using uint32_t because of the time since boot function

    // Compare to a timer of 500ms and check if it has passed and toggle LED state
    uint32_t now = to_ms_since_boot(get_absolute_time());
    if (now - last_toggle >= LONG_BLINK_DELAY) {
        set_brightness(led_state ? MINIMUM_BRIGHTNESS : BRIGHTNESS); // Set LED off/on depending on led state flag
        led_state = !led_state;
        last_toggle = now;
    }
}

// This one is blocking the system with the sleeps, but it's okay because we want system to
// show warning and not spin the wheel while it's blinking the LEDs
void blink_5_times(void) {
    for (int i = 0; i < 5; i++) {
        // Blink 5 times when piezo didn't recognize a pill drop
        set_brightness(BRIGHTNESS);
        sleep_ms(SHORT_BLINK_DELAY);
        set_brightness(MINIMUM_BRIGHTNESS);
        sleep_ms(SHORT_BLINK_DELAY);
    }
}
