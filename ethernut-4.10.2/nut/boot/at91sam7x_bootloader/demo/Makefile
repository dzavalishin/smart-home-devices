PROJ = demo-app

#Please adjust the Makedefs path to your Nut/OS application path 
#       (configuration using at91sam7xXXX_bootrom.ld linker script)
include ../../../nut-apps/Makedefs

SRCS =  $(PROJ).c
OBJS =  $(SRCS:.c=.o)
LIBS =  $(LIBDIR)/nutinit.o -lnutcrt -lnutarch -lnutdev -lnutos -lnutdev -lnutarch 
TARG =  $(PROJ).hex

all: $(OBJS) $(TARG) $(ITARG) $(DTARG)

#Please adjust the Makerules path to your Nut/OS application path 
#       (configuration using at91sam7xXXX_bootrom.ld linker script)
include ../../../nut-apps/Makerules

clean:
	-rm -f $(OBJS)
	-rm -f $(TARG) $(ITARG) $(DTARG)
	-rm -f $(PROJ).eep
	-rm -f $(PROJ).obj
	-rm -f $(PROJ).map
	-rm -f $(PROJ).dbg
	-rm -f $(PROJ).cof
	-rm -f $(PROJ).mp
	-rm -f $(SRCS:.c=.lst)
	-rm -f $(SRCS:.c=.lis)
	-rm -f $(SRCS:.c=.s)
	-rm -f $(SRCS:.c=.bak)
	-rm -f $(SRCS:.c=.i)
