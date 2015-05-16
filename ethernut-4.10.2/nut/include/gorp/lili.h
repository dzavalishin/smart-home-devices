#ifndef GORP_LILI_LILI_H
#define GORP_LILI_LILI_H

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
 * \file include/gorp/lili.h
 * \brief Linked list definitions.
 *
 * \verbatim
 * $Id$
 * \endverbatim
 */

#include <compiler.h>
#include <stdint.h>

/*!
 * \addtogroup xgLili
 */
/*@{*/

/*! \name List attribute flags */
/*@{*/
/*! \brief Last in, first out queue. */
#define LILI_F_LIFO     0x00
/*! \brief First in, first out queue. */
#define LILI_F_FIFO     0x01
/*! \brief Sorted list. */
#define LILI_F_SORT     0x02
/*! \brief List order mask. */
#define LILI_F_ORDER    0x03
/*! \brief Allow unique items only. */
#define LILI_F_UNIQUE   0x80
/*@}*/

/*! \brief Type of an item reference. */
typedef intptr_t LILI_ITEMREF;

typedef LILI_ITEMREF (*LiLiItemCreateFunc) (LILI_ITEMREF);
typedef void (*LiLiItemDestroyFunc) (LILI_ITEMREF);
typedef int (*LiLiItemCompareFunc) (LILI_ITEMREF, LILI_ITEMREF);

/*! \brief Node object type. */
typedef	struct _LILI_NODE LILI_NODE;

/*! \brief Node object structure. */
struct _LILI_NODE {
    LILI_NODE *nod_nxt;
    LILI_NODE *nod_prv;
    LILI_ITEMREF nod_itm;
};

/*! \brief List object type. */
typedef	struct _LILI LILI;

/*! \brief List object structure. */
struct _LILI {
    LILI_NODE *lst_head;
    LILI_NODE *lst_tail;
    uint_fast8_t lst_flags;
    LiLiItemCreateFunc lst_icreate;
    LiLiItemDestroyFunc lst_idestroy;
    LiLiItemCompareFunc lst_icompare;
};

/*!
 * \brief Return true if the given list is a last in, first out queue.
 */
#define LiLiIsLifo(list) (((list)->lst_flags & LILI_F_ORDER) == LILI_F_LIFO)

/*!
 * \brief Return true if the given list is a first in, first out queue.
 */
#define LiLiIsFifo(list) (((list)->lst_flags & LILI_F_ORDER) == LILI_F_FIFO)

/*!
 * \brief Return true if the given list is sorted.
 */
#define LiLiIsSorted(list) (((list)->lst_flags & LILI_F_ORDER) == LILI_F_SORT)

/*!
 * \brief Return true if the given list is empty.
 */
#define LiLiIsEmpty(list) ((list)->lst_head == NULL)

/*!
 * \brief Return true if the given list has unique items only.
 */
#define LiLiHasUniqueItems(list) (((list)->lst_flags & LILI_F_UNIQUE) == LILI_F_UNIQUE)

/*!
 * \brief Return the first node object of a given list.
 */
#define LiLiFirstNode(list) ((list)->lst_head)

/*!
 * \brief Return the last node object of a given list.
 */
#define LiLiLastNode(list) ((list)->lst_tail)

/*!
 * \brief Return the next node object of a given node.
 */
#define LiLiNextNode(node) ((node)->nod_nxt)

/*!
 * \brief Return the previous node object of a given node.
 */
#define LiLiPreviousNode(node) ((node)->nod_prv)

/*!
 * \brief Return the item reference of a given node.
 */
#define LiLiNodeItem(node) ((node)->nod_itm)

/*@}*/

__BEGIN_DECLS
/* Prototypes */

extern LILI *LiLiCreate(uint8_t flags, LiLiItemCreateFunc cre, LiLiItemDestroyFunc des, LiLiItemCompareFunc cmp);
extern void LiLiClean(LILI *list);
extern void LiLiDestroy(LILI *list);
extern int LiLiNodes(LILI *list);

extern int LiLiPushItem(LILI *list, LILI_ITEMREF ref);
extern int LiLiPopItem(LILI *list, LILI_ITEMREF *refp);
extern LILI_NODE *LiLiFindItem(LILI *list, LILI_ITEMREF ref);
extern LILI_NODE *LiLiLocateItem(LILI *list, LILI_ITEMREF ref);

extern LILI_NODE *LiLiInsertItemAfterNode(LILI *list, LILI_NODE *node, LILI_ITEMREF ref);
extern LILI_NODE *LiLiInsertItemBeforeNode(LILI *list, LILI_NODE *node, LILI_ITEMREF ref);
extern void LiLiRemoveNode(LILI *list, LILI_NODE *node);

extern LILI_ITEMREF LiLiCreateStringItemCopy(LILI_ITEMREF ref);
extern void LiLiDestroyStringItemCopy(LILI_ITEMREF ref);
extern int LiLiCompareStringItems(LILI_ITEMREF ref1, LILI_ITEMREF ref2);

__END_DECLS
/* End of prototypes */
#endif
