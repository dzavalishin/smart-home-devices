#
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
# Revision 1.3  2004/08/18 15:02:34  haraldkipp
# Application build is no longer fixed in top_srcdir
#
# Revision 1.2  2003/11/04 17:46:52  haraldkipp
# Adapted to Ethernut 2
#
# Revision 1.1  2003/05/15 14:30:09  haraldkipp
# Initial using 3.2.1
#
# Revision 1.3  2003/03/31 14:52:56  harald
# Prepare release 3.1
#
# Revision 1.2  2003/01/14 13:22:52  harald
# Shell no more required
#
# Revision 1.1  2002/10/31 18:13:20  harald
# Added by troth
#
#

all:
	$(MAKE) -C basemon
	$(MAKE) -C httpd
	$(MAKE) -C inetq
	$(MAKE) -C nutpiper
	$(MAKE) -C portdio
	$(MAKE) -C pppc
	$(MAKE) -C rs232d
	$(MAKE) -C simple
	$(MAKE) -C tcps
	$(MAKE) -C threads
	$(MAKE) -C timers
	$(MAKE) -C uart

install:
	$(MAKE) -C basemon install
	$(MAKE) -C httpd install
	$(MAKE) -C inetq install
	$(MAKE) -C nutpiper install
	$(MAKE) -C portdio install
	$(MAKE) -C pppc install
	$(MAKE) -C rs232d install
	$(MAKE) -C simple install
	$(MAKE) -C tcps install
	$(MAKE) -C threads install
	$(MAKE) -C timers install
	$(MAKE) -C uart install

clean:
	$(MAKE) -C basemon clean
	$(MAKE) -C httpd clean
	$(MAKE) -C inetq clean
	$(MAKE) -C nutpiper clean
	$(MAKE) -C portdio clean
	$(MAKE) -C pppc clean
	$(MAKE) -C rs232d clean
	$(MAKE) -C simple clean
	$(MAKE) -C tcps clean
	$(MAKE) -C threads clean
	$(MAKE) -C timers clean
	$(MAKE) -C uart clean
