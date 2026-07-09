CC=gcc
CFLAGS=-Wall -Wextra -std=c11 -Iinclude

.PHONY: all run clean

all: thermostat_iot packet_filter

thermostat_iot: examples/thermostat_iot.c src/preempt_point.c include/preempt_point.h
	$(CC) $(CFLAGS) examples/thermostat_iot.c src/preempt_point.c -o thermostat_iot

packet_filter: examples/packet_filter.c src/preempt_point.c include/preempt_point.h
	$(CC) $(CFLAGS) examples/packet_filter.c src/preempt_point.c -o packet_filter

run: all
	python3 scripts/run_all.py
	python3 scripts/analyze_traces.py

clean:
	rm -f thermostat_iot packet_filter
	rm -f results/traces.csv
