#include "preempt_point.h"

#include <stdio.h>
#include <stdlib.h>

static FILE *trace_stream = NULL;
static const char *trace_example = NULL;
static const char *trace_input_label = NULL;
static unsigned long next_sequence = 0;
static int trace_active = 0;

static void csv_write_field(FILE *stream, const char *text) {
    int must_quote = 0;

    if (text == NULL) {
        text = "";
    }

    for (const char *p = text; *p != '\0'; p++) {
        if (*p == ',' || *p == '"' || *p == '\n' || *p == '\r') {
            must_quote = 1;
            break;
        }
    }

    if (!must_quote) {
        fputs(text, stream);
        return;
    }

    fputc('"', stream);

    for (const char *p = text; *p != '\0'; p++) {
        if (*p == '"') {
            fputc('"', stream);
        }

        fputc(*p, stream);
    }

    fputc('"', stream);
}

void trace_begin(FILE *stream, const char *example, const char *input_label) {
    if (stream == NULL) {
        fprintf(stderr, "trace_begin: stream must not be NULL\n");
        exit(2);
    }

    trace_stream = stream;
    trace_example = example;
    trace_input_label = input_label;
    next_sequence = 0;
    trace_active = 1;
}

void trace_end(void) {
    if (trace_stream != NULL) {
        fflush(trace_stream);
    }

    trace_stream = NULL;
    trace_example = NULL;
    trace_input_label = NULL;
    next_sequence = 0;
    trace_active = 0;
}

void log_preempt_point(int id, const char *file, int line, const char *function) {
    if (!trace_active || trace_stream == NULL) {
        fprintf(stderr, "log_preempt_point: trace was not started\n");
        exit(2);
    }

    csv_write_field(trace_stream, trace_example);
    fputc(',', trace_stream);

    csv_write_field(trace_stream, trace_input_label);
    fputc(',', trace_stream);

    fprintf(trace_stream, "%lu,%d,", next_sequence, id);

    csv_write_field(trace_stream, file);
    fputc(',', trace_stream);

    fprintf(trace_stream, "%d,", line);

    csv_write_field(trace_stream, function);
    fputc('\n', trace_stream);

    next_sequence++;
}
