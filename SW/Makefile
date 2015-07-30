PROGRAM = main

MCU = atmega328

CC = avr-gcc
OBJCOPY = avr-objcopy
CFLAGS += -Wall -g -Os -mmcu=$(MCU)
LDFLAGS +=

OBJS = $(PROGRAM).o

all: $(PROGRAM)_$(MCU).hex

$(PROGRAM)_$(MCU).elf: $(PROGRAM).o
	@printf " LD $(subst $(shell pwd)/,,$(@))\n"
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

$(PROGRAM)_$(MCU).hex: $(PROGRAM)_$(MCU).elf
	@printf " OBJCOPY $(subst $(shell pwd)/,,$(@))\n"
	$(OBJCOPY) -O ihex $< $@

%.o: %.c
	@printf " CC $(subst $(shell pwd)/,,$(@))\n"
	$(CC) $(CFLAGS) -o $@ -c $<

flash: $(PROGRAM)_$(MCU).hex
	@printf " FLASH $(PROGRAM)_$(MCU).hex\n"
	avrdude -c stk500v1 -P /dev/ttyUSB0 -p $(MCU) -U flash:w:$(PROGRAM)_$(MCU).hex -E noreset

clean:
	@printf " CLEAN $(subst $(shell pwd)/,,$(OBJS))\n"
	rm -f *.o
	@printf " CLEAN $(PROGRAM).elf\n"
	rm -f *.elf
	@printf " CLEAN $(PROGRAM).hex\n"
	rm -f *.hex
