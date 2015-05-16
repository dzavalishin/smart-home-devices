#ifndef _ARCH_GBA_H_
#define _ARCH_GBA_H_

/*
 * Copyright (C) 2005 by egnite Software GmbH. All rights reserved.
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
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY EGNITE SOFTWARE GMBH AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL EGNITE
 * SOFTWARE GMBH OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * For additional information see http://www.ethernut.de/
 */

/*
 * $Log$
 * Revision 1.1  2005/10/24 10:31:13  haraldkipp
 * Moved from parent directory.
 *
 * Revision 1.2  2005/04/05 17:50:47  haraldkipp
 * Use register names in gba.h.
 *
 * Revision 1.1  2004/11/24 15:26:34  haraldkipp
 * Will fill this later
 *
 */


/*!
 * \brief Internal work RAM
 */
#define WRAM_START      0x03000000      /* Start of internal work RAM */
#define WRAM_END        (WRAM_START + 0x8000)   /* End of internal work RAM */
#define INT_VECTOR      (WRAM_END - 4)  /* Interrupt vector address. */

/*!
 * \brief Display control
 */
#define REG_DISPCNT     0x04000000      /* Display control */
#define REG_STAT        0x04000004      /* Display status */
#define REG_VCOUNT      0x04000006      /* Vertical counter */
#define REG_BG0CNT      0x04000008      /* BG 0 control */
#define REG_BG1CNT      0x0400000a      /* BG 1 control */
#define REG_BG2CNT      0x0400000c      /* BG 2 control */
#define REG_BG3CNT      0x0400000e      /* BG 3 control */
#define REG_BG0HOFS     0x04000010      /* BG 0 horizontal offset */
#define REG_BG0VOFS     0x04000012      /* BG 0 vertical offset */
#define REG_BG1HOFS     0x04000014      /* BG 1 horizontal offset */
#define REG_BG1VOFS     0x04000016      /* BG 1 vertical offset */
#define REG_BG2HOFS     0x04000018      /* BG 2 horizontal offset */
#define REG_BG2VOFS     0x0400001a      /* BG 2 vertical offset */
#define REG_BG3HOFS     0x0400001c      /* BG 3 horizontal offset */
#define REG_BG3VOFS     0x0400001e      /* BG 3 vertical offset */
#define REG_BG2PA       0x04000020      /* BG 2 Rotation/scaling parameter A */
#define REG_BG2PB       0x04000022      /* BG 2 Rotation/scaling parameter B */
#define REG_BG2PC       0x04000024      /* BG 2 Rotation/scaling parameter C */
#define REG_BG2PD       0x04000026      /* BG 2 Rotation/scaling parameter D */
#define REG_BG2X        0x04000028      /* BG 2 Reference point X coordinate */
#define REG_BG2Y        0x0400002c      /* BG 2 Reference point Y coordinate */
#define REG_BG3PA       0x04000030      /* BG 3 Rotation/scaling parameter A */
#define REG_BG3PB       0x04000032      /* BG 3 Rotation/scaling parameter B */
#define REG_BG3PC       0x04000034      /* BG 3 Rotation/scaling parameter C */
#define REG_BG3PD       0x04000036      /* BG 3 Rotation/scaling parameter D */
#define REG_BG3X        0x04000038      /* BG 3 Reference point X coordinate */
#define REG_BG3Y        0x0400003c      /* BG 3 Reference point Y coordinate */
#define REG_WINCNT      0x04000040      /* Window control */
#define REG_WININ       0x04000048      /* Inside window control */
#define REG_WINOUT      0x0400004a      /* Outside window control */
#define REG_MOSAIC      0x0400004c      /* Mosaic size */
#define REG_BLDCNT      0x04000050      /* Blending control */
#define REG_BLDALPHA    0x04000052      /* Alpha blending */
#define REG_BLDY        0x04000054      /* Brightness fading */

/*!
 * \brief Sound control
 */
#define REG_SOUND1CNT   0x04000060      /* Channel 1 control */
#define REG_SOUND2CNT   0x04000068      /* Channel 2 control */
#define REG_SOUND3CNT   0x04000070      /* Channel 3 control */
#define REG_SOUND4CNT   0x04000078      /* Channel 4 control */
#define REG_SOUNDCNT    0x04000080      /* Sound control */
#define REG_SOUNDBIAS   0x04000088      /* Sound PWM control */
#define REG_WAVE_RAM0   0x04000090      /* Channel 3 wave pattern RAM bank 0 */
#define REG_WAVE_RAM1   0x04000094      /* Channel 3 wave pattern RAM bank 1 */
#define REG_WAVE_RAM2   0x04000098      /* Channel 3 wave pattern RAM bank 2 */
#define REG_WAVE_RAM3   0x0400009c      /* Channel 3 wave pattern RAM bank 3 */
#define REG_FIFO_A      0x040000a0      /* Channel A FIFO */
#define REG_FIFO_B      0x040000a4      /* Channel B FIFO */

/*!
 * \brief DMA control
 */
#define REG_DMA0SAD     0x040000b0      /* DMA 0 source address */
#define REG_DMA0DAD     0x040000b4      /* DMA 0 destination address */
#define REG_DMA0CNT     0x040000b8      /* DMA 0 word count */
#define REG_DMA1SAD     0x040000bc      /* DMA 1 source address */
#define REG_DMA1DAD     0x040000c0      /* DMA 1 destination address */
#define REG_DMA1CNT     0x040000c4      /* DMA 1 word count */
#define REG_DMA2SAD     0x040000c8      /* DMA 2 source address */
#define REG_DMA2DAD     0x040000cc      /* DMA 2 destination address */
#define REG_DMA2CNT     0x040000d0      /* DMA 2 word count */
#define REG_DMA3SAD     0x040000d4      /* DMA 3 source address */
#define REG_DMA3DAD     0x040000d8      /* DMA 3 destination address */
#define REG_DMA3CNT     0x040000dc      /* DMA 3 word count */

/*!
 * \brief Timer control
 */
#define REG_TMR0CNT     0x04000100      /* Timer 0 control */
#define REG_TMR1CNT     0x04000104      /* Timer 1 control */
#define REG_TMR2CNT     0x04000108      /* Timer 2 control */
#define REG_TMR3CNT     0x0400010c      /* Timer 3 control */

/*!
 * \brief Serial communication control
 */
#define REG_SIODATA32   0x04000120      /* 32-bit serial data */
#define REG_SIOCNT      0x04000128      /* Serial communication control */
#define REG_SIODATA8    0x0400012a      /* 8-bit serial data */

/*!
 * \brief Keyboard control
 */
#define REG_KEYINPUT    0x04000130      /* Key status */
#define REG_KEYCNT      0x04000132      /* Key control */

/*!
 * \brief General I/O control
 */
#define REG_RCNT        0x04000134      /* General I/O control */

/*!
 * \brief JOY Bus control
 */
#define REG_JOYCNT      0x04000140      /* JOY Bus control */
#define REG_JOYSTAT     0x04000158      /* JOY Bus status */
#define REG_JOY_RECV    0x04000150      /* JOY Bus receive data */
#define REG_JOY_TRANS   0x04000154      /* JOY Bus transmit data */

/*!
 * \brief Interrupt control
 */
#define REG_IE          0x04000200      /* Interrupt enable */
#define REG_IF          0x04000202      /* Interrupt flags */
#define REG_WAITCNT     0x04000204      /* Game Pak wait state control */
#define REG_IME         0x04000208      /* Interrupt master enable */

/*!
 * \brief Interrupt flags.
 */
#define INT_VBLANK      0x0001      /* V blank interrupt flag. */
#define INT_HBLANK      0x0002      /* H blank interrupt flag. */
#define INT_VCOUNT      0x0004      /* V counter interrupt flag. */
#define INT_TMR0        0x0008      /* Timer 0 interrupt flag. */
#define INT_TMR1        0x0010      /* Timer 1 interrupt flag. */
#define INT_TMR2        0x0020      /* Timer 2 interrupt flag. */
#define INT_TMR3        0x0040      /* Timer 3 interrupt flag. */
#define INT_SIO         0x0080      /* Serial communication interrupt flag. */
#define INT_DMA0        0x0100      /* DMA 0 interrupt flag. */
#define INT_DMA1        0x0200      /* DMA 1 interrupt flag. */
#define INT_DMA2        0x0400      /* DMA 2 interrupt flag. */
#define INT_DMA3        0x0800      /* DMA 3 interrupt flag. */
#define INT_KEYPAD      0x1000      /* Key Pad interrupt flag. */
#define INT_GAMEPAK     0x2000      /* Game Pak interrupt flag. */

/*!
 * \brief Power management control
 */
#define REG_HALTCNT     0x04000300      /* Power down control */


#define outw(_reg, _val)    (*((volatile unsigned short *)(_reg)) = (_val))
#define outdw(_reg, _val)   (*((volatile unsigned long *)(_reg)) = (_val))
#define inw(_reg)           (*((volatile unsigned short *)(_reg)))
#define indw(_reg)          (*((volatile unsigned long *)(_reg)))


#define GBAKEY_A        0x0001
#define GBAKEY_B        0x0002
#define GBAKEY_SELECT   0x0003
#define GBAKEY_START    0x0008
#define GBAKEY_RIGHT    0x0010
#define GBAKEY_LEFT     0x0020
#define GBAKEY_UP       0x0040
#define GBAKEY_DOWN     0x0080
#define GBAKEY_R        0x0100
#define GBAKEY_L        0x0200

/*!
 * \brief Timer control.
 */
#define TMR_PRE_64      0x00010000  /* Prescaler 64 */
#define TMR_PRE_256     0x00020000  /* Prescaler 256 */
#define TMR_PRE_1024    0x00030000  /* Prescaler 1024 */
#define TMR_IRQ_ENA     0x00400000  /* Interrupt request enable */
#define TMR_ENA         0x00800000  /* Timer enable. */

/*!
 * \brief SIO control.
 */
#define SIO_BAUD_9600     0x0000
#define SIO_BAUD_38400    0x0001
#define SIO_BAUD_57600    0x0002
#define SIO_BAUD_115200   0x0003
#define SIO_CTS_ENA       0x0004
#define SIO_PARITY_ODD    0x0008
#define SIO_TX_FULL       0x0010
#define SIO_RX_EMPTY      0x0020
#define SIO_ERROR         0x0040
#define SIO_DATA_8BIT     0x0080
#define SIO_FIFO_ENA      0x0100
#define SIO_PARITY_ENA    0x0200
#define SIO_SEND_ENA      0x0400
#define SIO_RECV_ENA      0x0800
#define SIO_MODE_32BIT    0x1000
#define SIO_MODE_MULTI    0x2000
#define SIO_MODE_UART     0x3000
#define SIO_IRQ_ENA       0x4000

#endif
