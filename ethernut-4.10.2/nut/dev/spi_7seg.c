/*
 * Copyright (C) 2009 by Ulrich Prinz (uprinz2@netscape.net)
 * Copyright (C) 2009 by Rittal GmbH & Co. KG. All rights reserved.
 *
 * All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
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

/*!
 * \file dev/spi_7seg.c
 * \brief Routines for 7 segment display.
 *
 * \verbatim
 * $Id: spi_7seg.c,$
 * \endverbatim
 */
//#define SPI2SEG_DEBUG

#include <cfg/os.h>
#include <cfg/arch.h>
//#include <cfg/board.h>
#include <compiler.h>
#include <string.h>
#include <stdlib.h>

#ifdef SPI2SEG_DEBUG
#include <stdio.h>
#define NPRINTF(args,...) printf(args,##__VA_ARGS__)
#else
#define NPRINTF(args,...)
#endif


#include <sys/nutdebug.h>
#include <sys/timer.h>

#include <stdio.h>

#include <cfg/memory.h>

#include <dev/blockdev.h>
#include <cfg/spi_7seg.h>
#include <dev/spi_7seg.h>

#include <cfg/arch/gpio.h>
#include <dev/spibus_at91.h>

/* Number of used or connected digits */
#ifndef SEG7_DIGITS
#define SEG7_DIGITS 4
#endif

#define SEG7_REVERSE

/* AS1108 Register Abbreviations */
#define SEGR_NOOP       0x00
#define SEGR_DIG0       0x01
#define SEGR_DIG1       0x02
#define SEGR_DIG2       0x03
#define SEGR_DIG3       0x04
#define SEGR_DIG4       0x05
#define SEGR_DIG5       0x06
#define SEGR_DIG6       0x07
#define SEGR_DIG7       0x08

#define SEGR_DEC_MODE   0x09
#define SEGR_INTENSITY  0x0a
#define SEGR_SCAN_LIM   0x0b
#define SEGR_SHUTDOWN   0x0c
#define SEGR_FEATURE    0x0e
#define SEGR_DSP_TEST   0x0f

/* Shutdown Register (0x0c) */
#define SHUTDOWN_RESET  0x00
#define SHUTDOWN_SOFT   0x80
#define NORM_OP_RESET   0x01
#define NORM_OP_SOFT    0x81

/* Decode Mode Register (0x09) */
#define NO_DIG_DECODE   0x00
#define DIG_0_DECODE    0x01
#define DIG_0_3_DECODE  0x0f
#define DIG_0_7_DECODE  0xff

/* feature register (0x0e) */
#define SEGF_EXTCLK     0x01
#define SEGF_REGRES     0x02
#define SEGF_DECSEL     0x04
#define SEGF_SPIEN      0x08
#define SEGF_BLINK      0x10
#define SEGF_BLSLOW     0x20
#define SEGF_BLSYNC     0x40
#define SEGF_BLSTART    0x80

#define SEGF_BLMASK     (SEGF_BLINK|SEGF_BLSLOW)

/* Test Mode register (0x0f) */
#define TEST_MODE_OFF	0x00
#define TEST_MODE_ON	0x01

/* scan limit register (0x0b) */
#define DISPLAY_LIMIT	2

#ifndef SPI_RATE_DISP_7SEG
#define SPI_RATE_DISP_7SEG  400000
#endif

#ifndef SPI_MODE_DISP_7SEG
#define SPI_MODE_DISP_7SEG SPI_MODE_3
#endif

/* Bitmask for driver control */
#define ICMD_UPDATE     0x01
#define ICMD_INTENS     0x02
#define ICMD_ESCAPE     0x80

/*!
 * \brief Device Control Block for 7-Segment Display
 */
typedef struct {
    uint_fast8_t  digit[SEG7_DIGITS]; /*!< Buffer for Display content */
    uint_fast8_t  dip;        /*!< Actual cursor position */
    uint_fast8_t  freg;      /*!< mirror of internal feature control register */
    uint_fast8_t  icmd;      /*!< internal driver control register */
} DCB_7SEG;

/*********************************************************************************
 **
 **         7-Segement Driver: Character Table
 **
 **/

/* Display Interconnection
 *
 *      --A--
 *     |     |
 *     F     B      Bit:  7 6 5 4 3 2 1 0
 *     +--G--+      Led: dp A B C D E F G
 *     E     C
 *     |     |
 *      --D--  dp
 */
/* 7 Segment Display Character Table */
static CONST uint8_t Seg7CharTab[] = {
	/* ' ' */(0x00),
	/* '!' */(0x28),
	/* '"' */(0x22),
	/* '#' */(0x00),
	/* '$' */(0x5B),
	/* '%' */(0x00),
	/* '&' */(0x6F),
	/* '´' */(0x20),
	/* '(' */(0x4E),
	/* ')' */(0x78),
	/* '*' */(0x00),
	/* '+' */(0x31),
	/* ''' */(0x20),
	/* '-' */(0x01),
	/* '.' */(0x01),
	/* '/' */(0x15),
	/* '0' */(0x7E),
	/* '1' */(0x30),
	/* '2' */(0x6D),
	/* '3' */(0x79),
	/* '4' */(0x33),
	/* '5' */(0x5B),
	/* '6' */(0x5F),
	/* '7' */(0x70),
	/* '8' */(0x7F),
	/* '9' */(0x7B),
	/* ':' */(0x00),
	/* ';' */(0x00),
	/* '<' */(0x00),
	/* '=' */(0x09),
	/* '>' */(0x00),
	/* '?' */(0x65),
	/* '@' */(0x00),
	/* 'A' */(0x77),
	/* 'b' */(0x1F),
	/* 'c' */(0x0D),
	/* 'd' */(0x3D),
	/* 'E' */(0x4F),
	/* 'F' */(0x47),
	/* 'G' */(0x5F),
	/* 'H' */(0x37),
	/* 'i' */(0x10),
	/* 'J' */(0x3C),
	/* 'K' */(0x0F),
	/* 'L' */(0x0E),
	/* 'M' */(0x76),
	/* 'N' */(0x15),
	/* 'O' */(0x1D),
	/* 'P' */(0x67),
	/* 'Q' */(0x73),
	/* 'R' */(0x05),
	/* 'S' */(0x5B),
	/* 'T' */(0x0F),
	/* 'U' */(0x3E),
	/* 'V' */(0x1C),
	/* 'W' */(0x3F),
	/* 'X' */(0x37),
	/* 'Y' */(0x3B),
	/* 'Z' */(0x6D),
	/* '[' */(0x4E),
	/* '\' */(0x13),
	/* ']' */(0x78),
	/* '^' */(0x42),
	/* '_' */(0x01),
};

/*********************************************************************************
 **
 **         7-Segement Driver: Low Level Communication
 **
 **/

/*!
 * \brief Send 7seg command.
 *
 * \param node  Specifies the SPI node.
 * \param addr  Optional command parameter.
 * \param txbuf Pointer to the transmit data buffer, may be set to NULL.
 * \param rxbuf Pointer to the receive data buffer, may be set to NULL.
 * \param xlen  Number of byte to receive and/or transmit.
 */
static int disp7segCommand(NUTSPINODE * node, uint8_t addr, CONST void *txbuf, void *rxbuf, int xlen)
{
    int rc = -1;
    NUTSPIBUS *bus;
    uint8_t *tmp;
	uint8_t cmd[2];

    NUTASSERT(node != NULL);
    bus = (NUTSPIBUS *) node->node_bus;
    NUTASSERT(bus != NULL);
    NUTASSERT(bus->bus_alloc != NULL);
    NUTASSERT(bus->bus_transfer != NULL);
    NUTASSERT(bus->bus_release != NULL);

	/* write address */
	 cmd[0] = addr;

	tmp = (uint8_t *)txbuf;
	/* write data */
	 cmd[1] = tmp[0];
	//cmd[1] = (uint8_t )txbuf[0];
	/* write data */
    rc = (*bus->bus_alloc) (node, 1000);
    if (rc == 0) {
        rc = (*bus->bus_transfer) (node, cmd, NULL, 2);

        (*bus->bus_release) (node);
    }
    return rc;
}

/*!
 * \brief Push display buffer to display.
 *
 * \param dev  Display device.
 */
void Spi7SegPush( NUTDEVICE * dev)
{
	uint_fast8_t i;
    DCB_7SEG * dcb;
	NUTSPINODE *node;

    NUTASSERT(dev->dev_dcb != NULL);
    NUTASSERT(dev->dev_icb != NULL);

    dcb  = dev->dev_dcb;
    node = dev->dev_icb;

#ifdef SEG7_REVERSE
    NPRINTF("<");
	for (i=0;i<SEG7_DIGITS;i++)
	{
        /* Display is connected reverse... */
		disp7segCommand( node, SEG7_DIGITS-i, &(dcb->digit[i]), NULL, 1);
        NPRINTF(" %02x",dcb->digit[i]);
	}
    NPRINTF(">\n");
#else
	disp7segCommand(node, 0, dcb->digit, NULL, SEG7_DIGITS);
#endif
}

/*********************************************************************************
 **
 **         7-Segement Driver: Character Driver
 **
 **/

/*!
 * \brief Send single character to 7-Segment Display.
 *
 * A newline character will reset write pointer to digit 0.
 * Carriage return is ignored.
 *
 * \return Number of characters sent.
 */
int Spi7segPutc(NUTDEVICE * dev, char c)
{
    DCB_7SEG * dcb;
	NUTSPINODE *node;

    NUTASSERT(dev->dev_dcb != NULL);
    NUTASSERT(dev->dev_icb != NULL);

    dcb  = dev->dev_dcb;
    node = dev->dev_icb;

    NPRINTF("[%c]", c);

    if( dcb->icmd & ICMD_ESCAPE) {

        dcb->icmd &= ~ICMD_ESCAPE;
        /* Handle ESC sequences */
        switch( c) {
            case 'b':       /* blink slow */
                dcb->freg |= (SEGF_BLINK | SEGF_BLSLOW);
                disp7segCommand( node, SEGR_FEATURE, &dcb->freg, NULL, 1);
                return 0;
                break;
            case 'f':       /* blink fast */
                dcb->freg &= ~SEGF_BLMASK;
                dcb->freg |= SEGF_BLINK ;
                disp7segCommand( node, SEGR_FEATURE, &dcb->freg, NULL, 1);
                return 0;
                break;
            case 'n':       /* stop blinking */
                dcb->freg &= ~SEGF_BLMASK;
                disp7segCommand( node, SEGR_FEATURE, &dcb->freg, NULL, 1);
                return 0;
                break;
            case 'h':       /* home */
                dcb->dip = 0;
                return 0;
                break;
            case 'c':       /* clear */
                memset( dcb->digit, 0, SEG7_DIGITS);
                dcb->icmd |= ICMD_UPDATE;
                break;
            case 't':       /* test, all digits on */
                memset( dcb->digit, 0xFF, SEG7_DIGITS);
                dcb->icmd |= ICMD_UPDATE;
                break;
            case 'i':       /* intensity control */
                dcb->icmd |= ICMD_INTENS;
                return 0;
                break;
            default:
                break;
        }

    }
    else {
        /* Non-ESC Character incoming */

        /* Start ESC Sequence? */
        if( c == 0x1b) {
            dcb->icmd |= ICMD_ESCAPE;
            return 0;
        }

        /* Complete Intensity command */
        if( dcb->icmd & ICMD_INTENS) {
            dcb->icmd &= ~ICMD_INTENS;
            c = (c>'9')?(c-'A'+10):(c-'0');
            disp7segCommand( node, SEGR_INTENSITY, &c, NULL, 1);
            return 0;
        }

        if( c == '\n' ) {  /* Return to Digit 0 */
            dcb->dip = 0;
            return 0;
        }

        /* Everything from here down, needs update of display */

        if( c == '.') {
            /* Add decimal point to previous digit */
            if( dcb->dip > 0) dcb->digit[dcb->dip-1] |= 0x80;
            dcb->icmd |= ICMD_UPDATE;
        }
        else if( (c >= ' ') && ( dcb->dip < SEG7_DIGITS)) {
            if( c > 0x5F) c -= 0x20;  /* convert lower case to upper case */
            /* Print incoming character */
            dcb->digit[ dcb->dip++] = Seg7CharTab[(c&0xff)-' '];
            dcb->icmd |= ICMD_UPDATE;
        }
    }

    if( dcb->dip > SEG7_DIGITS) dcb->dip = SEG7_DIGITS;

    if( dcb->icmd & ICMD_UPDATE)
    {
        dcb->icmd &= ~ICMD_UPDATE;
		Spi7SegPush( dev);
    }
        return 0;
}

/*********************************************************************************
 **
 **         7-Segement Driver: File Device Handling
 **
 **/

/*!
 * \brief Handle I/O controls for 7-Segment Display.
 *
 * The 7-Seg Display is controlled by ESC-Sequences only.
 *
 * \return 0.
 */
static int Spi7SegIOCtl(NUTDEVICE * dev, int req, void *conf)
{
    return 0;
}

/*!
 * \brief Send characters to 7-Segment Display.
 *
 * A newline character will reset write pointer to digit 0.
 * Carriage return is ignored.
 *
 * \return Number of characters sent.
 */
int Spi7segWrite(NUTFILE * fp, CONST void *buffer, int len)
{
    int i=len;
    CONST char *cp = buffer;

    NUTASSERT(fp->nf_dev != NULL);

    while(i--)
    {
        Spi7segPutc( fp->nf_dev, *cp++);
    }
    return len;
}

/*!
 * \brief File Handle for 7-Sefgment Display.
 *
 * \param pos Selects the digit.
 * \param act Selects the action for the digits decimal dot.
 *
 */
void Spi7segDot(NUTDEVICE * dev, uint8_t pos, uint8_t act)
{
    DCB_7SEG * dcb;

    NUTASSERT(dev->dev_dcb != NULL);

    dcb  = dev->dev_dcb;

	if( pos < SEG7_DIGITS)
	{
		switch( act)
		{
			case DOT_7SEG_FLIP:	dcb->digit[pos] ^= 0x80; break;
			case DOT_7SEG_SET:	dcb->digit[pos] |= 0x80; break;
			case DOT_7SEG_CLR:	dcb->digit[pos] &= ~0x80; break;
		}
		Spi7SegPush( dev);
	}

}

/*!
 * \brief Generate File Handle for 7-Sefgment Display.
 *
 * \param dev Specifies the 7seg device.
 *
 * \return 0 on success or -1 if no valid 7seg was found.
 */
NUTFILE *Spi7SegOpen(NUTDEVICE * dev, CONST char *name, int mode, int acc)
{
    NUTFILE *fp;

    NUTASSERT( dev != NULL);

    if ((fp = malloc(sizeof(NUTFILE))) == 0) {
        return NUTFILE_EOF;
    }

    fp->nf_fcb = 0;
    fp->nf_dev = dev;
    fp->nf_next = 0;

    return fp;

}

/*!
 * \brief Close 7-Segment Device.
 *
 * \return 0 if closed and was opened before, else -1.
 */
static int Spi7SegClose(NUTFILE * fp)
{
    if( fp != NULL) {
        free( fp);
        return 0;
    }
    return -1;
}

/*********************************************************************************
 **
 **         7-Segement Driver: Initialization & Device Description
 **
 **/

/*!
 * \brief Initialize the 7seg device.
 *
 * This routine determines the 7seg type. It is internally called
 * by Nut/OS during device registration.
 *
 * The driver framework may call this function more than once.
 *
 * \param dev Specifies the 7seg device.
 *
 * \return 0 on success or -1 if no valid 7seg was found.
 */
int Spi7segInit(NUTDEVICE * dev)
{
	uint8_t data;
    NUTSPINODE *node;
    DCB_7SEG * dcb;

    NUTASSERT(dev != NULL);
    NUTASSERT(dev->dev_icb != NULL);
    node = dev->dev_icb;

    /* Allocate device control block */
    dcb = malloc( sizeof( DCB_7SEG));
    if( dcb == NULL)
        return -1;
    memset( dcb, 0, sizeof( DCB_7SEG));
    dev->dev_dcb = dcb;

    NPRINTF("INIT %d Digits...\n", SEG7_DIGITS);

	data = TEST_MODE_OFF;
	disp7segCommand(node, SEGR_DSP_TEST, &data, NULL, 1);
	disp7segCommand(node, SEGR_DSP_TEST, &data, NULL, 1);
	disp7segCommand(node, SEGR_DSP_TEST, &data, NULL, 1);

	data = NORM_OP_RESET;
	disp7segCommand(node, SEGR_SHUTDOWN, &data, NULL, 1);

	data = SEG7_DIGITS;
	disp7segCommand(node, SEGR_SCAN_LIM, &data, NULL, 1);

	dcb->freg = NO_DIG_DECODE;
	disp7segCommand(node, SEGR_DEC_MODE, &dcb->freg, NULL, 1);

	data = 0x0F;
	disp7segCommand(node, SEGR_INTENSITY, &data, NULL, 1);

	data = 0;
	disp7segCommand(node, SEGR_DIG0, &data, NULL, 1);
	disp7segCommand(node, SEGR_DIG1, &data, NULL, 1);
	disp7segCommand(node, SEGR_DIG2, &data, NULL, 1);

	return 0;

}

/*!
 * \brief 7seg SPI node implementation structure.
 */
NUTSPINODE nodeSpi7SEG = {
    NULL,                   /*!< \brief Pointer to the bus controller driver, node_bus. */
    NULL,                   /*!< \brief Pointer to device driver specific settings, node_stat. */
    SPI_RATE_DISP_7SEG,     /*!< \brief Initial clock rate, node_rate. */
    SPI_MODE_DISP_7SEG,     /*!< \brief Initial mode, node_mode. */
    8,                      /*!< \brief Initial data bits, node_bits. */
    0                       /*!< \brief Chip select, node_cs. */
};

/*!
 * \brief 7seg device implementation structure.
 */
NUTDEVICE devSpi7SEG = {
    NULL,                       	/*!< \brief Pointer to next device, dev_next. */
    {'7', 'S', 'E', 'G', 0, 0, 0},  /*!< \brief Unique device name, dev_name. */
    IFTYP_CHAR,                   	/*!< \brief Type of device, dev_type. */
    0,                          	/*!< \brief Base address, dev_base (not used). */
    0,                         		/*!< \brief First interrupt number, dev_irq (not used). */
    &nodeSpi7SEG,             		/*!< \brief Interface control block, dev_icb. */
    0,                              /*!< \brief Driver control block, dev_dcb. */
    Spi7segInit,               		/*!< \brief Driver initialization routine, dev_init. */
    Spi7SegIOCtl,                   /*!< \brief Driver specific control function, dev_ioctl. */
	0,								/*!< \brief Read from device, dev_read. */
    Spi7segWrite,                   /*!< \brief Write to device, dev_write. */
#ifdef __HARVARD_ARCH__
    0,								/*!< \brief Write data from program space to device, dev_write_P. */
#endif
    Spi7SegOpen,    				/*!< \brief Mount volume, dev_open. */
    Spi7SegClose,   				/*!< \brief Unmount volume, dev_close. */
    0 								/*!< \brief Request file size, dev_size. */
};

