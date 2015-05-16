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
 * \file gorp/list/lpushpop.c
 * \brief Linked list push and pop functions.
 *
 * \verbatim
 * $Id$
 * \endverbatim
 */

#include <sys/nutdebug.h>

#include <gorp/lili.h>

/*!
 * \addtogroup xgLili
 */
/*@{*/

/*!
 * \brief Find the node's location by a given item.
 *
 * Like LiLiFindItem(), but returns the first node of which the item is 
 * larger than the given item. This function is useful for sorted lists
 * only.
 *
 * \param list Pointer to a list object, obtained from a previous call
 *             to LiLiCreate().
 * \param ref  Reference of the item to search for.
 *
 * \return Pointer to the node, or NULL if no node was found.
 */
LILI_NODE *LiLiLocateItem(LILI *list, LILI_ITEMREF ref)
{
    LILI_NODE *node;

    /* Sanity checks. */
    NUTASSERT(list != NULL);

    for (node = LiLiFirstNode(list); node; node = LiLiNextNode(node)) {
        if (list->lst_icompare(LiLiNodeItem(node), ref) >= 0) {
            break;
        }
    }
    return node;
}

/*!
 * \brief Find the node of a given item.
 *
 * Like LiLiLocateItem(), but searches for an exact match. If the list
 * is unsorted, all nodes will be scanned.
 *
 * \param list Pointer to a list object, obtained from a previous call
 *             to LiLiCreate().
 * \param ref  Reference of the item to search for.
 *
 * \return Pointer to the node, or NULL if no node was found.
 */
LILI_NODE *LiLiFindItem(LILI *list, LILI_ITEMREF ref)
{
    LILI_NODE *node;
    int rc;

    /* Sanity checks. */
    NUTASSERT(list != NULL);

    for (node = LiLiFirstNode(list); node; node = LiLiNextNode(node)) {
        rc = list->lst_icompare(LiLiNodeItem(node), ref);
        if (rc == 0) {
            break;
        }
        if (rc > 0 && LiLiIsSorted(list)) {
            return NULL;
        }
    }
    return node;
}

/*!
 * \brief Add an item to the list.
 *
 * In most cases this function will be used by applications to add new
 * items to a list.
 *
 * If the attribute LILI_F_SORT has been set when creating the list,
 * then a node will be inserted before the first node, of which the 
 * item is larger or equal than the given one.
 *
 * If the attribute LILI_F_UNIQUE has been set and if a node with
 * the same item is already a member of the list, then no new node
 * will be added. This adds significant overhead to LIFO and FIFO
 * queues.
 *
 * \param list Pointer to a list object, obtained from a previous call
 *             to LiLiCreate().
 * \param ref  The item's reference. If a function for creating an item
 *             has been specified, if will be called before inserting a
 *             new node.
 *
 * \return 0 if a new node had been successfully added or of the given
 *         item already exists in a unique list. On errors, -1 is returned.
 */
int LiLiPushItem(LILI *list, LILI_ITEMREF ref)
{
    LILI_NODE *node;
    int rc;

    /* Sanity checks. */
    NUTASSERT(list != NULL);

    node = LiLiLastNode(list);
    if (node) {
        if (LiLiIsSorted(list)) {
            /* A sorted list, compare the new with the last item. */
            rc = list->lst_icompare(LiLiNodeItem(node), ref);
            if (rc <= 0) {
                /* The new item is greater or equal, so we should append 
                ** it. But only, if either the items are not equal or if
                ** this is not a list of unique items only. */
                if (rc || !LiLiHasUniqueItems(list)) {
                    rc = LiLiInsertItemAfterNode(list, node, ref) ? 0 : -1;
                }
                return rc;
            }
            /* The new item is lower than the last one in the list. Search
            ** the first node with an item greater or equal than the new one. */
            node = LiLiLocateItem(list, ref);
            /* If the list has unique items only, then ignore an equal item. */
            if (LiLiHasUniqueItems(list) && list->lst_icompare(LiLiNodeItem(node), ref) == 0) {
                return 0;
            }
        } else {
            /* Do not insert an equal item into a unique list. */
            if (LiLiHasUniqueItems(list) && LiLiFindItem(list, ref)) {
                return 0;
            }
            /* If not a sorted list, add new items in front. */
            node = LiLiFirstNode(list);
        }
    }
    /* Add a new node. */
    return LiLiInsertItemBeforeNode(list, node, ref) ? 0 : -1;
}

/*!
 * \brief Remove the next item from the list.
 *
 * \param list Pointer to a list object, obtained from a previous call
 *             to LiLiCreate().
 * \param refp Pointer that receives the item's reference of the removed 
 *             node. If a copy of the item object has been created during 
 *             list insertion, then the caller is responsible for destroying
 *             it.
 *
 * \return -1 if the list is empty, otherwise 0 is returned.
 */
int LiLiPopItem(LILI *list, LILI_ITEMREF *refp)
{
    LILI_NODE *node;

    /* Sanity checks. */
    NUTASSERT(list != NULL);

    /* Get the first node, check if the list is empty. */
    node = LiLiFirstNode(list);
    if (node) {
        /* List is not empty. However, if this is a FIFO queue, remove 
        ** the last node instead. */
        if (LiLiIsFifo(list)) {
            node = LiLiLastNode(list);
        }
        /* Pass the item reference to the caller and remove the node. */
        *refp = LiLiNodeItem(node);
        LiLiRemoveNode(list, node);
    }
    return node ? 0 : -1;
}

/*@}*/
