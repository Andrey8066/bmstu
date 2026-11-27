###############################################################################
# Makefile for the project avr_I2C_slave
###############################################################################

## General Flags
PROJECT = avr_I2C_slave
MCU = atmega16
TARGET = avr_I2C_slave.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -Os -std=gnu99 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=avr_I2C_slave.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = avr_I2C_slave.o LCD_lib.o I2CSlave.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) avr_I2C_slave.hex avr_I2C_slave.eep avr_I2C_slave.lss size

## Compile
avr_I2C_slave.o: ../avr_I2C_slave.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

LCD_lib.o: ../LCD_lib.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

I2CSlave.o: ../I2CSlave.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) avr_I2C_slave.elf dep/* avr_I2C_slave.hex avr_I2C_slave.eep avr_I2C_slave.lss avr_I2C_slave.map


## Other dependencies
-include $(shell mkdir dep 2>NUL) $(wildcard dep/*)

