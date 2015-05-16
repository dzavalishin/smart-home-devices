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
# Revision 1.3  2006/07/21 09:13:58  haraldkipp
# Added creation of *.elf for 'make all'.
#
# Revision 1.2  2006/07/10 14:23:20  haraldkipp
# C++ lib added. Contributed by Matthias Wilde.
#
# Revision 1.1  2005/01/22 19:03:31  haraldkipp
# Kindly contributed by Oliver Schulz (MDI)
#
#

PROJ = cppdemo

include ../Makedefs

SRCS =  $(PROJ).cc
OBJS =  $(SRCS:.cc=.o)
LIBS =   -lnutcpp $(LIBDIR)/nutinit.o $(MODS) -lnutcpp -lnutos -lnutcrt -lnutarch -lnutdev
TARG =  $(PROJ).hex

all: $(OBJS) $(TARG) $(ITARG) $(DTARG)

include ../Makerules

clean:
	-rm -f $(OBJS)
	-rm -f $(TARG) $(ITARG) $(DTARG)
	-rm -f $(PROJ).eep
	-rm -f $(PROJ).obj
	-rm -f $(PROJ).map
	-rm -f $(PROJ).dbg
	-rm -f $(PROJ).cof
	-rm -f $(PROJ).mp
	-rm -f $(SRCS:.cc=.lst)
	-rm -f $(SRCS:.cc=.lis)
	-rm -f $(SRCS:.cc=.s)
	-rm -f $(SRCS:.cc=.bak)
	-rm -f $(SRCS:.cc=.i)
