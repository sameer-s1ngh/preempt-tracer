# Preempt Tracer — Week 3

Week 3 builds a host-side C tracing library and instruments two toy programs. The tracer records when execution reaches a selected observation point. It does **not** perform a real context switch and does **not** claim that a preemption actually happened.

## Build and run

```bash
make clean
make run
```

This builds both C examples and writes the trace output to:

```text
results/week3_traces.csv
```

## Trace format

The trace file uses event-oriented CSV:

```text
example,input,sequence,point_id,file,line,function
```

Each row represents one observed preemption point. The sequence number resets to `0` for each new input run.

Example:

```text
thermostat,17,0,1,examples/thermostat_iot.c,35,main
```

## What `PREEMPT_POINT(id)` means

`PREEMPT_POINT(id)` means that execution reached a selected observation point with a stable point ID.

It does **not** mean:

* a real scheduler interrupted the program
* a context switch happened
* the code is running inside an RTOS
* a real preemption occurred

It is a host-side trace marker used to study which program locations are observable.

## Point ID table

### Thermostat example

| Point ID | Meaning            |
| -------: | ------------------ |
|        1 | Entry point        |
|        2 | Heat branch        |
|        3 | Comfortable branch |
|        4 | Cool branch        |
|        5 | Alert branch       |
|        6 | Exit point         |

### Packet-filter example

| Point ID | Meaning                        |
| -------: | ------------------------------ |
|       10 | Entry point                    |
|       11 | Status-packet branch           |
|       12 | Data-packet branch             |
|       13 | Data payload chunk observation |
|       14 | Rejected-packet branch         |
|       15 | Exit point                     |

## Tested finite input domains

### Thermostat

The thermostat example tests 32 synthetic temperatures:

```text
-10, -8, -6, ..., 52
```

This finite input range covers heat, comfortable, cool, low-alert, and high-alert paths.

### Packet filter

The packet-filter example tests 24 synthetic packet inputs:

```text
packet_type in {0, 1, 2}
payload_length in {0, 4, 8, 16, 24, 32, 48, 64}
```

This covers the status, data, and rejected packet branches. For data packets, the payload length controls how many payload-chunk observations occur.

## Concrete examples of different traces

Thermostat input `17` reaches entry, heat branch, and exit:

```text
1, 2, 6
```

Thermostat input `28` reaches entry, cool branch, and exit:

```text
1, 4, 6
```

So the point IDs distinguish heat behavior from cool behavior.

Packet input `0:24` reaches entry, status-packet branch, and exit:

```text
10, 11, 15
```

Packet input `1:24` reaches entry, data-packet branch, three payload-chunk observations, and exit:

```text
10, 12, 13, 13, 13, 15
```

So the point IDs distinguish packet type, and repeated point `13` reveals a selected loop observation.

## Source information recorded

Each event records:

* source file
* source line
* function name

This is useful for debugging, confirming where each point ID came from, and later interpreting trace differences.

## Determinism assumptions

The examples are deterministic because they use only command-line arguments and fixed branch logic. There is no randomness, real sensor input, networking, timing source, or concurrency in the host-side examples. The same input should produce the same trace on repeated runs.

## Week 3 scope

This week focuses on reliable raw trace collection. Grouping inputs into equivalence classes and calculating information leakage are future analysis tasks.
