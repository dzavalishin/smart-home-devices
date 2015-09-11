
# version
BUILD      = 000000

# program name should not be changed...
PROGRAM    = libdz-avr-util


MCU_TARGET = atmega128
#MCU_TARGET = atmega32

LDSECTION  = --section-start=.text=0x00000

OBJ        = crc8.o crc16.o


OPTIMIZE   = -O2

DEFS       =
LIBS       =

CC         = avr-gcc


# Override is only needed by avr-lib build system.

override CFLAGS        = -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) $(DEFS)  -MMD 
#-fdiagnostics-color=auto

#export GCC_COLORS=error=01;31:warning=01;35:note=01;36:caret=01;32:locus=01:quote=01

OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump

all: $(PROGRAM).a

$(PROGRAM).a: $(OBJ)
	ar crs $(PROGRAM).a $(OBJ)



clean:
	rm -rf *.o *.elf *.lst *.map *.sym *.lss *.eep *.d d/*.d *.c~ *.h~
	rm -rf $(PROGRAM).hex $(PROGRAM).srec $(PROGRAM).bin



-include *.d
-include d/*.d
