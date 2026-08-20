# Preempt Tracer — Week 4

This project studies how much information can leak from observable preemption-point traces in small C programs. Week 3 produced raw event traces from a host-side C harness. Week 4 extends that pipeline by reconstructing ordered traces, applying observation policies, grouping inputs into equivalence classes, and measuring leakage.

The main question is:

> How does changing which preemption points are visible change what an observer can learn about the hidden input?

## Build and run

Run:

```bash
make clean
make week4
```

This builds the C examples, runs the finite input sweep, creates a fresh raw trace file, and runs the Week 4 analyzer.

Generated files:

```text
results/week3_traces.csv
results/week4_leakage_comparison.csv
```

## Pipeline

```text
C harness
-> raw event file
-> Python loader/adapter
-> ordered point-ID trace
-> observation policy filter
-> equivalence classes
-> leakage metrics
```

The raw Week 3 event file is left unchanged. Week 4 derives filtered traces and summary measurements from that raw file.

## Raw trace format

The Week 3 harness writes event-oriented CSV:

```text
example,input,sequence,point_id,file,line,function
```

Each row is one observed `PREEMPT_POINT(id)` event.

Example:

```text
thermostat,17,0,1,examples/thermostat_iot.c,35,main
thermostat,17,1,2,examples/thermostat_iot.c,40,main
thermostat,17,2,6,examples/thermostat_iot.c,55,main
```

This becomes the ordered point-ID trace:

```text
1, 2, 6
```

## Loader behavior

The analyzer reads `results/week3_traces.csv`, groups rows by:

```text
example + input
```

Then it sorts each group by `sequence` and extracts the ordered `point_id` tuple. That tuple is the raw trace for one execution.

## Observation policies

Week 4 compares three policies:

| Policy    | Meaning                                     |
| --------- | ------------------------------------------- |
| `coarse`  | Shows only entry/exit style points          |
| `limited` | Shows selected meaningful preemption points |
| `all`     | Shows every manually instrumented point ID  |

The `all` policy does **not** mean every machine instruction. It only means every point currently instrumented in the C examples.

### Thermostat policies

```text
coarse: {1, 6}
limited: {1, 2, 4, 5, 6}
all: every thermostat point
```

### Packet-filter policies

```text
coarse: {10, 15}
limited: {10, 11, 12, 14, 15}
all: every packet-filter point
```

## Equivalence classes

After applying a policy, the analyzer groups inputs by identical visible traces.

Example:

```text
trace 1,2,6 -> inputs {16,17}
```

This means inputs `16` and `17` look the same to the observer under that policy. The observer may not know the exact input, but can narrow it to that group.

## Leakage metrics

The analyzer reports:

| Field           | Meaning                                                  |
| --------------- | -------------------------------------------------------- |
| `inputs`        | Number of tested inputs                                  |
| `traces`        | Number of distinct visible traces                        |
| `upper_bits`    | `log2(number of distinct traces)`                        |
| `mutual_info`   | Empirical mutual information under a uniform input prior |
| `largest_class` | Size of the largest equivalence class                    |

`upper_bits` measures how many visible trace outcomes the observer can distinguish. `mutual_info` estimates how much the visible trace reduces uncertainty about the input, assuming all tested inputs are equally likely.

These are empirical measurements for the tested finite domains, not proofs about all possible executions.

## Tested input domains

### Thermostat

```text
-10, -8, -6, ..., 52
```

This gives 32 synthetic temperature inputs covering heat, comfortable, cool, low-alert, and high-alert behavior.

### Packet filter

```text
packet_type in {0, 1, 2}
payload_length in {0, 4, 8, 16, 24, 32, 48, 64}
```

This gives 24 inputs covering status packets, data packets, rejected packets, and repeated payload chunk observations.

## Concrete leakage examples

For thermostat, the trace:

```text
1, 2, 6
```

reveals that the input followed the heat branch.

The trace:

```text
1, 4, 6
```

reveals that the input followed the cool branch.

For packet filter, the trace:

```text
10, 12, 13, 13, 13, 15
```

reveals a data-packet path with three visible chunk observations. This leaks information about both packet type and payload length.

## Design lesson

The main lesson is that visible preemption points can make more program behaviors distinguishable. Some points may be useful for scheduling, but they can also reveal control-flow information.

If the goal were to reduce leakage, one point I would consider hiding or relocating first is:

```text
13 = data payload chunk observation
```

Because it repeats based on payload length, it can reveal more than just the packet branch. It also reveals partial information about payload size.

## Limitations

These results are limited by:

* finite synthetic input domains
* small toy examples
* manually chosen point IDs
* deterministic execution
* uniform input prior
* host-side tracing instead of a real RTOS
* `all` meaning all instrumented points, not all machine instructions

## Week 4 summary

Week 4 connects the raw C trace harness to leakage measurement. The analyzer reconstructs ordered traces, applies coarse, limited, and all-points policies, groups inputs by identical visible traces, and reports leakage metrics. The results show that what an observer can learn depends directly on which preemption points are visible.
