#!/usr/bin/env python3

from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
RESULTS_DIR = ROOT / "results"
TRACE_FILE = RESULTS_DIR / "week3_traces.csv"

THERMOSTAT_EXE = ROOT / "build" / "thermostat_iot"
PACKET_EXE = ROOT / "build" / "packet_filter"

# 32 synthetic temperatures: -10, -8, ..., 52.
THERMOSTAT_INPUTS = list(range(-10, 54, 2))

# 24 synthetic packet inputs: 3 packet types x 8 payload lengths.
PACKET_TYPES = [0, 1, 2]
PAYLOAD_LENGTHS = [0, 4, 8, 16, 24, 32, 48, 64]


def run_command(args):
    result = subprocess.run(
        [str(arg) for arg in args],
        cwd=ROOT,
        text=True,
        capture_output=True,
        check=False,
    )

    if result.returncode != 0:
        print("Command failed:", " ".join(str(arg) for arg in args), file=sys.stderr)
        print(result.stderr, file=sys.stderr)
        raise SystemExit(result.returncode)

    return result.stdout


def count_event_rows(text):
    return sum(1 for line in text.splitlines() if line.strip())


def main():
    RESULTS_DIR.mkdir(exist_ok=True)

    thermostat_runs = 0
    thermostat_events = 0
    packet_runs = 0
    packet_events = 0

    with TRACE_FILE.open("w", encoding="utf-8", newline="") as output:
        output.write("example,input,sequence,point_id,file,line,function\n")

        for temperature in THERMOSTAT_INPUTS:
            rows = run_command([THERMOSTAT_EXE, temperature])
            output.write(rows)
            thermostat_runs += 1
            thermostat_events += count_event_rows(rows)

        for packet_type in PACKET_TYPES:
            for payload_length in PAYLOAD_LENGTHS:
                rows = run_command([PACKET_EXE, packet_type, payload_length])
                output.write(rows)
                packet_runs += 1
                packet_events += count_event_rows(rows)

    print("example        tested inputs  runs completed  events written")
    print(f"thermostat     {len(THERMOSTAT_INPUTS):<14} {thermostat_runs:<15} {thermostat_events}")
    print(f"packet_filter  {len(PACKET_TYPES) * len(PAYLOAD_LENGTHS):<14} {packet_runs:<15} {packet_events}")
    print(f"trace file: {TRACE_FILE.relative_to(ROOT)}")


if __name__ == "__main__":
    main()