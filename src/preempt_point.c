#include <stdio.h>
#include "preempt_point.h"

#define MAX_TRACE_LEN 256

static int trace_ids[MAX_TRACE_LEN];
static int trace_len = 0;
static int sequence_number = 0;

/*
 * Clears the current trace before running the program on a new input.
 */
void reset_trace(void) {
    trace_len = 0;
    sequence_number = 0;
}

/*
 * Records one preemption point.
 *
 * For week 1, the analyzer only needs the ID sequence.
 * The file, line, and function values are still accepted here so that
 * the PREEMPT_POINT macro records source context automatically.
 */
void log_preempt_point(
    int id,
    const char *file,
    int line,
    const char *func
) {
    if (trace_len < MAX_TRACE_LEN) {
        trace_ids[trace_len] = id;
        trace_len++;
    }

    sequence_number++;

    /*
     * Uncomment this if you want detailed debugging output:
     *
     * fprintf(
     *     stderr,
     *     "seq=%d,id=%d,file=%s,line=%d,function=%s\n",
     *     sequence_number,
     *     id,
     *     file,
     *     line,
     *     func
     * );
     */

    (void)file;
    (void)line;
    (void)func;
}

/*
 * Prints one CSV row:
 *
 * example,input,trace
 *
 * Example:
 * thermostat_iot,7,1-2-6-8-9
 */
void print_trace_csv(
    const char *example_name,
    int input_value
) {
    printf("%s,%d,", example_name, input_value);

    for (int i = 0; i < trace_len; i++) {
        printf("%d", trace_ids[i]);

        if (i + 1 < trace_len) {
            printf("-");
        }
    }

    printf("\n");
}
