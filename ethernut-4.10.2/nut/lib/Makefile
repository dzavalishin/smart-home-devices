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
# Revision 1.4  2009/03/07 00:05:23  olereinhardt
# Added gorp directory
#
# Revision 1.3  2006/05/25 09:36:09  haraldkipp
# Include UserConf.mk to determine architecture.
#
# Revision 1.2  2005/07/26 15:44:38  haraldkipp
# Include new libnutarch into the build
#
# Revision 1.1  2005/04/28 14:55:26  haraldkipp
# Moved from top directory because of automake
#
# Revision 1.8  2005/04/04 19:33:35  freckle
# added creation of include/netdb_orig.h, include/sys/socket_orig.h and
# include/netinet/in_orig.h to allow unix emulation to use tcp/ip sockets
#
# Revision 1.7  2005/01/22 19:12:51  haraldkipp
# Added C++ support contributed by Oliver Schulz (MPI).
#
# Revision 1.6  2004/10/22 15:06:22  freckle
# fixed distclean (was deleting the wrong files)
#
# Revision 1.5  2004/10/21 19:17:59  freckle
# added include/errno.h include/fcntl.h to distclean
#
# Revision 1.4  2004/09/10 17:18:25  haraldkipp
# Building inside source tree is working again
#
# Revision 1.3  2004/04/14 18:07:51  freckle
# Target distclean deletes: include/stdio_orig.h and include/sys/types_orig.h
#
# Revision 1.2  2004/03/16 16:48:24  haraldkipp
# Added Jan Dubiec's H8/300 port.
#
# Revision 1.1.1.1  2003/05/09 14:40:23  haraldkipp
# Initial using 3.2.1
#
# Revision 1.13  2003/03/31 14:52:54  harald
# Prepare release 3.1
#
# Revision 1.12  2003/01/14 13:22:44  harald
# Shell no more required
#
# Revision 1.11  2002/11/02 14:26:39  harald
# Application dirs explicitly specified
#
# Revision 1.10  2002/10/31 16:20:40  harald
# Mods by troth for Linux
#
# Revision 1.9  2002/06/26 17:28:58  harald
# First pre-release with 2.4 stack
#
# Revision 1.8  2002/05/08 16:02:25  harald
# First Imagecraft compilation
#
# Revision 1.7  2002/04/20 12:51:45  harald
# Separate application build
#
# Revision 1.6  2001/09/01 18:16:36  harald
# Added new samples
#
# Revision 1.5  2001/06/28 18:30:48  harald
# Preview release
#
# Revision 1.4  2001/06/09 16:38:38  harald
# Refined
#
# Revision 1.3  2001/06/09 10:33:08  harald
# Restructured
#
# Revision 1.2  2001/05/27 20:11:34  harald
# New device structure implemented
#
# Revision 1.1  2001/05/19 12:31:29  harald
# First check in
#
#

top_srcdir = ..
include $(top_srcdir)/UserConf.mk

all:
ifneq (, $(findstring h8300, $(ARCH)))
	$(MAKE) -C $(top_srcdir)/arch/$(ARCH)/init
endif
	$(MAKE) -C $(top_srcdir)/arch
	$(MAKE) -C $(top_srcdir)/os
	$(MAKE) -C $(top_srcdir)/fs
	$(MAKE) -C $(top_srcdir)/dev
	$(MAKE) -C $(top_srcdir)/net
	$(MAKE) -C $(top_srcdir)/pro
	$(MAKE) -C $(top_srcdir)/crt
	$(MAKE) -C $(top_srcdir)/gorp
ifeq ($(ARCH), avr)
	$(MAKE) -C $(top_srcdir)/cpp
endif

install:
ifneq (, $(findstring h8300, $(ARCH)))
	$(MAKE) -C $(top_srcdir)/arch/$(ARCH)/init install
endif
	$(MAKE) -C $(top_srcdir)/arch install
	$(MAKE) -C $(top_srcdir)/os install
	$(MAKE) -C $(top_srcdir)/fs install
	$(MAKE) -C $(top_srcdir)/dev install
	$(MAKE) -C $(top_srcdir)/net install
	$(MAKE) -C $(top_srcdir)/pro install
	$(MAKE) -C $(top_srcdir)/crt install
	$(MAKE) -C $(top_srcdir)/gorp install
ifeq ($(ARCH), avr)
	$(MAKE) -C $(top_srcdir)/cpp install
endif

clean:
ifneq (, $(findstring h8300, $(ARCH)))
	$(MAKE) -C $(top_srcdir)/arch/$(ARCH)/init clean
endif
	$(MAKE) -C $(top_srcdir)/arch clean
	$(MAKE) -C $(top_srcdir)/os clean
	$(MAKE) -C $(top_srcdir)/fs clean
	$(MAKE) -C $(top_srcdir)/dev clean
	$(MAKE) -C $(top_srcdir)/net clean
	$(MAKE) -C $(top_srcdir)/pro clean
	$(MAKE) -C $(top_srcdir)/crt clean
	$(MAKE) -C $(top_srcdir)/cpp clean
	$(MAKE) -C $(top_srcdir)/gorp clean
apps:
	$(MAKE) -C $(top_srcdir)/app

apps-install:
	$(MAKE) -C $(top_srcdir)/app install

cleanall:
ifneq (, $(findstring h8300, $(ARCH)))
	$(MAKE) -C $(top_srcdir)/arch/$(ARCH)/init clean
endif
	$(MAKE) -C $(top_srcdir)/arch clean
	$(MAKE) -C $(top_srcdir)/os clean
	$(MAKE) -C $(top_srcdir)/fs clean
	$(MAKE) -C $(top_srcdir)/dev clean
	$(MAKE) -C $(top_srcdir)/net clean
	$(MAKE) -C $(top_srcdir)/pro clean
	$(MAKE) -C $(top_srcdir)/crt clean
	$(MAKE) -C $(top_srcdir)/gorp clean	
	$(MAKE) -C $(top_srcdir)/cpp clean
	$(MAKE) -C $(top_srcdir)/app clean

distclean:
ifneq (, $(findstring h8300, $(ARCH)))
	$(MAKE) -C $(top_srcdir)/arch/$(ARCH)/init clean
endif
	$(MAKE) -C $(top_srcdir)/arch clean
	$(MAKE) -C $(top_srcdir)/os clean
	$(MAKE) -C $(top_srcdir)/fs clean
	$(MAKE) -C $(top_srcdir)/dev clean
	$(MAKE) -C $(top_srcdir)/net clean
	$(MAKE) -C $(top_srcdir)/pro clean
	$(MAKE) -C $(top_srcdir)/crt clean
	$(MAKE) -C $(top_srcdir)/gorp clean	
	$(MAKE) -C $(top_srcdir)/cpp clean
	$(MAKE) -C $(top_srcdir)/app clean
	cd $(top_srcdir)
	rm -f UserConf.mk Makerules Makedefs app/Makerules app/Makedefs
	rm -f include/stdio_orig.h include/sys/types_orig.h include/errno_orig.h include/fcntl_orig.h
	rm -f include/sys/socket_orig.h include/netdb_orig.h include/netinet/in_orig.h
	rm -f `find . -name '*\.[ch]~'`
