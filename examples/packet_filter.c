#include <stdio.h>
#include <stdlib.h>
#include "preempt_point.h"

/*
 * packet_filter.c
 *
 * Toy network/IoT-style example.
 *
 * The hidden input is a small integer from 0 to 31.
 * The program treats this input like simplified packet metadata.
 *
 * An observer who sees only the PREEMPT_POINT IDs may learn:
 * - packet type
 * - approximate payload length
 * - partial information from the number of loop iterations
 */

static void packet_filter_logic(int secret) {
    PREEMPT_POINT(10);  // Start packet processing

    int packet_type = secret % 4;
    int payload_length = secret % 16;

    /*
     * Packet type branch.
     */
    if (packet_type == 0) {
        PREEMPT_POINT(11);  // Control packet
    } else if (packet_type == 1) {
        PREEMPT_POINT(12);  // Data packet
    } else if (packet_type == 2) {
        PREEMPT_POINT(13);  // Sensor packet
    } else {
        PREEMPT_POINT(14);  // Unknown packet
    }

    /*
     * Payload length branch.
     */
    if (payload_length < 4) {
        PREEMPT_POINT(15);  // Short payload
    } else if (payload_length < 10) {
        PREEMPT_POINT(16);  // Medium payload
    } else {
        PREEMPT_POINT(17);  // Long payload
    }

    /*
     * Loop count depends on payload length.
     * Repeated point 18 can leak partial information about the payload size.
     */
    for (int i = 0; i < payload_length / 5; i++) {
        PREEMPT_POINT(18);  // Payload scan loop
    }

    PREEMPT_POINT(19);  // End packet processing
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <secret>\n", argv[0]);
        return 1;
    }

    int secret = atoi(argv[1]);

    reset_trace();
    packet_filter_logic(secret);
    print_trace_csv("packet_filter", secret);

    return 0;
}
