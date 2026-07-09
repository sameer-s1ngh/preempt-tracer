# Preempt Tracer

This project explores empirical program-counter leakage from preemption-point traces.

In a limited-preemption system, the scheduler may only preempt at selected program locations. This project asks what an observer could learn if they saw the sequence of allowed preemption-point IDs reached by a program.

## Files

```text
preempt-tracer/
  include/preempt_point.h
  src/preempt_point.c
  examples/thermostat_iot.c
  examples/packet_filter.c
  scripts/run_all.py
  scripts/analyze_traces.py
  results/
  README.md
  Makefile
```

## How to build and run

From the project folder:

```bash
make run
```

This command:

1. Compiles the two C examples.
2. Runs each example over inputs 0 through 31.
3. Writes the traces to `results/traces.csv`.
4. Groups inputs by identical trace.
5. Prints the number of distinct traces and the `log2(# distinct traces)` leakage bound.

## Preemption point macro

The C macro is:

```c
#define PREEMPT_POINT(ID) \
    log_preempt_point((ID), __FILE__, __LINE__, __func__)
```

Each call records that the program reached a specific preemption point ID. The macro also captures file, line, and function information automatically.

## Example 1: thermostat_iot

The thermostat example treats the hidden input as a simplified sensor/state value.

The input affects:

- temperature range
- occupancy
- fan adjustment loop count

For example, if an observer sees point `2`, the observer can infer that the temperature was in the cold range. If the observer sees point `6`, the observer can infer that the room was occupied. If point `8` repeats, the observer can infer partial information about `temperature % 3`.

The observer may not learn the exact input, but they can narrow the input down to a smaller equivalence class.

## Example 2: packet_filter

The packet filter example treats the hidden input as simplified packet metadata.

The input affects:

- packet type
- payload length category
- payload scan loop count

For example, if an observer sees point `11`, the observer can infer that the packet was a control packet. If the observer sees point `17`, the observer can infer that the payload was long. Repeated point `18` leaks partial information about the payload length.

## Equivalence classes

An equivalence class is a group of inputs that produce the same trace.

Example:

```text
trace 1,2,7,9 -> inputs {0,6}
```

This means inputs `0` and `6` produced the same observable sequence of preemption-point IDs.

## Leakage bound

The analyzer prints:

```text
upper bound = log2(# distinct traces)
```

If there are 4 distinct traces, then the upper bound is:

```text
log2(4) = 2 bits
```

This does not always mean the observer learns exactly 2 bits. It means that the trace can distinguish among 4 possible trace outcomes, so the maximum possible leakage is 2 bits.
