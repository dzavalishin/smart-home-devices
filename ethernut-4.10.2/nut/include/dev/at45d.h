#ifndef _DEV_AT45D_H_
#define _DEV_AT45D_H_
/*
 * Copyright (C) 2008-2009 by egnite GmbH
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
 * \file dev/at45d.h
 * \brief AT45D DataFlash defines.
 *
 * \verbatim
 * $Id: at45d.h 3653 2011-11-14 15:20:31Z haraldkipp $
 * \endverbatim
 */

#include <sys/types.h>
#include <stdint.h>

/*!
 * \name AT45D DataFlash Commands
 */
/*@{*/
/*! \brief Continuos read (low frequency). 
 *
 * Reads a continous stream in low speed mode. Automatically switches 
 * to the the page and wraps to the first page after the last has been 
 * read. Note, that the data buffers are not used for this operation.
 */
#define DFCMD_CONT_READ_LF      0x03

/*! \brief Continuos read (high frequency).
 *
 * Reads a continous stream in high speed mode. Automatically switches 
 * to the the page and wraps to the first page after the last has been 
 * read. Note, that the data buffers are not used for this operation.
 */
#define DFCMD_CONT_READ_HF      0x0B

/*! \brief Block erase. 
 */
#define DFCMD_BLOCK_ERASE       0x50

/*! \brief Sector erase. 
 */
#define DFCMD_SECTOR_ERASE      0x7C

/*! \brief Page erase. 
 */
#define DFCMD_PAGE_ERASE        0x81

/*! \brief Main memory page program through buffer 1. 
 */
#define DFCMD_BUF1_PROG         0x82

/*! \brief Buffer 1 flash with page erase. 
 */
#define DFCMD_BUF1_FLASH        0x83

/*! \brief Buffer 1 write. 
 */
#define DFCMD_BUF1_WRITE        0x84
/*! \brief Main memory page program through buffer 2.
 */
#define DFCMD_BUF2_PROG         0x85

/*! \brief Buffer 2 flash with page erase. 
 */
#define DFCMD_BUF2_FLASH        0x86

/*! \brief Buffer 2 write. 
 */
#define DFCMD_BUF2_WRITE        0x87

/*! \brief Buffer 1 flash without page erase. 
 */
#define DFCMD_BUF1_FLASH_NE     0x88

/*! \brief Buffer 2 flash without page erase. 
 */
#define DFCMD_BUF2_FLASH_NE     0x89
/*! \brief Chip erase. 
 */
#define DFCMD_CHIP_ERASE        0xC7

/*! \brief Buffer 1 read (low frequency).
 */
#define DFCMD_BUF1_READ_LF      0xD1

/*! \brief Read main memory page.
 *
 * Automatically wraps to the first byte of the same page after
 * the last byte had been read. The data buffers are left unchanged.
 */
#define DFCMD_READ_PAGE         0xD2

/*! \brief Buffer 2 read (low frequency).
 */
#define DFCMD_BUF2_READ_LF      0xD3

/*! \brief Buffer 1 read.
 */
#define DFCMD_BUF1_READ         0xD4

/*! \brief Buffer 2 read.
 */
#define DFCMD_BUF2_READ         0xD6

/*! \brief Read status register. 
 */
#define DFCMD_READ_STATUS       0xD7

/*! \brief Continuos read (legacy).
 *
 * Reads a continous stream. Automatically switches to the the page
 * and wraps to the first page after the last has been read. Note,
 * that the data buffers are not used for this operation.
 */
#define DFCMD_CONT_READ         0xE8
/*@}*/


/*!
 * \name AT45D DataFlash Status Bits
 */
/*@{*/
/*! \brief Ready (not busy) flag. */
#define AT45D_STATUS_RDY        0x80
/*! \brief Last compare result. */
#define AT45D_STATUS_COMP       0x40
/*! \brief Device type code. */
#define AT45D_STATUS_DENSITY    0x3C
/*! \brief Sector protection flag. */
#define AT45D_STATUS_PROTECT    0x02
/*! \brief Set when binary page size is configured. */
#define AT45D_STATUS_PAGE_SIZE  0x01
/*@}*/


/*!
 * \brief AT45D DataFlash parameter structure type.
 */
typedef CONST struct _AT45D_INFO AT45D_INFO;

/*!
 * \brief AT45D DataFlash parameter structure.
 */
struct _AT45D_INFO {
    /*! \brief Least significant bit of the page parameter. */
    uint_fast8_t at45d_pshft;
    /*! \brief Total number of pages. */
    uint32_t at45d_pages;
    /*! \brief Number of bytes per page. */
    size_t at45d_psize;
    /*! \brief Type determination value. */
    uint_fast8_t at45d_srval;
};

/*!
 * \brief Table of known AT45D DataFlash chips.
 */
extern AT45D_INFO at45d_info[];

/*!
 * \brief Number of known AT45D DataFlash chips.
 */
extern uint_fast8_t at45d_known_types;

#endif
