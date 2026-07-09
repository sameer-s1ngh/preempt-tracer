#!/usr/bin/env python3

"""
run_all.py

Compiles are handled by the Makefile.
This script runs both C examples over a finite input set and writes traces to:

results/traces.csv
"""

import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
RESULTS_DIR = ROOT / "results"
TRACE_FILE = RESULTS_DIR / "traces.csv"

PROGRAMS = [
    ROOT / "thermostat_iot",
    ROOT / "packet_filter",
]

INPUT_RANGE = range(32)


def main() -> None:
    RESULTS_DIR.mkdir(exist_ok=True)

    with TRACE_FILE.open("w", encoding="utf-8") as output:
        output.write("example,input,trace\n")

        for program in PROGRAMS:
            for secret in INPUT_RANGE:
                result = subprocess.run(
                    [str(program), str(secret)],
                    cwd=ROOT,
                    capture_output=True,
                    text=True,
                    check=True,
                )

                output.write(result.stdout)

    print(f"Wrote {TRACE_FILE.relative_to(ROOT)}")


if __name__ == "__main__":
    main()
