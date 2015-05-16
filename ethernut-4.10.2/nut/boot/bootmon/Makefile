#
# Copyright (C) 2003-2007 by egnite Software GmbH
#
# All rights reserved.
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
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
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
# $Id: Makefile 3422 2011-05-18 09:46:57Z haraldkipp $
#

PROJ = bootmon

top_srcdir = ..

include Makedefs

SRCS =  $(PROJ).c config.c twbbi.c nvmem.c dhcp.c tftp.c udp.c ip.c arp.c ether.c dialog.c uart.c utils.c
LIBS =  

OBJS  = start_rom.o $(SRCS:.c=.o)
TARG  = $(PROJ).hex
BTARG = $(PROJ).bin

all: $(OBJS) $(TARG) $(BTARG)

include Makerules

clean:
	-rm -f $(OBJS)
	-rm -f $(TARG) $(BTARG)
	-rm -f $(PROJ).obj
	-rm -f $(PROJ).sym
	-rm -f $(PROJ).map
	-rm -f start_rom.lst $(SRCS:.c=.lst)
	-rm -f $(SRCS:.c=.bak)
