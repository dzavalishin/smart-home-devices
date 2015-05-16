/*
 * Copyright 2010 by egnite GmbH
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
 * \file gorp/list/lstrcopy.c
 * \brief Linked list string duplicate helper function.
 *
 * \verbatim
 * $Id$
 * \endverbatim
 */

#include <memdebug.h>

#include <stdlib.h>

#include <gorp/lili.h>

/*!
 * \addtogroup xgLili
 */
/*@{*/

/*!
 * \brief Create a copy of a referenced string item.
 *
 * A pointer to this function can be passed to LiLiCreate(), if the list
 * items are simple strings.
 *
 * Note, that the result is not checked during list insertion. If not
 * enough memory is available to create a copy, then a reference value
 * of zero is stored in the node object.
 *
 * \param ref Reference of the original string. Actually this is a pointer
 *            to the string. If 0, no copy is created.
 *
 * \return Reference of a successfully created copy, otherwise 0.
 */
LILI_ITEMREF LiLiCreateStringItemCopy(LILI_ITEMREF ref)
{
    if (ref) {
        return (LILI_ITEMREF)strdup((char *)ref);
    }
    return 0;
}

/*!
 * \brief Destroy the string item copy.
 *
 * A pointer to this function can be passed to LiLiCreate(), if the list
 * items are simple strings.
 *
 * \param ref Reference of the string copy to destroy. Ignored if 0.
 */
void LiLiDestroyStringItemCopy(LILI_ITEMREF ref)
{
    if (ref) {
        free((void *)ref);
    }
}

/*@}*/
