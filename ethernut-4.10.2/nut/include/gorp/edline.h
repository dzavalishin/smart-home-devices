#ifndef _GORP_EDLINE_EDLINE_H_
#define	_GORP_EDLINE_EDLINE_H_

/*
 * Copyright 2009 by egnite GmbH
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

#include <compiler.h>
#include <stdint.h>

/*
 * \file include/gorp/edline.h
 * \brief Simple line editor definitions.
 *
 * \verbatim
 * $Id$
 * \endverbatim
 */

/*!
 * \addtogroup xgEdLine
 */
/*@{*/

/*!
 * \name Edit mode flags
 *
 * Used when calling \ref EdLineOpen to enable specific line
 * editor features.
 */
/*@{*/
/*! \brief Enables echoing of input characters. */
#define EDIT_MODE_ECHO      0x0001
/*! \brief Enables binary mode (currently unused). */
#define EDIT_MODE_BINARY    0x0002
/*! \brief Enables input line history. */
#define EDIT_MODE_HISTORY   0x0004
/*@}*/

/*!
 * \name Line editor input commands
 *
 * Special keys which are interpreted by the editor as commands.
 */
/*@{*/
#ifndef EDIT_KEY_IGNORE
/*! \brief No operation. Default is Ctrl-@. */
#define EDIT_KEY_IGNORE  0x00 /* CTRL-@ '\0' */
#endif
#ifndef EDIT_KEY_HOME
/*! \brief Move cursor to line begin. Default is Ctrl-A. */
#define EDIT_KEY_HOME    0x01 /* CTRL-A */
#endif
#ifndef EDIT_KEY_LEFT
/*! \brief Move cursor left. Default is Ctrl-B. */
#define EDIT_KEY_LEFT    0x02 /* CTRL-B */
#endif
#ifndef EDIT_KEY_END
/*! \brief Move cursor to line end. Default is Ctrl-E. */
#define EDIT_KEY_END     0x05 /* CTRL-E */
#endif
#ifndef EDIT_KEY_RIGHT
/*! \brief Move cursor right. Default is Ctrl-F. */
#define EDIT_KEY_RIGHT   0x06 /* CTRL-F */
#endif
#ifndef EDIT_KEY_REMOVE
/*! \brief Delete left character. Default is Ctrl-H. */
#define EDIT_KEY_REMOVE  0x08 /* CTRL-H '\b' */
#endif
#ifndef EDIT_KEY_ENTER
/*! \brief Confirm input. Default is Ctrl-J. */
#define EDIT_KEY_ENTER   0x0a /* CTRL-J '\n' */
#endif
#ifndef EDIT_DISABLE_HISTORY
#ifndef EDIT_KEY_UP
/*! \brief History upwards. Default is Ctrl-R. */
#define EDIT_KEY_UP      0x12 /* CTRL-R */
#endif
#ifndef EDIT_KEY_DOWN
/*! \brief History downwards. Default is Ctrl-V. */
#define EDIT_KEY_DOWN    0x16 /* CTRL-V */
#endif
#ifndef EDIT_KEY_RESTORE
/*! \brief Restore default. Default is ESC. */
#define EDIT_KEY_RESTORE 0x1b /* CTRL-[ '\e' */
#endif
#endif /* EDIT_DISABLE_HISTORY */
/*@}*/

/*!
 * \name Line editor output commands
 *
 * Special characters used by the editor to update the output.
 */
/*@{*/
#ifndef EDIT_CHAR_SPACE
/*! \brief Clear character right. */
#define EDIT_CHAR_SPACE ' '
#endif
#ifndef EDIT_CHAR_BACKSPACE
/*! \brief Move cursor left. */
#define EDIT_CHAR_BACKSPACE '\b'
#endif
#ifndef EDIT_CHAR_ALARM
/*! \brief Invoke audible alarm. */
#define EDIT_CHAR_ALARM '\a'
#endif
#ifndef EDIT_STR_EOL
/*! \brief Terminate input. */
#define EDIT_STR_EOL    "\r\n"
#endif
/*@}*/

#ifndef EDIT_DISABLE_HISTORY
/*! \brief Line input history. */
typedef struct _EDITHISTORY {
    /*! \brief Maximum number of entries. */
    int hist_siz;
    /*! \brief Table of entries. */
    char **hist_tab;
} EDITHISTORY;
#endif

/*! \brief Input routine type. */
typedef int (*EDLINEGET) (void *);
/*! \brief Output routine type. */
typedef int (*EDLINEPUT) (void *, int);
/*! \brief Character mapping routine type. */
typedef int (*EDLINEMAP) (int, int *);

/*! \brief Edit line information structure. */
typedef struct _EDLINE {
    /*! \brief Input routine. */
    EDLINEGET el_get;
    /*! \brief Input routine parameter. */
    void *el_iparm;
    /*! \brief Output routine. */
    EDLINEPUT el_put;
    /*! \brief Output routine parameter. */
    void *el_oparm;
    /*! \brief Character mapping routine. */
    EDLINEMAP el_map;
    /*! \brief Editor mode flags. */
    uint_fast16_t el_mode;
    /*! \brief Character mapping sequence. */
    int el_seq;
#ifndef EDIT_DISABLE_HISTORY
    /*! \brief Line input history. */
    EDITHISTORY *el_hist;
#endif
} EDLINE;

/*@}*/

__BEGIN_DECLS
/* */
extern EDLINE *EdLineOpen(uint16_t mode);
extern void EdLineClose(EDLINE *el);
extern int EdLineRead(EDLINE *el, char *buf, int siz);

#ifndef EDIT_DISABLE_HISTORY
extern EDITHISTORY *EditHistoryCreate(int siz);
extern void EditHistoryDestroy(EDITHISTORY *hist);
extern void EditHistorySet(EDITHISTORY *hist, int idx, char *buf);
extern int EditHistoryGet(EDITHISTORY *hist, int idx, char *buf, int siz);
extern void EditHistoryInsert(EDITHISTORY *hist, int idx, char *buf);
#endif

extern void EdLineRegisterKeymap(EDLINE *el, EDLINEMAP map);
extern int EdLineKeyMap(int key, int *seq);
extern int EdLineKeyMapVt100(int key, int *seq);

extern void EdLineRegisterInput(EDLINE *el, EDLINEGET get, void *iparm);
extern void EdLineRegisterOutput(EDLINE *el, EDLINEPUT put, void *oparm);

__END_DECLS
/* */
#endif
