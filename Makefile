CC=cc
CFLAGS=-std=c11 -O0 -g -Wall -Wextra -Wpedantic -Iinclude

BUILD_DIR=build

THERMOSTAT=$(BUILD_DIR)/thermostat_iot
PACKET_FILTER=$(BUILD_DIR)/packet_filter

.PHONY: all run week3 week4 clean check

all: $(THERMOSTAT) $(PACKET_FILTER)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(THERMOSTAT): src/preempt_point.c examples/thermostat_iot.c include/preempt_point.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) src/preempt_point.c examples/thermostat_iot.c -o $(THERMOSTAT)

$(PACKET_FILTER): src/preempt_point.c examples/packet_filter.c include/preempt_point.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) src/preempt_point.c examples/packet_filter.c -o $(PACKET_FILTER)

run: all
	python3 scripts/run_all.py

week3: run

week4: run
	python3 scripts/analyze_traces.py

check: all
	./$(THERMOSTAT) 17
	./$(PACKET_FILTER) 1 24

clean:
	rm -rf $(BUILD_DIR)
	rm -f results/week3_traces.csv
	rm -f results/week4_leakage_comparison.csv