#
# Copyright (C) 2005 by egnite Software GmbH. All rights reserved.
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

## Process this file with automake to generate Makefile.in

#
# $Log$
# Revision 1.5  2009/03/08 20:02:38  haraldkipp
# Added gorp library.
#
# Revision 1.4  2009/02/17 14:46:11  haraldkipp
# Added lua and removed mod.
#
# Revision 1.3  2008/01/31 15:33:33  haraldkipp
# Linux fixes.
#
# Revision 1.2  2007/08/29 15:05:27  haraldkipp
# New directory for bootloaders.
#
# Revision 1.1  2005/04/28 14:43:43  haraldkipp
# First check in
#
#

MAINTAINERCLEANFILES = depcomp compile missing install-sh

EXTRA_DIST = nut/nutsetup reconf \
nut/Makevars.all nut/Makerules.all \
nut/Makevars.arm-ecross-gcc \
nut/Makedefs.arm-ecross-gcc nut/Makerules.arm-ecross-gcc \
nut/Makedefs.arm-ecross-gccdbg nut/Makerules.arm-ecross-gccdbg \
nut/Makevars.gcc \
nut/Makevars.arm-gcc \
nut/Makedefs.arm-gcc nut/Makerules.arm-gcc \
nut/Makedefs.arm-gccdbg nut/Makerules.arm-gccdbg \
nut/Makevars.avr-gcc \
nut/Makedefs.avr-gcc nut/Makerules.avr-gcc \
nut/Makedefs.avr-gccdbg nut/Makerules.avr-gccdbg \
nut/Makevars.avr32-gcc \
nut/Makedefs.avr32-gcc nut/Makerules.avr32-gcc \
nut/Makedefs.avr32-gccdbg nut/Makerules.avr32-gccdbg \
nut/Makerules.gcc \
nut/Makedefs.h8-gcc nut/Makerules.h8-gcc \
nut/Makedefs.unix-gcc nut/Makerules.unix-gcc \
ChangeLog20090309 nut/tools/packaging/distcheck.lua

SUBDIRS = nut/doc nut/tools/linux

if ENABLE_CRUROM
SUBDIRS += nut/tools/crurom 
endif

if ENABLE_NUTCONF
SUBDIRS += nut/tools/nutconf
endif

if ENABLE_QNUTCONF
SUBDIRS += nut/tools/qnutconf 
endif

if ENABLE_NUTDISC
SUBDIRS += nut/tools/nutdisc
endif

dist-hook:
	mkdir -p $(distdir)/nut/app
	cp -R $(srcdir)/nut/app/* $(distdir)/nut/app
	mkdir -p $(distdir)/nut/arch
	cp -R $(srcdir)/nut/arch/* $(distdir)/nut/arch
	mkdir -p $(distdir)/nut/bin
	cp -R $(srcdir)/nut/bin/* $(distdir)/nut/bin
	mkdir -p $(distdir)/nut/boot
	cp -R $(srcdir)/nut/boot/* $(distdir)/nut/boot
	mkdir -p $(distdir)/nut/c
	cp -R $(srcdir)/nut/c/* $(distdir)/nut/c
	mkdir -p $(distdir)/nut/conf
	cp -R $(srcdir)/nut/conf/* $(distdir)/nut/conf
	mkdir -p $(distdir)/nut/cpp
	cp -R $(srcdir)/nut/cpp/* $(distdir)/nut/cpp
	mkdir -p $(distdir)/nut/crt
	cp -R $(srcdir)/nut/crt/* $(distdir)/nut/crt
	mkdir -p $(distdir)/nut/dev
	cp -R $(srcdir)/nut/dev/* $(distdir)/nut/dev
	mkdir -p $(distdir)/nut/fs
	cp -R $(srcdir)/nut/fs/* $(distdir)/nut/fs
	mkdir -p $(distdir)/nut/gorp
	cp -R $(srcdir)/nut/gorp/* $(distdir)/nut/gorp
	mkdir -p $(distdir)/nut/include
	cp -R $(srcdir)/nut/include/* $(distdir)/nut/include
	mkdir -p $(distdir)/nut/lib
	cp -R $(srcdir)/nut/lib/* $(distdir)/nut/lib
	mkdir -p $(distdir)/nut/lua
	cp -R $(srcdir)/nut/lua/* $(distdir)/nut/lua
	mkdir -p $(distdir)/nut/net
	cp -R $(srcdir)/nut/net/* $(distdir)/nut/net
	mkdir -p $(distdir)/nut/os
	cp -R $(srcdir)/nut/os/* $(distdir)/nut/os
	mkdir -p $(distdir)/nut/pro
	cp -R $(srcdir)/nut/pro/* $(distdir)/nut/pro
	rm -rf `find $(distdir) -name .svn`

