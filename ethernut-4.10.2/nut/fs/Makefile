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
# Revision 1.9  2008/02/15 17:30:00  haraldkipp
# Added missing files. Hopefully not broken after late commit.
#
# Revision 1.8  2006/05/25 09:28:54  haraldkipp
# Latest sources added, so building in the source tree will create complete
# libraries.
#
# Revision 1.7  2005/02/05 20:35:21  haraldkipp
# Peanut added
#
# Revision 1.6  2004/11/08 09:57:40  drsung
# Added rule to clean also intermediate files (*.i)
#
# Revision 1.5  2004/09/10 17:18:29  haraldkipp
# Building inside source tree is working again
#
# Revision 1.4  2004/04/07 16:28:22  freckle
# Target install: use ranlib only for arch *nix emulation (ranlib is required
# on mac os x)
#
# Revision 1.3  2004/04/07 12:13:56  haraldkipp
# Matthias Ringwald's *nix emulation added
#
# Revision 1.2  2004/03/16 16:48:27  haraldkipp
# Added Jan Dubiec's H8/300 port.
#
# Revision 1.1.1.1  2003/05/09 14:41:01  haraldkipp
# Initial using 3.2.1
#
# Revision 1.6  2003/02/04 17:57:14  harald
# Version 3 released
#
# Revision 1.5  2003/01/14 16:31:12  harald
# *** empty log message ***
#
# Revision 1.4  2002/11/02 15:15:42  harald
# Adapted to configure tool
#
# Revision 1.3  2002/06/26 17:29:13  harald
# First pre-release with 2.4 stack
#
# Revision 1.2  2002/06/04 18:40:43  harald
# New make environment
#
# Revision 1.1  2001/06/09 10:36:12  harald
# First check in
#
#

PROJ = libnutfs

top_srcdir = ..
top_blddir = ..

LIBDIR = $(top_blddir)/lib
include $(top_srcdir)/UserConf.mk
include $(top_srcdir)/Makedefs

SRCS = pathops.c dirent.c fat.c uromfs.c pnutfs.c \
       phatfs.c phatvol.c phatdir.c phatio.c \
       phat12.c phat16.c phat32.c phatutil.c phatdbg.c \
       basename.c dirname.c

OBJS = $(SRCS:.c=.o)

TARG = $(PROJ).a

all: $(TARG)

include $(top_srcdir)/Makerules

$(TARG): $(OBJS)
	$(AR) $(ARFLAGS) $(TARG) $(OBJS)

install: $(TARG)
	$(CP) $(TARG) $(LIBDIR)/$(TARG)
ifeq ($(ARCH), unix)
	$(RANLIB) $(LIBDIR)/$(TARG)
endif

clean:
	-rm -f $(OBJS)
	-rm -f $(TARG)
	-rm -f $(SRCS:.c=.lst)
	-rm -f $(SRCS:.c=.lis)
	-rm -f $(SRCS:.c=.s)
	-rm -f $(SRCS:.c=.bak)
	-rm -f $(SRCS:.c=.c~)
	-rm -f $(SRCS:.c=.i)
