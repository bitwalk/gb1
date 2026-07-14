GBDK_HOME ?= ../gbdk
LCC := $(GBDK_HOME)/bin/lcc
TARGET := star-catcher.gb
SOURCES := src/main.c
BENCHMARK_SOURCE := benchmarks/object-layout.c
BENCHMARK_TARGETS := benchmark-aos.gb benchmark-soa.gb

.PHONY: all benchmark-layout clean run

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(LCC) -msm83:gb -o $@ $(SOURCES)

benchmark-layout: $(BENCHMARK_TARGETS)

benchmark-aos.gb: $(BENCHMARK_SOURCE)
	$(LCC) -msm83:gb -DOBJECT_LAYOUT_SOA=0 -o $@ $<

benchmark-soa.gb: $(BENCHMARK_SOURCE)
	$(LCC) -msm83:gb -DOBJECT_LAYOUT_SOA=1 -o $@ $<

clean:
	rm -f $(TARGET) $(BENCHMARK_TARGETS) *.asm *.cdb *.ihx *.lk *.lst *.map *.noi *.o *.rel *.rst *.sym

# Set EMULATOR to your preferred Game Boy emulator, for example:
# make run EMULATOR=binjgb
run: $(TARGET)
	$(EMULATOR) $(TARGET)
