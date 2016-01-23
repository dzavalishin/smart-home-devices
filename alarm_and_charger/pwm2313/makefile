
# version
BUILD      = 000000

# program name should not be changed...
PROGRAM    = pwm2313


# MCU_TARGET = atmega128
MCU_TARGET = attiny2313

LDSECTION  = --section-start=.text=0x00000

OBJ        = main.o util.o pwm.o spi.o spi_ss.o usart.o encoder.o eeprom.o
#inputs.o outputs.o
OPTIMIZE   = -O2

DEFS       =
LIBS       =

CC         = avr-gcc

JTAG_CONF= -B 1000 -j /dev/com6


#ISPFUSES    = avrdude -c $(ISPTOOL) -p m128 -P $(ISPPORT) $(ISPSPEED) -u -U efuse:w:0xff:m -U hfuse:w:0xc8:m -U lfuse:w:0xdf:m
#ISPFLASH    = avrdude -c $(ISPTOOL) -p m128 -P $(ISPPORT) $(ISPSPEED) -V -U flash:w:$(PROGRAM)_$(PRODUCT)_$(BUILD).hex

ISPFLASH    = avarice $(JTAG_CONF) -e -p -f $(PROGRAM).hex
#ISPFUSES    = avarice $(JTAG_CONF) -W ff972f

#debug::
#	avarice -B 1000 -j /dev/com6 --ignore-intr :1234 &
#	avr-gdb servant.elf


# Override is only needed by avr-lib build system.

override CFLAGS        = -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS)
override LDFLAGS       = -Wl,-Map,$(PROGRAM).map,$(LDSECTION)

OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump

all: $(PROGRAM).elf text #isp

isp: $(PROGRAM).hex
	$(ISPFUSES)
	$(ISPFLASH)

$(PROGRAM).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf *.o *.elf *.lst *.map *.sym *.lss *.eep
	rm -rf $(PROGRAM).hex $(PROGRAM).srec $(PROGRAM).bin

lst:  $(PROGRAM).lst

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

# Rules for building the .text rom images

text: hex bin srec

hex:  $(PROGRAM).hex
bin:  $(PROGRAM).bin
srec: $(PROGRAM).srec

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@
	cp $@ $(PROGRAM)_$(PRODUCT)_$(AVR_FREQ)_$(BUILD).hex

%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@
	cp $@ $(PROGRAM)_$(PRODUCT)_$(AVR_FREQ)_$(BUILD).srec

%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@
	cp $@ $(PROGRAM)_$(PRODUCT)_$(AVR_FREQ)_$(BUILD).bin


# deps

#main.o: uart.o util.o timer0.o 


main.o: *.h

util.o: *.h

pwm.o: *.h

spi.o: *.h
ss_spi.o: *.h

usart.o: *.h

encoder.o: *.h

eeprom.o: *.h