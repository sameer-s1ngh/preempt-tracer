#!/usr/bin/env python3

"""
analyze_traces.py

Reads results/traces.csv and groups inputs by identical trace.

This produces equivalence classes:
all inputs in the same class produced the same observable trace.
"""

import csv
import math
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
TRACE_FILE = ROOT / "results" / "traces.csv"


def load_groups():
    groups = defaultdict(list)

    with TRACE_FILE.open(newline="", encoding="utf-8") as csv_file:
        reader = csv.DictReader(csv_file)

        for row in reader:
            example = row["example"]
            input_value = int(row["input"])
            trace = row["trace"]

            key = (example, trace)
            groups[key].append(input_value)

    return groups


def print_groups(groups) -> None:
    current_example = None
    distinct_by_example = defaultdict(int)

    for (example, trace), inputs in sorted(groups.items()):
        if example != current_example:
            print()
            print(f"=== {example} ===")
            current_example = example

        input_set = "{" + ",".join(str(x) for x in inputs) + "}"
        pretty_trace = trace.replace("-", ",")

        print(f"trace {pretty_trace} -> inputs {input_set}")

        distinct_by_example[example] += 1

    print()
    print("=== Summary ===")

    for example, distinct_count in sorted(distinct_by_example.items()):
        leakage_bound = math.log2(distinct_count)

        print(
            f"{example}: distinct traces = {distinct_count}, "
            f"upper bound = log2({distinct_count}) = {leakage_bound:.2f} bits"
        )


def main() -> None:
    if not TRACE_FILE.exists():
        raise FileNotFoundError(
            f"{TRACE_FILE} does not exist. Run `make run` first."
        )

    groups = load_groups()
    print_groups(groups)


if __name__ == "__main__":
    main()
