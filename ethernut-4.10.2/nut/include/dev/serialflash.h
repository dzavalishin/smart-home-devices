#ifndef _DEV_SERIALFLASH_H_
#define _DEV_SERIALFLASH_H_

/*
 * Copyright (C) 2010 by egnite GmbH
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

#include <dev/spibus.h>

/*!
 * \addtogroup xgSerialflash
 */
/*@{*/

/*! \brief Max. number of pages. */
#ifndef SERIALFLASH_MAX_UNITS
#if defined(__AVR__)
#define SERIALFLASH_MAX_UNITS   65535
#else
#define SERIALFLASH_MAX_UNITS   65536
#endif
#endif

#if SERIALFLASH_MAX_UNITS <= 65535
typedef uint16_t sf_unit_t;
typedef uint_fast16_t sf_unit_fast_t;
#else
typedef uint32_t sf_unit_t;
typedef uint_fast32_t sf_unit_fast_t;
#endif

/*!
 * \brief Serial flash support information structure type.
 */
typedef struct _NUTSERIALFLASH NUTSERIALFLASH;

/*!
 * \brief Serial flash support information structure.
 */
struct _NUTSERIALFLASH {
    /*! \brief Pointer to the SPI node structure. */
    NUTSPINODE *sf_node;
    /*! \brief Pointer to a local information structure. */
    void *sf_info;
    /*! \brief Size of the erase/write unit. */
    size_t sf_unit_size;
    /*! \brief Total number of units available, including reserved ones. */
    sf_unit_t sf_units;
    /*! \brief Configurable number of reserved units at the bottom. */
    sf_unit_t sf_rsvbot;
    /*! \brief Configurable number of reserved units at the top. */
    sf_unit_t sf_rsvtop;
    /*! \brief Flash device initialization. */
    int (*sf_init) (NUTSERIALFLASH *);
    /*! \brief Release the flash device. */
    void (*sf_exit) (NUTSERIALFLASH *);
    /*! \brief Check a number of units. */
    int (*sf_check) (NUTSERIALFLASH *, sf_unit_t, int);
    /*! \brief Read bytes from a unit. */
    int (*sf_read) (NUTSERIALFLASH *, sf_unit_t, int, void *, int);
    /*! \brief Compare bytes of a unit. */
    int (*sf_compare) (NUTSERIALFLASH *, sf_unit_t, int, CONST void *, int);
    /*! \brief Find first used byte. */
    int (*sf_used) (NUTSERIALFLASH *, sf_unit_t, int);
    /*! \brief Write bytes from a unit. */
    int (*sf_write) (NUTSERIALFLASH *, sf_unit_t, int, CONST void *, int);
    /*! \brief Copy a unit. */
    int (*sf_copy) (NUTSERIALFLASH *, sf_unit_t, sf_unit_t);
    /*! \brief Commit changes. */
    int (*sf_commit) (NUTSERIALFLASH *, sf_unit_t);
    /*! \brief Erase a number of units. */
    int (*sf_erase) (NUTSERIALFLASH *, sf_unit_t, int);
};

/*@}*/

#endif
