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

/*
 * \file gorp/edline/history.c
 * \brief Simple edit history.
 *
 * \verbatim
 * $Id$
 * \endverbatim
 */

#include <gorp/edline.h>

#include <stdlib.h>
#include <string.h>

/*!
 * \addtogroup xgEdLine
 */
/*@{*/

#ifndef EDIT_DISABLE_HISTORY

/*!
 * \brief Create an editor history table.
 *
 * \param siz Maximum number of history entries.
 *
 * \return Pointer to an \ref EDITHISTORY structure on success.
 *         A NULL pointer is returned in case of an error.
 */
EDITHISTORY *EditHistoryCreate(int siz)
{
    EDITHISTORY *hist = malloc(sizeof(EDITHISTORY));

    if (hist) {
        hist->hist_siz = siz;
        hist->hist_tab = calloc(siz, sizeof(char *));
        if (hist->hist_tab == NULL) {
            free(hist);
            hist = NULL;
        }
    }
    return hist;
}

/*!
 * \brief Create an editor history table.
 *
 * Releases occupied memory.
 *
 * \param hist Pointer to an \ref EDITHISTORY structure, obtained by a 
 *             previous call to \ref EditHistoryCreate.
 */
void EditHistoryDestroy(EDITHISTORY *hist)
{
    int i;

    if (hist) {
        for (i = 0; i < hist->hist_siz; i++) {
            if (hist->hist_tab[i]) {
                free(hist->hist_tab[i]);
            }
        }
        free(hist);
    }
}

/*!
 * \brief Replace the specified history table entry.
 *
 * \param idx  Index in to the history table.
 * \param buf  Pointer to the string that will replace the existing one.
 * \param hist Pointer to an \ref EDITHISTORY structure, obtained by a 
 *             previous call to \ref EditHistoryCreate.
 */
void EditHistorySet(EDITHISTORY *hist, int idx, char *buf)
{
    if (hist) {
        if (idx >= 0 && idx < hist->hist_siz) {
            if (hist->hist_tab[idx]) {
                free(hist->hist_tab[idx]);
                hist->hist_tab[idx] = NULL;
            }
            if (buf) {
                hist->hist_tab[idx] = strdup(buf);
            }
        }
    }
}

/*!
 * \brief Retrieve the specified history table entry.
 *
 * \param idx  Index in to the history table.
 * \param hist Pointer to an \ref EDITHISTORY structure, obtained by a 
 *             previous call to \ref EditHistoryCreate.
 */
int EditHistoryGet(EDITHISTORY *hist, int idx, char *buf, int siz)
{
    int rc = 0;
    char *cp;

    if (hist) {
        if (idx >= 0 && idx < hist->hist_siz) {
            cp = hist->hist_tab[idx];
            if (cp) {
                rc = strlen(cp) + 1;
                if (buf) {
                    if (rc > siz) {
                        rc = siz;
                    }
                    memcpy(buf, cp, rc);
                }
            }
        }
    }
    return rc - 1;
}

/*!
 * \brief Insert a new history table entry at the specified position.
 *
 * All following entries are moved up by 1. If the table is full, then 
 * the top most entry will be removed.
 *
 * \param idx  Index in to the history table.
 * \param buf  Pointer to the new string. If it doesn't differ from
 *             the existing entry, than nothing will be changed.
 * \param hist Pointer to an \ref EDITHISTORY structure, obtained by a 
 *             previous call to \ref EditHistoryCreate.
 */
void EditHistoryInsert(EDITHISTORY *hist, int idx, char *buf)
{
    int i;

    if (hist && buf && *buf) {
        i = hist->hist_siz - 1;
        if (idx >= 0 && idx < i) {
            if (hist->hist_tab[idx]) {
                /* Make sure we have a new contents. */
                if (strcmp(hist->hist_tab[idx], buf)) {
                    /* Release the entry on top. */
                    if (hist->hist_tab[i]) {
                        free(hist->hist_tab[i]);
                    }
                    /* Move all entries above the insertion point up. */
                    while (--i >= idx) {
                        hist->hist_tab[i + 1] = hist->hist_tab[i];
                    }
                } else {
                    /* No change. */
                    return;
                }
            }
            hist->hist_tab[idx] = strdup(buf);
        }
    }
}

#endif /* EDIT_DISABLE_HISTORY */

/*@}*/
