#include <stdio.h>
#include <stdbool.h>
#include "functions.h"


int main(void) {
    int state = 1;

    while (true) {
        // Switch case for the main loop
        switch (state) {
            case 1:
                // start state blink led wait for button press.
                state = 2;
                break;
            case 2:
                // button is pressed calibrate system
                state = 3;
                break;
            case 3:
                // after calib wait for button press and keep led on
                // start dispensing pills every 30 sec
                // check for piezo sensor
                // if no pill dispensed -> blink led 5 times
                // if wheel turned 7 times restart the cycle
                state = 1;
                break;
        }
    }
}
