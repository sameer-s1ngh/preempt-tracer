#ifndef PREEMPT_POINT_H
#define PREEMPT_POINT_H

/*
 * preempt_point.h
 *
 * This header defines PREEMPT_POINT(ID), which records that the program
 * reached a specific allowed preemption point.
 *
 * The macro automatically captures file, line, and function information.
 */

void reset_trace(void);

void log_preempt_point(
    int id,
    const char *file,
    int line,
    const char *func
);

void print_trace_csv(
    const char *example_name,
    int input_value
);

#define PREEMPT_POINT(ID) \
    log_preempt_point((ID), __FILE__, __LINE__, __func__)

#endif
