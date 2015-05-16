PROJ =	libnutlua

include ../../Makedefs

SRCS =	lapi.c ldo.c ldump.c ldebug.c lfunc.c lgc.c \
	lmem.c lobject.c lopcodes.c lstate.c lstring.c ltable.c \
	ltm.c lundump.c lvm.c lzio.c lauxlib.c linit.c \
	loadlib.c lbaselib.c liolib.c loslib.c ldblib.c \
	lmathlib.c ltablib.c lstrlib.c \
	lcode.c llex.c lparser.c lrotable.c

OBJS =	$(SRCS:.c=.o)

all:	$(PROJ).a $(OBJS)

include ../../Makerules

clean:
	-rm -f $(PROJ).a
	-rm -f $(PROJ).map
	-rm -f $(OBJS)
	-rm -f $(SRCS:.c=.lst)
