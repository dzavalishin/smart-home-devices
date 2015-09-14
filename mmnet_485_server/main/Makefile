.SILENT:

PROJ   = servant
WEBDIR = html-small
#WEBDIR = html

WEBFILE= urom.c

JTAG_CONF= -B 1000 -j /dev/com6

include ../Makedefs

SRCS =  main.c $(PROJ).c delay.c spi.c runtime_cfg.c util.c modbus.c 
SRCS += io_adc.c io_dig.c io_dht.c io_pwm.c io_temp.c io_bmp180.c
SRCS += onewire.c ds18x20.c
SRCS += $(WEBFILE) web_cgi.c web_cgi_os.c web_setup.c web.c web_status.c httpd.c 
# i2c.c net_io.c icp.c freq_send.c ds18x20.c onewire.c ds2482.c dallas_ds2482.c  child-1wire.c 
OBJS =  $(SRCS:.c=.o)

LIBS =  $(LIBDIR)/nutinit.o $(MODS) -lnutnet -lnutpro -lnutfs -lnutos -lnutdev -lnutnet -lnutcrt -lnutarch -lnutgorp ../../modbus-lib/libmodbus.a  ../../avr-util-lib/libdz-avr-util.a 
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
