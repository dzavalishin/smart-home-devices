#
# Copyright (C) 2008 by egnite GmbH. All rights reserved.
# Copyright (C) 2001-2006 by egnite Software GmbH. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. All advertising materials mentioning features or use of this
#    software must display the following acknowledgement:
#
#    This product includes software developed by egnite Software GmbH
#    and its contributors.
#
# THIS SOFTWARE IS PROVIDED BY EGNITE SOFTWARE GMBH AND CONTRIBUTORS
# ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL EGNITE
# SOFTWARE GMBH OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
# OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
# AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
# THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
#
# For additional information see http://www.ethernut.de/
#
# $Log: Makefile,v $
# Revision 1.11  2008/02/15 17:29:59  haraldkipp
# Added missing files. Hopefully not broken after late commit.
#
# Revision 1.10  2006/05/25 09:32:37  haraldkipp
# Incomplete source list results in a bunch of linker errors when building
# in source tree.
#
# Revision 1.9  2005/08/05 11:17:53  olereinhardt
# Added support for _seek, _tell, fseek, ftell functions
#
# Revision 1.8  2004/11/08 09:57:39  drsung
# Added rule to clean also intermediate files (*.i)
#
# Revision 1.7  2004/09/10 17:18:29  haraldkipp
# Building inside source tree is working again
#
# Revision 1.6  2004/05/24 20:26:45  drsung
# Added function kbhit(). Return nonzero if stdin has at least one char
# in input buffer. Uses _filelength to get the file size.
#
# Revision 1.5  2004/04/07 16:28:20  freckle
# Target install: use ranlib only for arch *nix emulation (ranlib is required
# on mac os x)
#
# Revision 1.4  2004/03/16 16:48:27  haraldkipp
# Added Jan Dubiec's H8/300 port.
#
# Revision 1.3  2004/03/03 18:09:32  drsung
# Added function 'fpurge' to crt.
#
# Revision 1.2  2003/11/24 18:10:24  drsung
# Time functions added
#
# Revision 1.1.1.1  2003/05/09 14:40:30  haraldkipp
# Initial using 3.2.1
#
# Revision 1.2  2003/03/31 14:53:05  harald
# Prepare release 3.1
#
# Revision 1.1  2003/02/04 17:49:04  harald
# *** empty log message ***
#
#

PROJ = libnutcrt

top_srcdir = ..
top_blddir = ..

LIBDIR = $(top_blddir)/lib
include $(top_srcdir)/UserConf.mk
include $(top_srcdir)/Makedefs

SRCC =  close.c clrerr.c ioctl.c open.c getf.c read.c putf.c write.c fclose.c \
        fcloseall.c fdopen.c feof.c ferror.c fflush.c filelength.c fileno.c flushall.c \
        fmode.c fopen.c fpurge.c freopen.c fseek.c ftell.c seek.c tell.c fgetc.c fgets.c \
        fread.c fscanf.c getc.c getchar.c gets.c kbhit.c scanf.c ungetc.c vfscanf.c \
        fprintf.c fputc.c fputs.c fwrite.c printf.c putc.c putchar.c puts.c vfprintf.c \
        sprintf.c sscanf.c vsprintf.c vsscanf.c gmtime.c localtim.c mktime.c time.c \
        tzset.c errno.c malloc.c environ.c getenv.c putenv.c setenv.c

SRCCP = fprintf_p.c fputs_p.c fscanf_p.c fwrite_p.c printf_p.c puts_p.c \
	scanf_p.c sprintf_p.c sscanf_p.c vfprintf_p.c vfscanf_p.c \
	vsprintf_p.c vsscanf_p.c write_p.c

ifneq (, $(findstring __HARVARD_ARCH__, $(HWDEF)))
# Include _P functions for Harvard based MCUs
    SRCC += $(SRCCP)
endif

ifneq (, $(findstring h8300, $(ARCH)))
# Include errno definition for H8 based MCUs
    SRCC += errno.c
endif

SRCS =  $(SRCC) putf.c getf.c
SRCF =  $(SRCC) putff.c getff.c

OBJS = $(SRCS:.c=.o)
OBJF = $(SRCF:.c=.o)

TARG  = $(PROJ).a
TARGF = $(PROJ)f.a
TARGS = $(TARG) $(TARGF)

all: $(TARGS)

include $(top_srcdir)/Makerules

$(TARG): $(OBJS)
	$(AR) $(ARFLAGS) $(TARG) $(OBJS)

$(TARGF): $(OBJF)
	$(AR) $(ARFLAGS) $(TARGF) $(OBJF)

install: $(TARGS)
	$(CP) $(TARG) $(LIBDIR)/$(TARG)
	$(CP) $(TARGF) $(LIBDIR)/$(TARGF)
ifeq ($(ARCH), unix)
	$(RANLIB) $(LIBDIR)/$(TARG)
	$(RANLIB) $(LIBDIR)/$(TARGF)
endif

clean:
	-rm -f $(OBJS)
	-rm -f $(OBJF)
	-rm -f $(TARGS)
	-rm -f $(SRCS:.c=.lst) $(SRCF:.c=.lst)
	-rm -f $(SRCS:.c=.lis) $(SRCF:.c=.lis)
	-rm -f $(SRCS:.c=.s) $(SRCF:.c=.s)
	-rm -f $(SRCS:.c=.bak) $(SRCF:.c=.bak)
	-rm -f $(SRCS:.c=.c~) $(SRCF:.c=.c~)
	-rm -f $(SRCS:.c=.i) $(SRCF:.c=.i)
