#include "preempt_point.h"

#include <stdio.h>
#include <stdlib.h>

/*
 * Point IDs for thermostat:
 *
 * 1 = entry
 * 2 = heat branch
 * 3 = comfortable branch
 * 4 = cool branch
 * 5 = alert branch
 * 6 = exit
 */

static int parse_temperature(const char *text, int *temperature) {
    char *end = NULL;
    long value = strtol(text, &end, 10);

    if (text[0] == '\0' || *end != '\0' || value < -40 || value > 80) {
        return 0;
    }

    *temperature = (int)value;
    return 1;
}

int main(int argc, char **argv) {
    int temperature;

    if (argc != 2 || !parse_temperature(argv[1], &temperature)) {
        fprintf(stderr, "usage: %s TEMPERATURE\n", argv[0]);
        return 2;
    }

    trace_begin(stdout, "thermostat", argv[1]);

    PREEMPT_POINT(1);

    if (temperature < 18) {
        PREEMPT_POINT(2);
        fprintf(stderr, "mode=heat\n");
    } else if (temperature > 26) {
        PREEMPT_POINT(4);
        fprintf(stderr, "mode=cool\n");
    } else {
        PREEMPT_POINT(3);
        fprintf(stderr, "mode=comfortable\n");
    }

    if (temperature < 0 || temperature > 40) {
        PREEMPT_POINT(5);
        fprintf(stderr, "alert=1\n");
    }

    PREEMPT_POINT(6);

    trace_end();

    return 0;
}
