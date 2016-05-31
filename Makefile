BIN = example_DS18B20

PORT ?= usb
MCU_TARGET ?= atmega328p
F_CPU ?= 16000000UL

AVRDUDE_TARGET ?= m328p
PROGRAMMER ?= usbasp-clone

INCLUDEDIRS += .
INCLUDEDIRS +=

DEFINES += F_CPU=$(F_CPU)
DEFINES +=

LIBDIRS +=
LIBS +=

COMMONFLAGS += -mmcu=$(MCU_TARGET)

CFLAGS += -c $(COMMONFLAGS) -MD -Os -std=gnu99
CFLAGS += $(addprefix -I, $(INCLUDEDIRS))
CFLAGS += $(addprefix -D, $(DEFINES))

LDFLAGS += $(COMMONFLAGS)
LDFLAGS += $(addprefix -L, $(LIBDIRS)) $(LIBS)

TOOLS_PATH ?= /usr/local
TOOLS_PREFIX ?= avr-

FLASH ?= $(shell which avrdude)
FLASHFLAGS ?= -p $(AVRDUDE_TARGET) -c $(PROGRAMMER) -P $(PORT)

CC = $(TOOLS_PATH)/bin/$(TOOLS_PREFIX)gcc
OBJCOPY = $(TOOLS_PATH)/bin/$(TOOLS_PREFIX)objcopy
OBJDUMP = $(TOOLS_PATH)/bin/$(TOOLS_PREFIX)objdump
SIZE = $(TOOLS_PATH)/bin/$(TOOLS_PREFIX)size

OBJDIR ?= mk

SRC ?= $(wildcard *.c)
OBJ = $(addprefix $(OBJDIR)/, $(SRC:%.c=%.o))

DEPS = $(OBJ:.o=.d)

HEX ?= $(BIN).hex
ELF ?= $(OBJDIR)/$(BIN).elf
LIST ?= $(OBJDIR)/$(BIN).list
MAP ?= $(OBJDIR)/$(BIN).map

all: hex

hex: $(HEX)
elf: $(ELF)
list: $(LIST)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

$(ELF): $(OBJDIR) $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -Wl,-Map=$(MAP) -o $@

$(HEX): $(ELF)
	$(OBJCOPY) -Oihex -R.eeprom $< $@
	$(SIZE) -B $<

$(LIST): $(ELF)
	$(OBJDUMP) -S $< > $@

clean:
	rm -f $(OBJ) $(ELF) $(MAP) $(LIST) $(HEX) $(DEPS)
	rmdir $(OBJDIR)

flash: $(HEX)
	$(FLASH) $(FLASHFLAGS) -U flash:w:$<

upload: flash

-include $(DEPS)

.PHONY: clean flash upload hex list elf
