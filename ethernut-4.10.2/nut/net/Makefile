#
# Copyright (C) 2008 by egnite GmbH. All rights reserved.
# Copyright (C) 2001-2004 by egnite Software GmbH. All rights reserved.
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
# Revision 1.12  2008/08/20 07:05:48  haraldkipp
# Added IP demultiplexer.
#
# Revision 1.11  2008/02/15 17:30:00  haraldkipp
# Added missing files. Hopefully not broken after late commit.
#
# Revision 1.10  2007/07/09 16:20:19  olereinhardt
# 2007-07-09  Ole Reinhardt <ole.reinhardt@embedded-it.de>
#         * net/Makefile: Commented in igmp_in.c and igmp_out.c again
#         * net/ipin.c: igmp support reenabled
#
# Revision 1.9  2007/07/09 13:38:50  olereinhardt
# 2007-07-09  Ole Reinhardt <ole.reinhardt@embedded-it.de>
#         * dev/twbbif.c: Added default TWI_DELAY for avr architecture
#         to allow in-tree compilation
#         * net/Makefile: Commented out igmp support as long as it does
#         not exists to allow in-tree compilation
#
# Revision 1.8  2007/05/02 11:32:50  haraldkipp
# IGMP added.
#
# Revision 1.7  2004/11/08 09:57:40  drsung
# Added rule to clean also intermediate files (*.i)
#
# Revision 1.6  2004/09/10 17:18:29  haraldkipp
# Building inside source tree is working again
#
# Revision 1.5  2004/04/07 16:28:23  freckle
# Target install: use ranlib only for arch *nix emulation (ranlib is required
# on mac os x)
#
# Revision 1.4  2004/04/07 12:13:58  haraldkipp
# Matthias Ringwald's *nix emulation added
#
# Revision 1.3  2004/03/18 13:45:57  haraldkipp
# Deprecated functions removed
#
# Revision 1.2  2004/03/16 16:48:45  haraldkipp
# Added Jan Dubiec's H8/300 port.
#
# Revision 1.1.1.1  2003/05/09 14:41:34  haraldkipp
# Initial using 3.2.1
#
#

PROJ = libnutnet

top_srcdir = ..
top_blddir = ..

LIBDIR = $(top_blddir)/lib
include $(top_srcdir)/UserConf.mk
include $(top_srcdir)/Makedefs

SRC1 =  tcpsock.c tcpsm.c tcpin.c tcpout.c tcputil.c \
	udpsock.c udpin.c udpout.c \
	icmpin.c icmpout.c \
	lcpin.c ipcpin.c pppsm.c papin.c papout.c

SRC2 =  ipin.c ipout.c ipcsum.c route.c inet.c \
	netdebug.c pppdebug.c \
	arpin.c arpout.c arpcache.c \
	ethin.c ethout.c ethdemux.c ifconfig.c confnet.c \
	pppin.c pppout.c lcpout.c ipcpout.c \
	igmpin.c igmpout.c ether_addr.c
#       igmp.c

OBJ1 = $(SRC1:.c=.o)
OBJ2 = $(SRC2:.c=.o)

TARG = $(PROJ).a

all: $(TARG)

include $(top_srcdir)/Makerules

$(TARG): $(OBJ1) $(OBJ2)
	$(AR) $(ARFLAGS) $(TARG) $(OBJ1)
	$(AR) $(ARFLAGS) $(TARG) $(OBJ2)

install: $(TARG)
	$(CP) $(TARG) $(LIBDIR)/$(TARG)
ifeq ($(ARCH), unix)
	$(RANLIB) $(LIBDIR)/$(TARG)
endif

clean:
	-rm -f $(TARG)
	-rm -f $(OBJ1) $(OBJ2)
	-rm -f $(SRC1:.c=.lst) $(SRC2:.c=.lst)
	-rm -f $(SRC1:.c=.lis) $(SRC2:.c=.lis)
	-rm -f $(SRC1:.c=.s) $(SRC2:.c=.s)
	-rm -f $(SRC1:.c=.bak) $(SRC2:.c=.bak)
	-rm -f $(SRC1:.c=.c~) $(SRC2:.c=.c~)
	-rm -f $(SRC1:.c=.i) $(SRC2:.c=.i)
