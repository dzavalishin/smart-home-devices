#
# Copyright (C) 2008 by egnite GmbH. All rights reserved.
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
# Revision 1.15  2009/01/04 20:48:53  olereinhardt
# 2009-01-04  Ole Reinhardt <ole.reinhardt@thermotemp.de>
#
# 	* pro/Makefile: Fixed Makefile. Thanks to Uwe Bonnes for this patch
#
# Revision 1.14  2008/05/16 03:38:27  thiagocorrea
# Revert httpd memory allocation calls to NutHeapAlloc for consistency and
# move DestroyRequestInfo to a shared file (reduces code size and remove duplicates
# from httpd.c and ssi.c)
#
# Revision 1.13  2008/04/01 10:11:35  haraldkipp
# Added the new, enhanced httpd API library.
# Bugs #1839026 and #1839029 fixed.
#
# Revision 1.12  2008/02/15 17:30:00  haraldkipp
# Added missing files. Hopefully not broken after late commit.
#
# Revision 1.11  2007/07/16 15:52:31  haraldkipp
# Missing discovery support added.
#
# Revision 1.10  2005/08/05 11:23:11  olereinhardt
# Added support to register a custom handler for mime types.
# Added Server side include support and ASP support.
#
# Revision 1.9  2005/02/05 20:44:49  haraldkipp
# Wins and FTP added.
#
# Revision 1.8  2004/11/08 09:57:40  drsung
# Added rule to clean also intermediate files (*.i)
#
# Revision 1.7  2004/09/19 11:18:45  haraldkipp
# Syslog client added
#
# Revision 1.6  2004/09/10 17:18:30  haraldkipp
# Building inside source tree is working again
#
# Revision 1.5  2004/04/07 16:28:24  freckle
# Target install: use ranlib only for arch *nix emulation (ranlib is required
# on mac os x)
#
# Revision 1.4  2004/04/07 12:14:00  haraldkipp
# Matthias Ringwald's *nix emulation added
#
# Revision 1.3  2004/03/16 16:48:46  haraldkipp
# Added Jan Dubiec's H8/300 port.
#
# Revision 1.2  2003/11/24 18:12:40  drsung
# Added sntp.c to sources
#
# Revision 1.1.1.1  2003/05/09 14:41:58  haraldkipp
# Initial using 3.2.1
#
# Revision 1.7  2003/02/04 18:18:46  harald
# Clean up ICC leftovers
#
# Revision 1.6  2002/10/31 16:33:34  harald
# Mods by troth for Linux
#
# Revision 1.5  2002/06/26 17:29:48  harald
# First pre-release with 2.4 stack
#
# Revision 1.4  2002/06/04 18:39:18  harald
# New make environment
#
# Revision 1.3  2002/05/08 16:03:20  harald
# First Imagecraft compilation
#
# Revision 1.2  2001/08/10 18:16:53  harald
# GCC version 3 update
#
# Revision 1.1  2001/06/09 10:36:06  harald
# First check in
#
#

PROJ = libnutpro

top_srcdir = ..
top_blddir = ..

LIBDIR = $(top_blddir)/lib
include $(top_srcdir)/UserConf.mk
include $(top_srcdir)/Makedefs

SRCS =  dhcpc.c resolv.c httpd.c httpd_p.c httpopt.c ssi.c asp.c auth.c cgi.c dencode.c sntp.c syslog.c ftpd.c \
	wins.c discover.c snmp.c snmp_agent.c snmp_api.c snmp_auth.c snmp_config.c snmp_mib.c asn1.c

OBJS = $(SRCS:.c=.o)

TARG = $(PROJ).a

all: $(TARG)

include $(top_srcdir)/Makerules

$(TARG): $(OBJS)
	$(AR) $(ARFLAGS) $(TARG) $(OBJS)

install: $(TARG)
	$(CP)     $(TARG) $(LIBDIR)/$(TARG)
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
