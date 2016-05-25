
# version
BUILD      = 000000

# program name should not be changed...
PROGRAM    = PumpControl


MCU_TARGET = atmega32

LDSECTION  = --section-start=.text=0x00000

OBJ        = $(PROGRAM).o adc.o uart.o util.o pwm.o timer0.o delay.o temp_sens_eeprom.o uart_debug.o  ui_menu.o ui_lcd.o
OBJ       += crc8.o onewire.o ds18x20.o temperature.o 
OBJ       += modbus_ascii.o modbus_crc.o modbus_pkt.o modbus_reg.o dcon.o pump.o errlog.o
# crc16.o

OPTIMIZE   = -O2

DEFS       =
LIBS       =

CC         = avr-gcc


# Override is only needed by avr-lib build system.

override CFLAGS        = -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS)  -MMD 
#-fdiagnostics-color=auto
override LDFLAGS       = -Wl,-Map,$(PROGRAM).map,$(LDSECTION)

#export GCC_COLORS=error=01;31:warning=01;35:note=01;36:caret=01;32:locus=01:quote=01

OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump

all: $(PROGRAM).elf lst text


isp: $(PROGRAM).hex
	$(ISPFUSES)
	$(ISPFLASH)

$(PROGRAM).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)
	-mv --target-directory=d *.d

clean:
	rm -rf *.o *.elf *.lst *.map *.sym *.lss *.eep *.d d/*.d *.c~ *.h~
	rm -rf $(PROGRAM).hex $(PROGRAM).srec $(PROGRAM).bin

lst:  $(PROGRAM).lst

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

# Rules for building the .text rom images

text: hex bin 
#srec

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

#$(PROGRAM).o: uart.o util.o adc.o pwm.o timer0.o  crc8.o onewire.o ds18x20.o delay.o temperature.o lcd.o
#$(PROGRAM).o: $(OBJ)

# dports.o freq.o spi.o

delay.h: freq_defs.h

uart.o: uart.h freq_defs.h defs.h modbus.h

util.o: util.h defs.h freq_defs.h

adc.o: adc.h defs.h

#freq.o: freq.h defs.h

crc8.o: crc8.h defs.h

onewire.o: onewire.h defs.h

ds18x20.o: ds18x20.h defs.h

delay.o: delay.h defs.h

temperature.o: temperature.h defs.h

spi.o: spi.h util.h defs.h

pwm.o: freq_defs.h

timer0.o: freq_defs.h

lcd.o: lcd.h defs.h lcdconf.h

ui_menu.o: ui_menu.h

-include *.d
-include d/*.d
