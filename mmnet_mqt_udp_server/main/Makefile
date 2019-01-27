.SILENT:

#NEW_AVR_DIR=c:/avr-libc-bin/avr
#NEW_AVR_LIB=$(NEW_AVR_DIR)/lib/libc.a
#NEW_AVR_INC=-I$(NEW_AVR_DIR)/include

PROJ   = servant
WEBDIR = html-small
#WEBDIR = html

WEBFILE= urom.c

JTAG_CONF= -B 1000 -j /dev/com6
JTAG_CMD=cmd /c avarice 
#JTAG_CMD=avarice 
JTAG=$(JTAG_CMD) $(JTAG_CONF)

include ../Makedefs

SRCS =  main.c $(PROJ).c delay.c  runtime_cfg.c util.c map.c
#SRCS += modbus.c 
#SRCS += io_adc.c io_dig.c io_dht.c io_pwm.c io_temp.c io_bmp180.c
SRCS += io_dig.c io_pwm.c io_temp.c 
SRCS += onewire.c ds18x20.c 
SRCS += web_cgi.c web_cgi_os.c web_setup.c web.c web_status.c web_cgi_log.c
SRCS += $(WEBFILE) httpd.c lua.c syslog.c
# i2c.c net_io.c icp.c freq_send.c ds18x20.c onewire.c ds2482.c dallas_ds2482.c  child-1wire.c 
SRCS += ui_lcd.c ui_encoder.c ui_menu.c
#ui_menu.c
#SRCS += libemqtt.c mqtt_main.c
SRCS += mqtt_map.c
SRCS += logbuf.c
SRCS += mqtt_udp_glue.c

OBJS =  $(SRCS:.c=.o) 
#eewr_block.o eewr_byte.o

#LIBS =  $(LIBDIR)/nutinit.o $(MODS) -lnutnet -lnutpro -lnutfs -lnutos -lnutdev -lnutnet -lnutcrt -lnutarch -lnutgorp ../../modbus-lib/libmodbus.a  ../../avr-util-lib/libdz-avr-util.a $(NEW_AVR_LIB)
LIBS  =  $(LIBDIR)/nutinit.o $(MODS) -lnutnet -lnutpro -lnutfs -lnutos -lnutdev -lnutnet -lnutcrt -lnutarch -lnutgorp  ../../avr-util-lib/libdz-avr-util.a   $(NEW_AVR_LIB)
LIBS += ../mqtt_udp/libmqttudp.a
#LIBS += -lnutlua 

TARG =  $(PROJ).hex $(PROJ).elf


#CFLAGS += -I../../modbus-lib/ -I../../avr-util-lib/ $(NEW_AVR_INC) -g
CFLAGS +=  -I../mqtt_udp -I../../avr-util-lib/ $(NEW_AVR_INC) -g



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

#makedate.h:
#	echo Make Date = $(date_now)
#	echo char *makeDate = \"$(date_now)\"\; > makedate.h


include ../Makerules

-include *.d

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
