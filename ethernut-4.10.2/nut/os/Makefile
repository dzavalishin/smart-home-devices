#
# Copyright (C) 2008 by egnite GmbH. All rights reserved.
# Copyright (C) 2001-2002 by egnite Software GmbH. All rights reserved.
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
# Revision 1.13  2008/04/21 22:30:58  olereinhardt
# Added condition.c
#
# Revision 1.12  2008/02/15 17:30:00  haraldkipp
# Added missing files. Hopefully not broken after late commit.
#
# Revision 1.11  2004/11/08 09:57:40  drsung
# Added rule to clean also intermediate files (*.i)
#
# Revision 1.10  2004/09/16 11:42:01  olereinhardt
# Changed Makefile to install libraries into correct directory
#
# Revision 1.9  2004/09/10 17:18:29  haraldkipp
# Building inside source tree is working again
#
# Revision 1.8  2004/05/18 10:23:41  haraldkipp
# Typo corrected
#
# Revision 1.7  2004/05/17 18:59:49  freckle
# Implemented recursive mutex and semaphore functions for nut/os based on the
# posix specifation using existing nut/os primitives (post/wait event)
#
# Revision 1.6  2004/04/07 16:28:24  freckle
# Target install: use ranlib only for arch *nix emulation (ranlib is required
# on mac os x)
#
# Revision 1.5  2004/04/07 12:13:58  haraldkipp
# Matthias Ringwald's *nix emulation added
#
# Revision 1.4  2004/03/16 16:48:45  haraldkipp
# Added Jan Dubiec's H8/300 port.
#
# Revision 1.3  2004/02/04 18:05:46  drsung
# Message queueing added.
#
# Revision 1.2  2003/07/21 18:12:44  haraldkipp
# IR and banked memory support added
#
# Revision 1.1.1.1  2003/05/09 14:41:51  haraldkipp
# Initial using 3.2.1
#
# Revision 1.13  2003/02/04 18:15:56  harald
# Version 3 released
#
# Revision 1.12  2003/01/17 18:56:28  harald
# Device routines splitted
#
# Revision 1.11  2002/10/31 16:34:41  harald
# Mods by troth for Linux
#
# Revision 1.10  2002/07/03 16:45:42  harald
# Using GCC 3.2
#
# Revision 1.9  2002/06/26 17:29:44  harald
# First pre-release with 2.4 stack
#
# Revision 1.8  2002/06/04 18:40:21  harald
# New make environment
#
# Revision 1.7  2002/05/08 16:03:15  harald
# First Imagecraft compilation
#
# Revision 1.6  2002/01/28 19:31:08  harald
# *** empty log message ***
#
# Revision 1.5  2001/12/04 16:12:30  harald
# First check in
#
# Revision 1.4  2001/06/09 10:25:32  harald
# Cleanup
#
# Revision 1.3  2001/05/27 20:01:52  harald
# New device structure implemented
#
# Revision 1.2  2001/05/19 14:06:18  harald
# Removed init from lib
#
#

PROJ = libnutos

top_srcdir = ..
top_blddir = ..

LIBDIR = $(top_blddir)/lib
include $(top_srcdir)/UserConf.mk
include $(top_srcdir)/Makedefs

SRC1 = 	nutinit.c
SRC2 =  devreg.c timer.c msg.c event.c thread.c heap.c osdebug.c confos.c \
	version.c semaphore.c mutex.c tracer.c condition.c

#Add support for memory banks on AVR architecture
ifeq ($(ARCH), avr)
    SRC2 += bankmem.c
endif


OBJ1 = $(SRC1:.c=.o)
OBJ2 = $(SRC2:.c=.o) 


TRG1 = $(OBJ1) 
TRG2 = $(PROJ).a $(SRC1.c=.d)

all: $(TRG1) $(TRG2)

include $(top_srcdir)/Makerules
$(TRG1): $(SRC1.c=.d)

$(TRG2): $(OBJ2) $(SRC2.c=.d)
	$(AR) $(ARFLAGS) $(TRG2) $(OBJ2)

install: $(TRG1) $(TRG2)
	$(CP) $(TRG1) $(LIBDIR)/$(TRG1)
	$(CP) $(TRG2) $(LIBDIR)/$(TRG2)
ifeq ($(ARCH), unix)
	$(RANLIB) $(LIBDIR)/$(TRG2)
endif

clean:
	-rm -f $(OBJ1) $(OBJ2)
	-rm -f $(TRG1) $(TRG2)
	-rm -f $(SRC1:.c=.lst) $(SRC2:.c=.lst)
	-rm -f $(SRC1:.c=.lis) $(SRC2:.c=.lis)
	-rm -f $(SRC1:.c=.s) $(SRC2:.c=.s)
	-rm -f $(SRC1:.c=.bak) $(SRC2:.c=.bak)
	-rm -f $(SRC1:.c=.c~) $(SRC2:.c=.c~)
	-rm -f $(SRC1:.c=.i) $(SRC2:.c=.i)
