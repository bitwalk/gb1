GBDK_HOME ?= ../gbdk
LCC := $(GBDK_HOME)/bin/lcc
TARGET := star-catcher.gb
SOURCES := src/main.c

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(LCC) -msm83:gb -o $@ $(SOURCES)

clean:
	rm -f $(TARGET) *.asm *.cdb *.ihx *.lk *.lst *.map *.noi *.o *.rel *.rst *.sym

# Set EMULATOR to your preferred Game Boy emulator, for example:
# make run EMULATOR=binjgb
run: $(TARGET)
	$(EMULATOR) $(TARGET)
