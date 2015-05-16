/*
 * Copyright (C) 2003 by Pavel Chromy. All rights reserved.
 * Copyright (C) 2001-2006 by egnite Software GmbH. All rights reserved.
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
 * -
 *
 * This software has been inspired by all the valuable work done by
 * Jesper Hansen <jesperh@telia.com>. Many thanks for all his help.
 */

/*
 * $Log$
 * Revision 1.4  2008/08/11 06:59:18  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.3  2006/05/15 11:46:00  haraldkipp
 * Bug corrected, which stopped player on flush. Now flushing plays
 * the remaining bytes in the buffer.
 * VS1001 ports are now fully configurable.
 * Several changes had been added to adapt the code to newer
 * Nut/OS style, like replacing outp with outb and using API
 * routines for interrupt control.
 *
 * Revision 1.2  2006/01/23 19:52:10  haraldkipp
 * Added required typecasts before left shift.
 *
 * Revision 1.1  2005/07/26 18:02:40  haraldkipp
 * Moved from dev.
 *
 * Revision 1.3  2004/03/16 16:48:27  haraldkipp
 * Added Jan Dubiec's H8/300 port.
 *
 * Revision 1.2  2003/07/21 18:06:34  haraldkipp
 * Buffer function removed. The driver is now using the banked memory routines.
 * New functions allows the application to enable/disable decoder interrupts.
 *
 * Revision 1.1.1.1  2003/05/09 14:40:58  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.12  2003/05/06 18:35:21  harald
 * ICCAVR port
 *
 * Revision 1.11  2003/04/21 16:43:54  harald
 * Added more comments.
 * Avoid initializing static globals to zero.
 * New function VsSdiWrite/_P checks DREQ
 * Removed decoder interrupt en/disable from low level routines.
 * Keep decoder in reset state until ports have been initialized.
 * Do not send initial zero bytes as the datasheet recommends.
 * A single nop is sufficient delay during reset active.
 * Clear interrupt flag after reset to avoid useless interrupt.
 * Available buffer size corrected.
 * New function to read header information.
 * New function invokes decoder memory test.
 * Beep makes use of VsSdiWrite.
 *
 * Revision 1.10  2003/04/18 14:46:08  harald
 * Copyright update by the maintainer, after none of the original code had
 * been left. We have a clean BSD licence now.
 * This release had been prepared by Pavel Chromy.
 * BSYNC vs. transfer in progress issue in VsSdiPutByte().
 * Fixed possible transfer in progress issue in VsPlayerFeed().
 * HW reset may be forced by VS_SM_RESET mode bit
 *
 * Revision 1.9  2003/04/07 20:29:20  harald
 * Redesigned by Pavel Chromy
 *
 * Revision 1.9  2003/04/04 15:01:00  mac
 * VS_STATUS_EMTY is reported correctly.
 *
 * Revision 1.9  2003/02/14 13:39:00  mac
 * Several serious bugs fixed,
 * interrupt routine completely remade.
 * Unreliable spurious interrupts detection removed.
 * Mpeg frame detection removed.
 * Watermark check removed (this was rather limiting)
 * Can be optionaly compiled not to use SPI
 *
 * Revision 1.8  2003/02/04 17:50:55  harald
 * Version 3 released
 *
 * Revision 1.7  2003/01/14 16:15:19  harald
 * Sending twice the number of zeros to end MP3 stream.
 * Check for spurious interrupts to detect hanging chip.
 * Simpler portable inline assembler for short delays.
 *
 * Revision 1.6  2002/11/02 15:15:13  harald
 * Library dependencies removed
 *
 * Revision 1.5  2002/09/15 16:44:14  harald
 * *** empty log message ***
 *
 * Revision 1.4  2002/08/16 17:49:02  harald
 * First public release
 *
 * Revision 1.3  2002/06/26 17:29:08  harald
 * First pre-release with 2.4 stack
 *
 */

/*
 * This header file specifies the hardware port bits. You
 * need to change or replace it, if your hardware differs.
 */
#include <cfg/arch/avr.h>

#include <sys/atom.h>
#include <sys/event.h>
#include <sys/timer.h>
#include <sys/heap.h>

#include <dev/irqreg.h>
#include <dev/vs1001k.h>

#include <sys/bankmem.h>

#include <stddef.h> /* NULL definition */

/*!
 * \addtogroup xgVs1001
 */
/*@{*/

#ifndef VS_SCK_BIT
/*! 
 * \brief VS1001 serial control interface clock input bit. 
 *
 * The first rising clock edge after XCS has gone low marks the first 
 * bit to be written to the decoder.
 */
#define VS_SCK_BIT      0
#endif

#if !defined(VS_SCK_AVRPORT) || (VS_SCK_AVRPORT == AVRPORTB)
#define VS_SCK_PORT PORTB   /*!< Port register of \ref VS_SCK_BIT. */
#define VS_SCK_DDR  DDRB    /*!< Data direction register of \ref VS_SCK_BIT. */
#elif (VS_SCK_AVRPORT == AVRPORTD)
#define VS_SCK_PORT PORTD
#define VS_SCK_DDR  DDRD
#elif (VS_SCK_AVRPORT == AVRPORTE)
#define VS_SCK_PORT PORTE
#define VS_SCK_DDR  DDRE
#elif (VS_SCK_AVRPORT == AVRPORTF)
#define VS_SCK_PORT PORTF
#define VS_SCK_DDR  DDRF
#else
#warning "Bad SCK port specification"
#endif

#ifndef VS_SS_BIT
/*!
 * \brief VS1001 serial data interface clock input bit. 
 */
#define VS_SS_BIT       1       
#endif

#if !defined(VS_SS_AVRPORT) || (VS_SS_AVRPORT == AVRPORTB)
#define VS_SS_PORT  PORTB   /*!< Port output register of \ref VS_SS_BIT. */
#define VS_SS_DDR   DDRB    /*!< Data direction register of \ref VS_SS_BIT. */
#elif (VS_SS_AVRPORT == AVRPORTD)
#define VS_SS_PORT  PORTD
#define VS_SS_DDR   DDRD
#elif (VS_SS_AVRPORT == AVRPORTE)
#define VS_SS_PORT  PORTE
#define VS_SS_DDR   DDRE
#elif (VS_SS_AVRPORT == AVRPORTF)
#define VS_SS_PORT  PORTF
#define VS_SS_DDR   DDRF
#else
#warning "Bad SS port specification"
#endif

#ifndef VS_SI_BIT
/*!
 * \brief VS1001 serial control interface data input.
 *
 * The decoder samples this input on the rising edge of SCK if XCS is low.
 */
#define VS_SI_BIT       2       
#endif

#if !defined(VS_SI_AVRPORT) || (VS_SI_AVRPORT == AVRPORTB)
#define VS_SI_PORT  PORTB   /*!< Port output register of \ref VS_SI_BIT. */
#define VS_SI_DDR   DDRB    /*!< Data direction register of \ref VS_SI_BIT. */
#elif (VS_SI_AVRPORT == AVRPORTD)
#define VS_SI_PORT  PORTD
#define VS_SI_DDR   DDRD
#elif (VS_SI_AVRPORT == AVRPORTE)
#define VS_SI_PORT  PORTE
#define VS_SI_DDR   DDRE
#elif (VS_SI_AVRPORT == AVRPORTF)
#define VS_SI_PORT  PORTF
#define VS_SI_DDR   DDRF
#else
#warning "Bad SI port specification"
#endif

#ifndef VS_SO_BIT
/*!
 * \brief VS1001 serial control interface data output. 
 *
 * If data is transfered from the decoder, bits are shifted out on the 
 * falling SCK edge. If data is transfered to the decoder, SO is at a 
 * high impedance state.
 */
#define VS_SO_BIT       3
#endif

#if !defined(VS_SO_AVRPORT) || (VS_SO_AVRPORT == AVRPORTB)
#define VS_SO_PIN   PINB    /*!< Port input register of \ref VS_SO_BIT. */
#define VS_SO_DDR   DDRB    /*!< Data direction register of \ref VS_SO_BIT. */
#elif (VS_SO_AVRPORT == AVRPORTD)
#define VS_SO_PIN   PIND
#define VS_SO_DDR   DDRD
#elif (VS_SO_AVRPORT == AVRPORTE)
#define VS_SO_PIN   PINE
#define VS_SO_DDR   DDRE
#elif (VS_SO_AVRPORT == AVRPORTF)
#define VS_SO_PIN   PINF
#define VS_SO_DDR   DDRF
#else
#warning "Bad SO port specification"
#endif

#ifndef VS_XCS_BIT
/*!
 * \brief VS1001 active low chip select input. 
 *
 * A high level forces the serial interface into standby mode, ending 
 * the current operation. A high level also forces serial output (SO) 
 * to high impedance state.
 */
#define VS_XCS_BIT      4
#endif

#if !defined(VS_XCS_AVRPORT) || (VS_XCS_AVRPORT == AVRPORTB)
#define VS_XCS_PORT PORTB   /*!< Port output register of \ref VS_XCS_BIT. */
#define VS_XCS_DDR  DDRB    /*!< Data direction register of \ref VS_XCS_BIT. */
#elif (VS_XCS_AVRPORT == AVRPORTD)
#define VS_XCS_PORT PORTD
#define VS_XCS_DDR  DDRD
#elif (VS_XCS_AVRPORT == AVRPORTE)
#define VS_XCS_PORT PORTE
#define VS_XCS_DDR  DDRE
#elif (VS_XCS_AVRPORT == AVRPORTF)
#define VS_XCS_PORT PORTF
#define VS_XCS_DDR  DDRF
#else
#warning "Bad XCS port specification"
#endif

#ifndef VS_BSYNC_BIT
/*!
 * \brief VS1001 serial data interface bit sync. 
 *
 * The first DCLK sampling edge, during which BSYNC is high, marks the 
 * first bit of a data byte.
 */
#define VS_BSYNC_BIT    5
#endif

#if !defined(VS_BSYNC_AVRPORT) || (VS_BSYNC_AVRPORT == AVRPORTB)
#define VS_BSYNC_PORT   PORTB   /*!< Port output register of \ref VS_BSYNC_BIT. */
#define VS_BSYNC_DDR    DDRB    /*!< Data direction register of \ref VS_BSYNC_BIT. */
#elif (VS_BSYNC_AVRPORT == AVRPORTD)
#define VS_BSYNC_PORT   PORTD
#define VS_BSYNC_DDR    DDRD
#elif (VS_BSYNC_AVRPORT == AVRPORTE)
#define VS_BSYNC_PORT   PORTE
#define VS_BSYNC_DDR    DDRE
#elif (VS_BSYNC_AVRPORT == AVRPORTF)
#define VS_BSYNC_PORT   PORTF
#define VS_BSYNC_DDR    DDRF
#else
#warning "Bad BSYNC port specification"
#endif

#ifndef VS_RESET_BIT
/*!
 * \brief VS1001 hardware reset input. 
 */
#define VS_RESET_BIT    7       
#endif

#if !defined(VS_RESET_AVRPORT) || (VS_RESET_AVRPORT == AVRPORTB)
#define VS_RESET_PORT   PORTB   /*!< Port output register of \ref VS_RESET_BIT. */
#define VS_RESET_DDR    DDRB    /*!< Data direction register of \ref VS_RESET_BIT. */
#elif (VS_RESET_AVRPORT == AVRPORTD)
#define VS_RESET_PORT   PORTD
#define VS_RESET_DDR    DDRD
#elif (VS_RESET_AVRPORT == AVRPORTE)
#define VS_RESET_PORT   PORTE
#define VS_RESET_DDR    DDRE
#elif (VS_RESET_AVRPORT == AVRPORTF)
#define VS_RESET_PORT   PORTF
#define VS_RESET_DDR    DDRF
#else
#warning "Bad RESET port specification"
#endif

#ifndef VS_SIGNAL_IRQ
/*!
 * \brief VS1001 data request interrupt. 
 */
#define VS_SIGNAL       sig_INTERRUPT6
#define VS_DREQ_BIT     6
#define VS_DREQ_PORT    PORTE   /*!< Port output register of \ref VS_DREQ_BIT. */
#define VS_DREQ_PIN     PINE    /*!< Port input register of \ref VS_DREQ_BIT. */
#define VS_DREQ_DDR     DDRE    /*!< Data direction register of \ref VS_DREQ_BIT. */

#elif (VS_SIGNAL_IRQ == INT0)
#define VS_SIGNAL       sig_INTERRUPT0
#define VS_DREQ_BIT     0
#define VS_DREQ_PORT    PORTD
#define VS_DREQ_PIN     PIND
#define VS_DREQ_DDR     DDRD

#elif (VS_SIGNAL_IRQ == INT1)
#define VS_SIGNAL       sig_INTERRUPT1
#define VS_DREQ_BIT     1
#define VS_DREQ_PORT    PORTD
#define VS_DREQ_PIN     PIND
#define VS_DREQ_DDR     DDRD

#elif (VS_SIGNAL_IRQ == INT2)
#define VS_SIGNAL       sig_INTERRUPT2
#define VS_DREQ_BIT     2
#define VS_DREQ_PORT    PORTD
#define VS_DREQ_PIN     PIND
#define VS_DREQ_DDR     DDRD

#elif (VS_SIGNAL_IRQ == INT3)
#define VS_SIGNAL       sig_INTERRUPT3
#define VS_DREQ_BIT     3
#define VS_DREQ_PORT    PORTD
#define VS_DREQ_PIN     PIND
#define VS_DREQ_DDR     DDRD

#elif (VS_SIGNAL_IRQ == INT4)
#define VS_SIGNAL       sig_INTERRUPT4
#define VS_DREQ_BIT     4
#define VS_DREQ_PORT    PORTE
#define VS_DREQ_PIN     PINE
#define VS_DREQ_DDR     DDRE

#elif (VS_SIGNAL_IRQ == INT5)
#define VS_SIGNAL       sig_INTERRUPT5
#define VS_DREQ_BIT     5
#define VS_DREQ_PORT    PORTE
#define VS_DREQ_PIN     PINE
#define VS_DREQ_DDR     DDRE

#elif (VS_SIGNAL_IRQ == INT7)
#define VS_SIGNAL       sig_INTERRUPT7
#define VS_DREQ_BIT     7
#define VS_DREQ_PORT    PORTE
#define VS_DREQ_PIN     PINE
#define VS_DREQ_DDR     DDRE

#else
#warning "Bad interrupt specification"
#endif

static volatile uint8_t vs_status = VS_STATUS_STOPPED;
static volatile uint16_t vs_flush;

/*
 * \brief Write a byte to the VS1001 data interface.
 *
 * The caller is responsible for checking the DREQ line. Also make sure,
 * that decoder interrupts are disabled.
 *
 * \param b Byte to be shifted to the decoder's data interface.
 */
static INLINE void VsSdiPutByte(uint8_t b)
{
#ifdef VS_NOSPI
    uint8_t mask = 0x80;
    sbi(VS_BSYNC_PORT, VS_BSYNC_BIT);
    while (mask) {
        if (b & mask)
            sbi(VS_SI_PORT, VS_SI_BIT);
        else
            cbi(VS_SI_PORT, VS_SI_BIT);

        sbi(VS_SS_PORT, VS_SS_BIT);
        mask >>= 1;
        cbi(VS_SS_PORT, VS_SS_BIT);
        cbi(VS_BSYNC_PORT, VS_BSYNC_BIT);
    }
#else
    /* Wait for previous SPI transfer to finish. */
    loop_until_bit_is_set(SPSR, SPIF);
    sbi(VS_BSYNC_PORT, VS_BSYNC_BIT);
    outb(SPDR, b);
    _NOP();
    _NOP();
    _NOP();
    _NOP();
    cbi(VS_BSYNC_PORT, VS_BSYNC_BIT);
#endif
}

/*!
 * \brief Write a specified number of bytes to the VS1001 data interface.
 *
 * This function will check the DREQ line. Decoder interrupts must have 
 * been disabled before calling this function.
 */
static int VsSdiWrite(CONST uint8_t * data, uint16_t len)
{
    uint16_t try = 5000;

    while (len--) {
        while (try-- && bit_is_clear(VS_DREQ_PIN, VS_DREQ_BIT));
        VsSdiPutByte(*data);
        data++;
    }
    return try ? 0 : -1;
}

/*!
 * \brief Write a specified number of bytes from program space to the 
 *        VS1001 data interface.
 *
 * This function is similar to VsSdiWrite() except that the data is 
 * located in program space.
 */
static int VsSdiWrite_P(PGM_P data, uint16_t len)
{
    uint16_t try = 5000;

    while (len--) {
        while (try-- && bit_is_clear(VS_DREQ_PIN, VS_DREQ_BIT));
        VsSdiPutByte(PRG_RDB(data));
        data++;
    }
    return try ? 0 : -1;
}

/*!
 * \brief Write a byte to the serial control interface.
 *
 * Decoder interrupts must have been disabled and the decoder chip
 * select must have been enabled before calling this function.
 */
static INLINE void VsSciPutByte(uint8_t data)
{
    uint8_t mask = 0x80;

    /*
     * Loop until all 8 bits are processed.
     */
    while (mask) {

        /* Set data line. */
        if (data & mask)
            sbi(VS_SI_PORT, VS_SI_BIT);
        else
            cbi(VS_SI_PORT, VS_SI_BIT);

        /* Toggle clock and shift mask. */
        sbi(VS_SCK_PORT, VS_SCK_BIT);
        mask >>= 1;
        cbi(VS_SCK_PORT, VS_SCK_BIT);
    }
}

/*!
 * \brief Read a byte from the serial control interface.
 *
 * Decoder interrupts must have been disabled and the decoder chip
 * select must have been enabled before calling this function.
 */
static INLINE uint8_t VsSciGetByte(void)
{
    uint8_t mask = 0x80;
    uint8_t data = 0;

    /*
     * Loop until all 8 bits are processed.
     */
    while (mask) {
        /* Toggle clock and get the data. */
        sbi(VS_SCK_PORT, VS_SCK_BIT);
        if (bit_is_set(VS_SO_PIN, VS_SO_BIT))
            data |= mask;
        mask >>= 1;
        cbi(VS_SCK_PORT, VS_SCK_BIT);
    }
    return data;
}

/*!
 * \brief Write to a decoder register.
 *
 * Decoder interrupts must have been disabled before calling this function.
 */
static void VsRegWrite(uint8_t reg, uint16_t data)
{
    /* Select chip. */
    cbi(VS_XCS_PORT, VS_XCS_BIT);

#ifndef VS_NOSPI
    /* Disable SPI */
    cbi(SPCR, SPE);
#endif

    VsSciPutByte(VS_OPCODE_WRITE);
    VsSciPutByte(reg);
    VsSciPutByte((uint8_t) (data >> 8));
    VsSciPutByte((uint8_t) data);

#ifndef VS_NOSPI
    /* Re-enable SPI. Hint given by Jesper Hansen. */
    outb(SPCR, BV(MSTR) | BV(SPE));
    outb(SPSR, inb(SPSR));
#endif

    /* Deselect chip. */
    sbi(VS_XCS_PORT, VS_XCS_BIT);
}

/*
 * \brief Read from a register.
 *
 * Decoder interrupts must have been disabled before calling this function.
 * 
 * \return Register contents.
 */
static uint16_t VsRegRead(uint8_t reg)
{
    uint16_t data;

    /* Disable interrupts and select chip. */
    cbi(VS_XCS_PORT, VS_XCS_BIT);

#ifndef VS_NOSPI
    /* Disable SPI. */
    cbi(SPCR, SPE);
#endif

    VsSciPutByte(VS_OPCODE_READ);
    VsSciPutByte(reg);
    data = (uint16_t)VsSciGetByte() << 8;
    data |= VsSciGetByte();

#ifndef VS_NOSPI
    /* Re-enable SPI. Changed due to a hint by Jesper. */
    outb(SPCR, BV(MSTR) | BV(SPE));
    outb(SPSR, inb(SPSR));
#endif

    /* Deselect chip and enable interrupts. */
    sbi(VS_XCS_PORT, VS_XCS_BIT);

    return data;
}

/*!
 * \brief Enable or disable player interrupts.
 *
 * This routine is typically used by applications when dealing with 
 * unprotected buffers.
 *
 * \param enable Disables interrupts when zero. Otherwise interrupts
 *               are enabled.
 *
 * \return Zero if interrupts were disabled before this call.
 */
uint8_t VsPlayerInterrupts(uint8_t enable)
{
    static uint8_t is_enabled = 0;
    uint8_t rc;

    rc = is_enabled;
    if(enable) {
        NutIrqEnable(&VS_SIGNAL);
    }
    else {
        NutIrqDisable(&VS_SIGNAL);
    }
    is_enabled = enable;

    return rc;
}

/*
 * \brief Feed the decoder with data.
 *
 * This function serves two purposes: 
 * - It is called by VsPlayerKick() to initially fill the decoder buffer.
 * - It is used as an interrupt handler for the decoder.
 */
static void VsPlayerFeed(void *arg)
{
    uint8_t ief;
    uint8_t j = 32;
    size_t total = 0;

    if (bit_is_clear(VS_DREQ_PIN, VS_DREQ_BIT)) {
        return;
    }

    /*
     * We are hanging around here some time and may block other important
     * interrupts. Disable decoder interrupts and enable global interrupts.
     */
    ief = VsPlayerInterrupts(0);
    sei();

    /* 
     * Feed the decoder until its buffer is full or we ran out of data.
     */
    if (vs_status == VS_STATUS_RUNNING) {
        char *bp = 0;
        size_t consumed = 0;
        size_t available = 0;
        do {
            if(consumed >= available) {
                /* Commit previously consumed bytes. */
                if(consumed) {
                    NutSegBufReadCommit(consumed);
                    consumed = 0;
                }
                /* All bytes consumed, request new. */
                bp = NutSegBufReadRequest(&available);
                if(available == 0) {
                    /* End of stream. */
                    vs_status = VS_STATUS_EOF;
                    break;
                }
            }
            /* We have some data in the buffer, feed it. */
            VsSdiPutByte(*bp);
            bp++;
            consumed++;
            total++;
            if (total > 4096) {
                vs_status = VS_STATUS_EOF;
                break;
            }

            /* Allow 32 bytes to be sent as long as DREQ is set, This includes
               the one in progress. */
            if (bit_is_set(VS_DREQ_PIN, VS_DREQ_BIT))
                j = 32;
        } while(j--);

        /* Finally re-enable the producer buffer. */
        NutSegBufReadLast(consumed);
    }

    /* 
     * Flush the internal VS buffer. 
     */
    if(vs_status != VS_STATUS_RUNNING && vs_flush) {
        do {
            VsSdiPutByte(0);
            if (--vs_flush == 0) {
                /* Decoder internal buffer is flushed. */
                vs_status = VS_STATUS_EMPTY;
                break;
            }
            /* Allow 32 bytes to be sent as long as DREQ is set, This includes
               the one in progress. */
            if (bit_is_set(VS_DREQ_PIN, VS_DREQ_BIT))
                j = 32;
        } while(j--);
    }
    VsPlayerInterrupts(ief);
}

/*!
 * \brief Start playback.
 *
 * This routine will send the first MP3 data bytes to the
 * decoder, until it is completely filled. The data buffer
 * should have been filled before calling this routine.
 *
 * Decoder interrupts will be enabled.
 *
 * \return 0 on success, -1 otherwise.
 */
int VsPlayerKick(void)
{
    /*
     * Start feeding the decoder with data.
     */
    VsPlayerInterrupts(0);
    vs_status = VS_STATUS_RUNNING;
    VsPlayerFeed(NULL);
    VsPlayerInterrupts(1);

    return 0;
}

/*!
 * \brief Stops the playback.
 *
 * This routine will stops the MP3 playback, VsPlayerKick() may be used 
 * to resume the playback.
 *
 * \return 0 on success, -1 otherwise.
 */
int VsPlayerStop(void)
{
    uint8_t ief;

    ief = VsPlayerInterrupts(0);
    /* Check whether we need to stop at all to not overwrite other than running status */
    if (vs_status == VS_STATUS_RUNNING) {
        vs_status = VS_STATUS_STOPPED;
    }
    VsPlayerInterrupts(ief);

    return 0;
}


/*!
 * \brief Sets up decoder internal buffer flushing.
 *
 * This routine will set up internal VS buffer flushing,
 * unless the buffer is already empty and starts the playback
 * if necessary. The internal VS buffer is flushed in VsPlayerFeed()
 * at the end of the stream.
 *
 * Decoder interrupts will be enabled.
 *
 * \return 0 on success, -1 otherwise.
 */
int VsPlayerFlush(void)
{
    VsPlayerInterrupts(0);
    /* Set up fluhing unless both buffers are empty. */
    if (vs_status != VS_STATUS_EMPTY || NutSegBufUsed()) {
        if (vs_flush == 0)
            vs_flush = VS_FLUSH_BYTES;
        /* start the playback if necessary */
        if (vs_status != VS_STATUS_RUNNING)
            VsPlayerKick();
    }
    VsPlayerInterrupts(1);
    return 0;
}


/*!
 * \brief Initialize the VS1001 hardware interface.
 *
 * \return 0 on success, -1 otherwise.
 */
int VsPlayerInit(void)
{
    /* Disable decoder interrupts. */
    VsPlayerInterrupts(0);

    /* Keep decoder in reset state. */
    cbi(VS_RESET_PORT, VS_RESET_BIT);
    sbi(VS_RESET_DDR, VS_RESET_BIT);

    /* Set BSYNC output low. */
    cbi(VS_BSYNC_PORT, VS_BSYNC_BIT);
    sbi(VS_BSYNC_DDR, VS_BSYNC_BIT);

    /* Set MP3 chip select output low. */
    sbi(VS_XCS_PORT, VS_XCS_BIT);
    sbi(VS_XCS_DDR, VS_XCS_BIT);

    /* Set DREQ input with pullup. */
    sbi(VS_DREQ_PORT, VS_DREQ_BIT);
    cbi(VS_DREQ_DDR, VS_DREQ_BIT);

    /* Init SPI Port. */
    sbi(VS_SI_DDR, VS_SI_BIT);
    sbi(VS_SS_DDR, VS_SS_BIT);
    cbi(VS_SO_DDR, VS_SO_BIT);

    /* Set SCK output low. */
    cbi(VS_SCK_PORT, VS_SCK_BIT);
    sbi(VS_SCK_DDR, VS_SCK_BIT);

#ifndef VS_NOSPI
    {
        uint8_t dummy;           /* Required by some compilers. */

        /* 
         * Init SPI mode to no interrupts, enabled, MSB first, master mode, 
         * rising clock and fosc/4 clock speed. Send an initial zero byte to 
         * make sure SPIF is set. Note, that the decoder reset line is still
         * active.
         */
        outb(SPCR, BV(MSTR) | BV(SPE));
        dummy = inb(SPSR);
        outb(SPDR, 0);
    }
#endif

    /* Register the interrupt routine */
    if (NutRegisterIrqHandler(&VS_SIGNAL, VsPlayerFeed, NULL)) {
        return -1;
    }

    /* Rising edge will generate interrupts. */
    NutIrqSetMode(&VS_SIGNAL, NUT_IRQMODE_RISINGEDGE);

    /* Release decoder reset line. */
    sbi(VS_RESET_PORT, VS_RESET_BIT);
    NutDelay(4);

    /* Force frequency change (see datasheet). */
    VsRegWrite(VS_CLOCKF_REG, 0x9800);
    VsRegWrite(VS_INT_FCTLH_REG, 0x8008);
    NutDelay(200);

    /* Clear any spurious interrupt. */
    outb(EIFR, BV(VS_DREQ_BIT));

    return 0;
}

/*!
 * \brief Software reset the decoder.
 *
 * This function is typically called after VsPlayerInit() and at the end
 * of each track.
 *
 * \param mode Any of the following flags may be or'ed
 * - VS_SM_DIFF Left channel inverted.
 * - VS_SM_FFWD Fast forward.
 * - VS_SM_RESET Force hardware reset.
 * - VS_SM_PDOWN Switch to power down mode.
 * - VS_SM_BASS Bass/treble enhancer.
 *
 * \return 0 on success, -1 otherwise.
 */
int VsPlayerReset(uint16_t mode)
{
    /* Disable decoder interrupts and feeding. */
    VsPlayerInterrupts(0);
    vs_status = VS_STATUS_STOPPED;

    /* Software reset, set modes of decoder. */
    VsRegWrite(VS_MODE_REG, VS_SM_RESET | mode);
    NutDelay(2);

    /*
     * Check for correct reset.
     */
    if ((mode & VS_SM_RESET) != 0 || bit_is_clear(VS_DREQ_PIN, VS_DREQ_BIT)) {
        /* If not succeeded, give it one more chance and try hw reset,
           HW reset may also be forced by VS_SM_RESET mode bit. */
        cbi(VS_RESET_PORT, VS_RESET_BIT);
        _NOP();
        sbi(VS_RESET_PORT, VS_RESET_BIT);
        NutDelay(4);

        /* Set the requested modes. */
        VsRegWrite(VS_MODE_REG, VS_SM_RESET | mode);
        NutDelay(2);
        if (bit_is_clear(VS_DREQ_PIN, VS_DREQ_BIT))
            return -1;
    }

    /* Force frequency change (see datasheet). */
    VsRegWrite(VS_CLOCKF_REG, 0x9800);
    VsRegWrite(VS_INT_FCTLH_REG, 0x8008);
    NutDelay(2);

    /* Clear any spurious interrupts. */
    outb(EIFR, BV(VS_DREQ_BIT));

    return 0;
}

/*!
 * \brief Set mode register of the decoder.
 *
 * \param mode Any of the following flags may be or'ed
 * - VS_SM_DIFF Left channel inverted.
 * - VS_SM_FFWD Fast forward.
 * - VS_SM_RESET Software reset.
 * - VS_SM_PDOWN Switch to power down mode.
 * - VS_SM_BASS Bass/treble enhancer.
 *
 * \return 0 on success, -1 otherwise.
 */
int VsPlayerSetMode(uint16_t mode)
{
    uint8_t ief;
    
    ief = VsPlayerInterrupts(0);
    VsRegWrite(VS_MODE_REG, mode);
    VsPlayerInterrupts(ief);

    return 0;
}

/*!
 * \brief Returns play time since last reset.
 *
 * \return Play time since reset in seconds
 */
uint16_t VsPlayTime(void)
{
    uint16_t rc;
    uint8_t ief;

    ief = VsPlayerInterrupts(0);
    rc = VsRegRead(VS_DECODE_TIME_REG);
    VsPlayerInterrupts(ief);

    return rc;
}

/*!
 * \brief Returns status of the player.
 *
 * \return Any of the following value:
 * - VS_STATUS_STOPPED Player is ready to be started by VsPlayerKick().
 * - VS_STATUS_RUNNING Player is running.
 * - VS_STATUS_EOF Player has reached the end of a stream after VsPlayerFlush() has been called.
 * - VS_STATUS_EMPTY Player runs out of data. VsPlayerKick() will restart it.
 */
uint8_t VsGetStatus(void)
{
    return vs_status;
}

#ifdef __GNUC__

/*!
 * \brief Query MP3 stream header information.
 *
 * \param vshi Pointer to VS_HEADERINFO structure.
 *
 * \return 0 on success, -1 otherwise.
 */
int VsGetHeaderInfo(VS_HEADERINFO * vshi)
{
    uint16_t *usp = (uint16_t *) vshi;
    uint8_t ief;

    ief = VsPlayerInterrupts(0);
    *usp = VsRegRead(VS_HDAT1_REG);
    *++usp = VsRegRead(VS_HDAT0_REG);
    VsPlayerInterrupts(ief);

    return 0;
}
#endif

/*!
 * \brief Initialize decoder memory test and return result.
 *
 * \return Memory test result.
 * - Bit 0: Good X ROM
 * - Bit 1: Good Y ROM (high)
 * - Bit 2: Good Y ROM (low)
 * - Bit 3: Good Y RAM
 * - Bit 4: Good X RAM
 * - Bit 5: Good Instruction RAM (high)
 * - Bit 6: Good Instruction RAM (low)
 */
uint16_t VsMemoryTest(void)
{
    uint16_t rc;
    uint8_t ief;
    static prog_char mtcmd[] = { 0x4D, 0xEA, 0x6D, 0x54, 0x00, 0x00, 0x00, 0x00 };

    ief = VsPlayerInterrupts(0);
    VsSdiWrite_P(mtcmd, sizeof(mtcmd));
    NutDelay(40);
    rc = VsRegRead(VS_HDAT0_REG);
    VsPlayerInterrupts(ief);

    return rc;
}

/*!
 * \brief Set volume.
 *
 * \param left  Left channel volume.
 * \param right Right channel volume.
 *
 * \return 0 on success, -1 otherwise.
 */
int VsSetVolume(uint8_t left, uint8_t right)
{
    uint8_t ief;

    ief = VsPlayerInterrupts(0);
    VsRegWrite(VS_VOL_REG, (((uint16_t) left) << 8) | (uint16_t) right);
    VsPlayerInterrupts(ief);

    return 0;
}

/*!
 * \brief Sine wave beep.
 *
 * \param fsin Frequency.
 * \param ms   Duration.
 *
 * \return 0 on success, -1 otherwise.
 */
int VsBeep(uint8_t fsin, uint8_t ms)
{
    uint8_t ief;
    static prog_char on[] = { 0x53, 0xEF, 0x6E };
    static prog_char off[] = { 0x45, 0x78, 0x69, 0x74 };
    static prog_char end[] = { 0x00, 0x00, 0x00, 0x00 };

    /* Disable decoder interrupts. */
    ief = VsPlayerInterrupts(0);

    fsin = 56 + (fsin & 7) * 9;
    VsSdiWrite_P(on, sizeof(on));
    VsSdiWrite(&fsin, 1);
    VsSdiWrite_P(end, sizeof(end));
    NutDelay(ms);
    VsSdiWrite_P(off, sizeof(off));
    VsSdiWrite_P(end, sizeof(end));

    /* Enable decoder interrupts. */
    VsPlayerInterrupts(ief);

    return 0;
}


/*@}*/
