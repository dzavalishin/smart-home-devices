#
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
# Revision 1.8  2006/07/21 09:13:59  haraldkipp
# Added creation of *.elf for 'make all'.
#
# Revision 1.7  2006/01/23 19:49:17  haraldkipp
# Re-arranging library list. Again!
#
# Revision 1.6  2005/07/26 15:44:52  haraldkipp
# Include new libnutarch into the build
#
# Revision 1.5  2004/11/08 09:57:39  drsung
# Added rule to clean also intermediate files (*.i)
#
# Revision 1.4  2004/09/10 10:29:11  haraldkipp
# Undefined refs solved
#
# Revision 1.3  2004/09/08 10:18:15  haraldkipp
# Configurable library list
#
# Revision 1.2  2004/08/18 15:02:36  haraldkipp
# Application build is no longer fixed in top_srcdir
#
# Revision 1.1  2003/08/05 18:59:05  haraldkipp
# Release 3.3 update
#
# Revision 1.7  2003/02/04 16:24:36  harald
# Adapted to version 3
#
# Revision 1.6  2002/10/31 16:26:30  harald
# Mods by troth for Linux
#
# Revision 1.5  2002/08/11 12:28:41  harald
# Using hex file extension now
#
# Revision 1.4  2002/06/26 17:29:06  harald
# First pre-release with 2.4 stack
#
# Revision 1.3  2002/06/04 18:49:11  harald
# New make environment
#
#

PROJ = threads

include ../Makedefs

SRCS =  $(PROJ).c
OBJS =  $(SRCS:.c=.o)
LIBS =  $(LIBDIR)/nutinit.o -lnutos -lnutarch -lnutdev -lnutarch -lnutos -lnutcrt $(ADDLIBS)
TARG =  $(PROJ).hex

all: $(OBJS) $(TARG) $(ITARG) $(DTARG)

include ../Makerules

clean:
	-rm -f $(OBJS)
	-rm -f $(TARG) $(ITARG) $(DTARG)
	-rm -f $(PROJ).eep
	-rm -f $(PROJ).obj
	-rm -f $(PROJ).map
	-rm -f $(SRCS:.c=.lst)
	-rm -f $(SRCS:.c=.bak)
	-rm -f $(SRCS:.c=.i)
