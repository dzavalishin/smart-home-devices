#.SILENT:

PROJ   = servant
WEBDIR = html-small
#WEBDIR = html

WEBFILE= urom.c

JTAG_CONF= -B 1000 -j /dev/com6
JTAG_CMD=cmd /c avarice 
#JTAG_CMD=avarice 
JTAG=$(JTAG_CMD) $(JTAG_CONF)

include ../Makedefs

SRCS =  main.c $(PROJ).c delay.c spi.c runtime_cfg.c util.c modbus.c map.c
SRCS += io_adc.c io_dig.c io_dht.c io_pwm.c io_temp.c io_bmp180.c
SRCS += onewire.c ds18x20.c tunnel.c
SRCS += $(WEBFILE) web_cgi.c web_cgi_os.c web_setup.c web.c web_status.c httpd.c lua.c syslog.c
# i2c.c net_io.c icp.c freq_send.c ds18x20.c onewire.c ds2482.c dallas_ds2482.c  child-1wire.c 
OBJS =  $(SRCS:.c=.o)

LIBS =  $(LIBDIR)/nutinit.o $(MODS) -lnutnet -lnutpro -lnutfs -lnutos -lnutdev -lnutnet -lnutcrt -lnutarch -lnutgorp ../../modbus-lib/libmodbus.a  ../../avr-util-lib/libdz-avr-util.a 
# -lnutlua 
TARG =  $(PROJ).hex $(PROJ).elf


CFLAGS += -I../../modbus-lib/ -I../../avr-util-lib/ -g



all: makedate.h $(OBJS) $(TARG)
	$(JTAG) -e -p -f $(PROJ).hex

exe: $(TARG)

wfuses:
	$(JTAG) -W ff972f

debug::
	$(JTAG) --ignore-intr :1234 &
	avr-gdb servant.elf

$(WEBFILE): $(WEBDIR)/*.* $(WEBDIR)/i/*.*
	$(CRUROM) -r -o$(WEBFILE) $(WEBDIR)

date_now := $(shell date)

makedate.h: *.c *.h
	echo char *makeDate = \"$(date_now)\"\; > makedate.h

include ../Makerules

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
