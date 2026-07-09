#include <stdio.h>
#include <stdlib.h>
#include "preempt_point.h"

/*
 * thermostat_iot.c
 *
 * Toy Arduino/IoT-style example.
 *
 * The hidden input is a small integer from 0 to 31.
 * The program treats this input like a simplified sensor/state value.
 *
 * An observer who sees only the PREEMPT_POINT IDs may learn:
 * - whether the temperature is cold, normal, hot, or alert-level
 * - whether the room is occupied
 * - partial information about the loop count
 */

static void thermostat_logic(int secret) {
    PREEMPT_POINT(1);  // Start thermostat processing

    int temperature = secret % 32;
    int occupied = secret % 2;

    /*
     * Temperature branch.
     */
    if (temperature < 10) {
        PREEMPT_POINT(2);  // Cold range: heat might turn on
    } else if (temperature < 22) {
        PREEMPT_POINT(3);  // Normal range: no major action
    } else if (temperature < 28) {
        PREEMPT_POINT(4);  // Hot range: cooling might turn on
    } else {
        PREEMPT_POINT(5);  // Alert range: temperature is very high
    }

    /*
     * Occupancy branch.
     */
    if (occupied) {
        PREEMPT_POINT(6);  // Room is occupied
    } else {
        PREEMPT_POINT(7);  // Room is empty
    }

    /*
     * Loop count depends on temperature.
     * Repeated point 8 can leak partial information about temperature % 3.
     */
    for (int i = 0; i < temperature % 3; i++) {
        PREEMPT_POINT(8);  // Fan adjustment loop
    }

    PREEMPT_POINT(9);  // End thermostat processing
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <secret>\n", argv[0]);
        return 1;
    }

    int secret = atoi(argv[1]);

    reset_trace();
    thermostat_logic(secret);
    print_trace_csv("thermostat_iot", secret);

    return 0;
}
