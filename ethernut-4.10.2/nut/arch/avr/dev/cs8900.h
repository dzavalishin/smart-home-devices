#ifndef _CDCS_CS8900_H_
#define _CDCS_CS8900_H_

/*
 * Copyright (C) 2002 by Call Direct Cellular Solutions Pty. Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgement:
 *
 *    This product includes software developed by Call Direct Cellular Solutions Pty. Ltd.
 *    and its contributors.
 *
 * THIS SOFTWARE IS PROVIDED BY CALL DIRECT CELLULAR SOLUTIONS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL CALL DIRECT
 * CELLULAR SOLUTIONS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * For additional information see http://www.calldirect.com.au/
 */
 
/*
 * $Log$
 * Revision 1.1  2005/07/26 18:02:27  haraldkipp
 * Moved from dev.
 *
 * Revision 1.2  2004/05/25 11:39:48  olereinhardt
 * Define NUT_CS8900_OLD to get the old functionality back again
 *
 * Revision 1.1  2004/05/24 17:09:17  olereinhardt
 * Changed base address handling in cs8900.c and moved cs8900.h to /include/dev
 * Base address can now be passed to the nic driver by NutRegisterDevice.
 * Removed some Assembler code in cs8900.c
 *
 * Added some databus waitstate settings for the upper half of the address space in os/arch/avr_nutinit.c. Now three waitstates are default for 0x8000-0xFFFF
 *
 * Added terminal device driver for hd44780 compatible LCD displays directly
 * connected to the memory bus (memory mapped). See hd44780.c for more information.
 * Therefore some minor changed in include/dev/term.h and dev/term.c are needet to
 * pass a base address to the lcd driver.
 *
 * Revision 1.1  2003/07/20 16:37:21  haraldkipp
 * CrystalTek 8900A driver added.
 *
 *
 * Revision 1.0  2002/10/1 MJC CDCS
 * created
 *
 */

// Cirrus Logic CS8900a I/O Registers

#ifdef  NUT_CS8900_OLD

#define	CS_DATA_P0		0x1100
#define	CS_DATA_P1		0x1102
#define	CS_TX_CMD_I		0x1104
#define	CS_TX_LEN_I		0x1106
#define	CS_INT_STAT		0x1108
#define	CS_PP_PTR		0x110A
#define	CS_PP_DATA0		0x110C
#define	CS_PP_DATA1		0x110E

#else

#define	CS_DATA_P0		(cs_base + 0x0000)
#define	CS_DATA_P1		(cs_base + 0x0002)
#define	CS_TX_CMD_I		(cs_base + 0x0004)
#define	CS_TX_LEN_I		(cs_base + 0x0006)
#define	CS_INT_STAT		(cs_base + 0x0008)
#define	CS_PP_PTR		(cs_base + 0x000A)
#define	CS_PP_DATA0		(cs_base + 0x000C)
#define	CS_PP_DATA1		(cs_base + 0x000E)

#endif


// Cirrus Logic CS8900a Packet Page registers
#define	CS_PROD_ID		0x0000
#define	CS_IO_BASE		0x0020
#define	CS_INT_NUM		0x0022
#define	CS_DMA_CHAN		0x0024
#define	CS_DMA_SOF		0x0026
#define	CS_DMA_FCNT		0x0028
#define	CS_DMA_RXCNT	0x002A
#define	CS_MEM_BASE		0x002C
#define	CS_BOOT_BASE	0x0030
#define	CS_BOOT_MASK	0x0034
#define	CS_EE_CMD		0x0040
#define	CS_EE_DATA		0x0042
#define	CS_RX_FRM_CNT	0x0050

#define	CS_ISQ			0x0120
#define	CS_RX_CFG		0x0102
#define	CS_RX_EVENT		0x0124
#define	CS_RX_CTL		0x0104
#define	CS_TX_CFG		0x0106
#define	CS_TX_EVENT		0x0128
#define	CS_TX_CMD_P		0x0108
#define	CS_BUF_CFG		0x010A
#define	CS_BUF_EVENT	0x012C
#define	CS_RX_MISS		0x0130
#define	CS_TX_COLL		0x0132
#define	CS_LINE_CTRL	0x0112
#define	CS_LINE_STAT	0x0134
#define	CS_SELF_CTRL	0x0114
#define	CS_SELF_STAT	0x0136
#define	CS_BUS_CTRL		0x0116
#define	CS_BUS_STAT		0x0138
#define	CS_TEST_CTRL	0x0118
#define	CS_AUI_TDR		0x013C

#define	CS_PP_TX_CMD	0x0144
#define	CS_PP_TX_LEN	0x0146

#define	CS_IEEE_ADDR	0x0158

#define		RESETE		0x80

#endif
