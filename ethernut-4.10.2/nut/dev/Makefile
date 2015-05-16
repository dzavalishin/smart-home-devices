#
# Copyright (C) 2008 by egnite GmbH. All rights reserved.
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
# Revision 1.28  2008/02/15 17:30:00  haraldkipp
# Added missing files. Hopefully not broken after late commit.
#
# Revision 1.27  2007/07/16 15:49:53  haraldkipp
# RTC support was missing.
#
# Revision 1.26  2006/06/30 22:06:56  christianwelzel
# DS1307 RTC Driver added.
#
# Revision 1.25  2006/06/18 16:33:59  haraldkipp
# Watchdog module added.
#
# Revision 1.24  2006/05/25 09:28:54  haraldkipp
# Latest sources added, so building in the source tree will create complete
# libraries.
#
# Revision 1.23  2006/02/02 11:30:48  olereinhardt
# Added nvmem.c to target list
#
# Revision 1.22  2005/09/17 09:23:57  drsung
# irqstack.c also moved to arch
#
# Revision 1.21  2005/07/26 16:08:26  haraldkipp
# Target dependent modules moved to arch.
#
# Revision 1.20  2005/02/10 07:06:18  hwmaier
# Changes to incorporate support for AT90CAN128 CPU
#
# Revision 1.19  2005/01/22 19:23:21  haraldkipp
# Removed uartspi devices.
#
# Revision 1.18  2004/11/08 09:57:39  drsung
# Added rule to clean also intermediate files (*.i)
#
# Revision 1.17  2004/09/16 11:54:48  olereinhardt
# Added hd44780_bus.c again as someone removed it??? Any reasons?
#
# Revision 1.16  2004/09/10 17:18:29  haraldkipp
# Building inside source tree is working again
#
# Revision 1.15  2004/08/02 10:04:26  olereinhardt
# Added adc.c to Makefile
#
# Revision 1.14  2004/06/07 15:10:34  olereinhardt
# Added can_dev.c and sja1000.c
#
# Revision 1.13  2004/05/24 17:09:17  olereinhardt
# Changed base address handling in cs8900.c and moved cs8900.h to /include/dev
# Base address can now be passed to the nic driver by NutRegisterDevice.
# Removed some Assembler code in cs8900.c
#
# Added some databus waitstate settings for the upper half of the address space in os/arch/avr_nutinit.c. Now three waitstates are default for 0x8000-0xFFFF
#
# Added terminal device driver for hd44780 compatible LCD displays directly
# connected to the memory bus (memory mapped). See hd44780.c for more information.
# Therefore some minor changed in include/dev/term.h and dev/term.c are needet to
# pass a base address to the lcd driver.
#
# Revision 1.12  2004/04/07 16:28:22  freckle
# Target install: use ranlib only for arch *nix emulation (ranlib is required
# on mac os x)
#
# Revision 1.11  2004/04/07 12:13:56  haraldkipp
# Matthias Ringwald's *nix emulation added
#
# Revision 1.10  2004/03/18 19:59:26  haraldkipp
# WLAN preview added
#
# Revision 1.9  2004/03/16 16:48:27  haraldkipp
# Added Jan Dubiec's H8/300 port.
#
# Revision 1.8  2004/03/08 11:16:54  haraldkipp
# Asynchronous HDLC driver added
#
# Revision 1.7  2004/01/30 17:02:19  drsung
# Separate interrupt stack for avr-gcc only added.
#
# Revision 1.6  2003/12/15 19:24:49  haraldkipp
# New USART driver added
#
# Revision 1.5  2003/11/23 15:05:21  drsung
# irqreg.c removed from source list
#
# Revision 1.4  2003/10/13 10:13:49  haraldkipp
# First release
#
# Revision 1.3  2003/07/21 18:07:54  haraldkipp
# Sony infrared remote control driver added
#
# Revision 1.2  2003/07/20 16:37:21  haraldkipp
# CrystalTek 8900A driver added.
#
# Revision 1.1.1.1  2003/05/09 14:40:45  haraldkipp
# Initial using 3.2.1
#
# Revision 1.12  2003/05/06 18:27:02  harald
# Cleanup
#
# Revision 1.11  2003/04/07 12:20:02  harald
# Debug devices added
#
# Revision 1.10  2003/03/31 14:53:05  harald
# Prepare release 3.1
#
# Revision 1.9  2003/02/04 17:50:54  harald
# Version 3 released
#
# Revision 1.8  2003/01/14 16:28:03  harald
# *** empty log message ***
#
# Revision 1.7  2002/11/02 15:14:54  harald
# Adapted to configure tool
#
# Revision 1.6  2002/10/29 12:44:36  harald
# PPP support added
#
# Revision 1.5  2002/08/16 17:56:29  harald
# Fixed: Second UART not included
#
# Revision 1.4  2002/06/26 17:29:08  harald
# First pre-release with 2.4 stack
#
# Revision 1.3  2002/06/04 18:39:53  harald
# New make environment
#
# Revision 1.2  2001/12/04 15:52:14  harald
# Added uartspi support
#
# Revision 1.1  2001/05/27 20:05:38  harald
# New device structure implemented
#
#
#

PROJ = libnutdev

top_srcdir = ..
top_blddir = ..

LIBDIR = $(top_blddir)/lib
include $(top_srcdir)/UserConf.mk
include $(top_srcdir)/Makedefs

SRCS = ihndlr.c can_dev.c usart.c null.c chat.c term.c netbuf.c ppp.c nvmem.c \
       twbbif.c mmcard.c x12rtc.c cy2239x.c npl.c nplmmc.c watchdog.c \
       rtc.c ds1307rtc.c at49bv.c avrtarget.c sbbif0.c pcf8563.c \
       pca9555.c usart0sc16is752.c usartsc16is752.c spi_7seg.c led.c keys.c

ifeq ($(DEVICE), AT91R40008)
SRCS += npluled.c sbimmc.c
endif

ifeq ($(DEVICE), AT91SAM7X256)
SRCS += at45db.c
endif

ifeq ($(DEVICE), AT91SAM9260)
SRCS += at45db.c
endif

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
