#include "preempt_point.h"

#include <stdio.h>
#include <stdlib.h>

/*
 * Point IDs for packet_filter:
 *
 * 10 = entry
 * 11 = status-packet branch
 * 12 = data-packet branch
 * 13 = data payload chunk observation
 * 14 = rejected-packet branch
 * 15 = exit
 */

static int parse_bounded(const char *text, int maximum, int *result) {
    char *end = NULL;
    long value = strtol(text, &end, 10);

    if (text[0] == '\0' || *end != '\0' || value < 0 || value > maximum) {
        return 0;
    }

    *result = (int)value;
    return 1;
}

int main(int argc, char **argv) {
    int packet_type;
    int payload_length;
    int offset;
    char input_label[32];

    if (
        argc != 3
        || !parse_bounded(argv[1], 2, &packet_type)
        || !parse_bounded(argv[2], 64, &payload_length)
    ) {
        fprintf(stderr, "usage: %s TYPE PAYLOAD_LENGTH\n", argv[0]);
        return 2;
    }

    snprintf(input_label, sizeof(input_label), "%d:%d", packet_type, payload_length);

    trace_begin(stdout, "packet_filter", input_label);

    PREEMPT_POINT(10);

    if (packet_type == 0) {
        PREEMPT_POINT(11);
        fprintf(stderr, "packet=status\n");
    } else if (packet_type == 1) {
        PREEMPT_POINT(12);
        fprintf(stderr, "packet=data\n");

        for (offset = 0; offset < payload_length; offset += 8) {
            PREEMPT_POINT(13);
            fprintf(stderr, "chunk=%d\n", offset / 8);
        }
    } else {
        PREEMPT_POINT(14);
        fprintf(stderr, "packet=rejected\n");
    }

    PREEMPT_POINT(15);

    trace_end();

    return 0;
}
