.SILENT:

PROJ   = servant
WEBDIR = html-small
#WEBDIR = html

WEBFILE= urom.c

JTAG_CONF= -B 1000 -j /dev/com6

include ../nutos_Makedefs

SRCS =  main.c $(PROJ).c delay.c runtime_cfg.c util.c modbus.c dev_map.c
SRCS += io_adc.c io_dig.c io_pwm.c io_temp.c io_spi.c io_twi.c
SRCS += onewire.c ds18x20.c tunnel.c prop.c charger.c
SRCS += $(WEBFILE) web_cgi.c web_cgi_os.c web_setup.c web_util.c web_status.c web_httpd.c lua.c syslog.c eeprom.c
SRCS += ui_lcd.c ui_lcd_queue.c ui_menu.c

OBJS =  $(SRCS:.c=.o)

LIBS =  $(LIBDIR)/nutinit.o $(MODS) -lnutnet -lnutpro -lnutfs -lnutos -lnutdev -lnutnet -lnutcrt -lnutarch -lnutgorp ../../modbus-lib/libmodbus.a  ../../avr-util-lib/libdz-avr-util.a 
# -lnutlua 
TARG =  $(PROJ).hex $(PROJ).elf


CFLAGS += -I../../modbus-lib/ -I../../avr-util-lib/
# -g



all: makedate.h $(OBJS) $(TARG)
	avarice $(JTAG_CONF) -e -p -f $(PROJ).hex

exe: $(TARG)

wfuses:
	avarice $(JTAG_CONF) -W ff972f

debug::
	avarice -B 1000 -j /dev/com6 --ignore-intr :1234 &
	avr-gdb servant.elf

$(WEBFILE): $(WEBDIR)/*.* $(WEBDIR)/i/*.*
	$(CRUROM) -r -o$(WEBFILE) $(WEBDIR)

date_now := $(shell date)

makedate.h: *.c *.h
	echo char *makeDate = \"$(date_now)\"\; > makedate.h

include ../nutos_Makerules

clean:
	-rm -f $(OBJS)
	-rm -f $(TARG)
	-rm -f $(PROJ).eep
	-rm -f $(PROJ).obj
	-rm -f $(PROJ).map
	-rm -f $(SRCS:.c=.lst)
	-rm -f $(SRCS:.c=.bak)
	-rm -f $(WEBFILE)
	-rm -f *.c~ *.h~ *.d *.o *.lst
