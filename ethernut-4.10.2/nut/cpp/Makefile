#
# Copyright (C) 2001-2005 by egnite Software GmbH. All rights reserved.
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
# Revision 1.1  2005/01/22 19:38:25  haraldkipp
# Added C++ support contributed by Oliver Schulz (MPI).
#


PROJ = libnutcpp

top_srcdir = ..
top_blddir = ..

LIBDIR = $(top_blddir)/lib
include $(top_srcdir)/UserConf.mk
include $(top_srcdir)/Makedefs

SRCS =  nutcpp.cc

OBJS = $(SRCS:.cc=.o)

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
	-rm -f $(SRCS:.cc=.lst)
	-rm -f $(SRCS:.cc=.lis)
	-rm -f $(SRCS:.cc=.s)
	-rm -f $(SRCS:.cc=.bak)
	-rm -f $(SRCS:.cc=.c~)
