# GNU-ARM tools directory
GNU_ARM = C:\Program Files\CodeSourcery\Sourcery G++ Lite

# The gcc compiler and linker
CC      = $(GNU_ARM)\bin\arm-none-eabi-gcc
ASM     = $(GNU_ARM)\bin\arm-none-eabi-as
LINK    = $(GNU_ARM)\bin\arm-none-eabi-gcc
OBJCOPY = $(GNU_ARM)\bin\arm-none-eabi-objcopy

BLDDIR  = .

CCINC   = -I$(BLDDIR)

APP_DEP = $(BLDDIR)\board.h 

ARM_CPU  = arm7tdmi
CHIP     = at91sam7s256
APP_NAME = bootldr_$(CHIP)
BINDIR   = bin


CCFLAGS = -c -mcpu=$(ARM_CPU) -D$(CHIP) -mthumb-interwork -Os -mlong-calls -ffunction-sections -Wall -DNDBEBUG -o$@
CCFLAGS += -Wa,-adhlns=$(subst $(suffix $<),.lst,$<) 
ASMFLAGS = -gdwarf2 -mcpu=$(ARM_CPU) -mthumb-interwork -o$@
ASMFLAGS += -adhlns=$(subst $(suffix $<),.lst,$<)
LINKFLAGS = -T .\$(APP_NAME).ld -o $(BINDIR)\$(APP_NAME).elf -Wl,-Map,$(BINDIR)\$(APP_NAME).map,--cref -lm


#.............................................................................

all: $(BINDIR)\$(APP_NAME).bin

$(BINDIR)\$(APP_NAME).bin : $(BINDIR)\$(APP_NAME).elf
	$(OBJCOPY) -O binary $(BINDIR)\$(APP_NAME).elf $(BINDIR)\$(APP_NAME).bin

$(BINDIR)\$(APP_NAME).elf : \
	.\$(APP_NAME).ld \
	$(BINDIR)\startup.o \
	$(BINDIR)\main.o
	$(LINK) \
	$(BINDIR)\startup.o \
	$(BINDIR)\main.o \
	$(LINKFLAGS)


$(BINDIR)\startup.o: $(BLDDIR)\startup.s
	$(ASM) $(ASMFLAGS) $<

$(BINDIR)\board.o: $(BLDDIR)\board.c $(APP_DEP)
	$(CC) -marm $(CCFLAGS) $(CCINC) $<

$(BINDIR)\Atmel_OS.o: $(BLDDIR)\Atmel_OS.c $(APP_DEP)
	$(CC) -mthumb $(CCFLAGS) $(CCINC) $<

$(BINDIR)\main.o: $(BLDDIR)\main.c $(APP_DEP)
	$(CC) -mthumb $(CCFLAGS) $(CCINC) $<


bin: all

# clean targets...
clean:   
	-rm -f $(BINDIR)/*.o $(BINDIR)/*.elf $(BINDIR)/*.map
