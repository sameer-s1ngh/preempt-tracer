#pragma once

#include <stdio.h>

void trace_begin(FILE *stream, const char *example, const char *input_label);
void trace_end(void);
void log_preempt_point(int id, const char *file, int line, const char *function);

#define PREEMPT_POINT(ID) \
    log_preempt_point((ID), __FILE__, __LINE__, __func__)
