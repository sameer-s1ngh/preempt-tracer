#!/usr/bin/env python3

import csv
import math
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

RAW_TRACE_FILE = ROOT / "results" / "week3_traces.csv"
SUMMARY_FILE = ROOT / "results" / "week4_leakage_comparison.csv"

POLICIES = {
    "thermostat": {
        "coarse": {1, 6},
        "limited": {1, 2, 4, 5, 6},
        "all": None,
    },
    "packet_filter": {
        "coarse": {10, 15},
        "limited": {10, 11, 12, 14, 15},
        "all": None,
    },
}


def load_raw_traces():
    """
    Read Week 3 event CSV and reconstruct one ordered point-ID trace
    per example/input execution.
    """
    events_by_run = defaultdict(list)

    with RAW_TRACE_FILE.open(newline="", encoding="utf-8") as csv_file:
        reader = csv.DictReader(csv_file)

        for row in reader:
            example = row["example"]
            input_label = row["input"]
            sequence = int(row["sequence"])
            point_id = int(row["point_id"])

            key = (example, input_label)
            events_by_run[key].append((sequence, point_id))

    raw_traces = []

    for (example, input_label), events in events_by_run.items():
        events.sort(key=lambda event: event[0])
        trace = tuple(point_id for _sequence, point_id in events)

        raw_traces.append(
            {
                "example": example,
                "input": input_label,
                "trace": trace,
            }
        )

    return raw_traces


def apply_policy(trace, allowed_ids):
    """
    Return the visible trace under an observation policy.
    allowed_ids = None means keep every point.
    """
    if allowed_ids is None:
        return trace

    return tuple(point_id for point_id in trace if point_id in allowed_ids)


def group_inputs(raw_traces, example, policy_name):
    """
    Group inputs by identical visible trace.
    """
    allowed_ids = POLICIES[example][policy_name]
    groups = defaultdict(list)

    for run in raw_traces:
        if run["example"] != example:
            continue

        visible = apply_policy(run["trace"], allowed_ids)
        groups[visible].append(run["input"])

    return groups


def compute_metrics(groups):
    """
    Compute distinct traces, log2(k), largest class, and empirical
    mutual information under a uniform input distribution.
    """
    class_sizes = [len(inputs) for inputs in groups.values()]
    total_inputs = sum(class_sizes)
    distinct_traces = len(class_sizes)

    upper_bits = math.log2(distinct_traces)

    conditional_entropy = 0.0

    for size in class_sizes:
        probability = size / total_inputs
        conditional_entropy += probability * math.log2(size)

    mutual_info = math.log2(total_inputs) - conditional_entropy

    return {
        "inputs": total_inputs,
        "traces": distinct_traces,
        "upper_bits": upper_bits,
        "mutual_info": mutual_info,
        "largest_class": max(class_sizes),
    }


def trace_string(trace):
    if not trace:
        return "(empty)"

    return ",".join(str(point_id) for point_id in trace)


def print_groups(example, policy_name, groups, metrics):
    print()
    print(f"=== {example} / {policy_name} ===")

    for trace, inputs in sorted(groups.items()):
        input_set = "{" + ",".join(inputs) + "}"
        print(f"trace {trace_string(trace)} -> inputs {input_set}")

    print(
        f"summary: inputs={metrics['inputs']}, "
        f"traces={metrics['traces']}, "
        f"upper_bits={metrics['upper_bits']:.3f}, "
        f"mutual_info={metrics['mutual_info']:.3f}, "
        f"largest_class={metrics['largest_class']}"
    )


def write_summary(rows):
    with SUMMARY_FILE.open("w", newline="", encoding="utf-8") as csv_file:
        fieldnames = [
            "example",
            "policy",
            "inputs",
            "traces",
            "upper_bits",
            "mutual_info",
            "largest_class",
        ]

        writer = csv.DictWriter(csv_file, fieldnames=fieldnames)
        writer.writeheader()

        for row in rows:
            writer.writerow(row)


def main():
    if not RAW_TRACE_FILE.exists():
        raise FileNotFoundError(
            f"{RAW_TRACE_FILE} does not exist. Run `make run` first."
        )

    raw_traces = load_raw_traces()
    summary_rows = []

    for example in ["thermostat", "packet_filter"]:
        for policy_name in ["coarse", "limited", "all"]:
            groups = group_inputs(raw_traces, example, policy_name)
            metrics = compute_metrics(groups)

            print_groups(example, policy_name, groups, metrics)

            summary_rows.append(
                {
                    "example": example,
                    "policy": policy_name,
                    "inputs": metrics["inputs"],
                    "traces": metrics["traces"],
                    "upper_bits": f"{metrics['upper_bits']:.6f}",
                    "mutual_info": f"{metrics['mutual_info']:.6f}",
                    "largest_class": metrics["largest_class"],
                }
            )

    write_summary(summary_rows)

    print()
    print(f"Wrote {SUMMARY_FILE.relative_to(ROOT)}")


if __name__ == "__main__":
    main()