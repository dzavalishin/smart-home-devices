#ifndef _FS_UROMFS_H_
#define _FS_UROMFS_H_

/*
 * Copyright (C) 2001-2004 by egnite Software GmbH. All rights reserved.
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
 *
 */

/*
 * $Log$
 * Revision 1.6  2009/01/17 11:26:47  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.5  2008/08/11 07:00:19  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.4  2006/01/05 16:45:29  haraldkipp
 * Size and pos types changed to support 32 bit targets.
 *
 * Revision 1.3  2004/03/18 11:38:09  haraldkipp
 * Deprecated functions removed
 *
 * Revision 1.2  2003/07/20 19:27:59  haraldkipp
 * Patch by Alessandro Zummo. Moves the urom filesystem filenames to
 * AVR's flash memory.
 *
 * Revision 1.1.1.1  2003/05/09 14:41:10  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.8  2003/02/04 18:00:41  harald
 * Version 3 released
 *
 * Revision 1.7  2003/01/14 16:34:21  harald
 * *** empty log message ***
 *
 * Revision 1.6  2002/11/02 15:17:00  harald
 * Library dependencies moved to compiler.h
 *
 * Revision 1.5  2002/07/03 16:45:40  harald
 * Using GCC 3.2
 *
 * Revision 1.4  2002/06/26 17:29:17  harald
 * First pre-release with 2.4 stack
 *
 */
#include <stddef.h>
#include <sys/types.h>
#include <stdint.h>

/*!
 * \file fs/uromfs.h
 * \brief Micro-ROM file system definitions.
 */

/*!
 * \brief Mikro-ROM directory entry type.
 */
typedef struct _ROMENTRY ROMENTRY;

/*!
 * \struct _ROMENTRY uromfs.h fs/uromfs.h
 * \brief Mikro-ROM directory entry structure.
 */
struct _ROMENTRY {
    ROMENTRY *rome_next;    /*!< Link to next ROMENTRY structure. */
    prog_char *rome_name;   /*!< Filename. */
    unsigned int rome_size;        /*!< File size. */
    prog_char *rome_data;   /* __attribute__ ((progmem));  !< File data. */
};

/*!
 * \brief Mikro-ROM file information type.
 */
typedef struct _ROMFILE ROMFILE;

/*!
 * \struct _ROMFILE uromfs.h fs/uromfs.h
 * \brief Mikro-ROM file information structure.
 */
struct _ROMFILE {
    ROMENTRY *romf_entry;   /*!< Points to ROMENTRY */
    unsigned int romf_pos;      /*!< Current read position. */
};

/*!
 * \brief Linked list of all microROM files.
 */
extern ROMENTRY *romEntryList;

#endif
