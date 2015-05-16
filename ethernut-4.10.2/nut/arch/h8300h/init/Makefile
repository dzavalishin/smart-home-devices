#
# Copyright (C) 2004 by Jan Dubiec. All rights reserved.
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
# 3. Neither the name of the copyright holders nor the names of
#    contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY JAN DUBIEC AND CONTRIBUTORS
# ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL JAN DUBIEC
# OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

#
# $Log: Makefile,v $
# Revision 1.1  2004/03/16 16:48:26  haraldkipp
# Added Jan Dubiec's H8/300 port.
#
#

PROJ = libnutentry

top_srcdir = ../../..

include $(top_srcdir)/Makedefs

SRC1 = 	nutentry.S
SRC2 = 	nuthwinit.c

OBJ1 = $(SRC1:.S=.o)
OBJ2 = $(SRC2:.c=.o)

TRG = $(PROJ).a

all: $(OBJ1) $(OBJ2) $(TRG)

include $(top_srcdir)/Makerules

$(TRG): $(OBJ1) $(OBJ2)
	$(AR) $(ARFLAGS) $(TRG) $(OBJ1) $(OBJ2)

install: $(TRG)
	$(CP) $(TRG) $(LIBDIR)/$(TRG)

clean:
	-rm -f $(OBJ1) $(OBJ2) $(TRG)
	-rm -f $(SRC1:.S=.lst) $(SRC2:.c=.lst)
	-rm -f $(SRC1:.S=.lis) $(SRC2:.c=.lis)
	-rm -f $(SRC1:.S=.s) $(SRC2:.c=.s)
	-rm -f $(SRC1:.S=.bak) $(SRC2:.c=.bak)
	-rm -f $(SRC1:.S=.S~) $(SRC2:.c=.c~)
