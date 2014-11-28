PROJ   = servant
WEBDIR = html-small
#WEBDIR = html

WEBFILE= urom.c

include ../Makedefs

SRCS =  $(PROJ).c $(WEBFILE) crc8.c crc16.c cgi.c oscgi.c delay.c spi.c runtime_cfg.c util.c httpd.c websetup.c
# i2c.c net_io.c adc.c alarm.c dports.c icp.c freq_send.c ds18x20.c temperature.c onewire.c ds2482.c dallas_ds2482.c  child-1wire.c 
OBJS =  $(SRCS:.c=.o)
#MODS =  $(MODDIR)/dev/lanc111.o
LIBS =  $(LIBDIR)/nutinit.o $(MODS) -lnutnet -lnutpro -lnutfs -lnutos -lnutdev -lnutnet -lnutcrt
TARG =  $(PROJ).hex $(PROJ).elf

ifneq (, $(findstring h8300, $(ARCH)))
# Libraries specific for H8 port
LIBS += -lnutentry -lc -lgcc
endif



all: $(OBJS) $(TARG)
	avarice -j /dev/com6 -e -p -f $(PROJ).hex

exe: $(TARG)


$(WEBFILE): $(WEBDIR)/*.* $(WEBDIR)/i/*.*
	$(CRUROM) -r -o$(WEBFILE) $(WEBDIR)



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
	-rm -f *.c~ *.h~
