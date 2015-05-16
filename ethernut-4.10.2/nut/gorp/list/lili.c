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
 * \file gorp/list/lili.c
 * \brief Linked list main functions.
 *
 * \verbatim
 * $Id$
 * \endverbatim
 */

#include <memdebug.h>
#include <sys/nutdebug.h>

#include <stdlib.h>
#include <gorp/lili.h>

/*!
 * \addtogroup xgLili
 */
/*@{*/

/*!
 * \brief Compare two items references.
 *
 * This is the default comparison function used on sorted lists.
 *
 * \param ref1 Reference of the first item.
 * \param ref2 Reference of the second item.
 *
 * \return An integer less than, equal to, or greater than zero, if the
 *         value of the first reference is less than, equal, or greater 
 *         than the value of the second, resp.
 */
static int LiLiDefaultItemCompare(LILI_ITEMREF ref1, LILI_ITEMREF ref2)
{
    return ref1 - ref2;
}

/*
 * \brief Add the first node object to an empty list.
 *
 * \param list Pointer to a list object, obtained from a previous call
 *             to LiLiCreate().
 * \param node Pointer to the node object to add.
 */
static void LiLiNodeAddFirst(LILI *list, LILI_NODE *node)
{
    /* The list must be empty. */
    NUTASSERT(list->lst_head == NULL);
    NUTASSERT(list->lst_tail == NULL);
    /* Insert the first node. */
    list->lst_head = node;
    list->lst_tail = node;
    node->nod_nxt = NULL;
    node->nod_prv = NULL;
}

/*!
 * \brief Remove a given node from the list.
 *
 * Special applications may use this function instead of LiLiPopItem().
 *
 * The caller must make sure, that this node is a member of the specified
 * list. After returning from this function, this pointer is no longer 
 * usable.
 *
 * Note, that this call will not destroy the copy of an associated item 
 * object.
 *
 * \param list Pointer to a list object, obtained from a previous call
 *             to LiLiCreate().
 * \param node Pointer to the node object to remove. Ignored if NULL.
 */
void LiLiRemoveNode(LILI *list, LILI_NODE *node)
{
    /* Sanity checks. */
    NUTASSERT(list != NULL);
    if (node) {
        NUTASSERT(list->lst_head != NULL);
        NUTASSERT(list->lst_tail != NULL);

        /* Remove the forward link. */
        if (node->nod_nxt) {
            node->nod_nxt->nod_prv = node->nod_prv;
        } else {
            list->lst_tail = node->nod_prv;
        }
        /* Remove the backward link. */
        if (node->nod_prv) {
            node->nod_prv->nod_nxt = node->nod_nxt;
        } else {
            list->lst_head = node->nod_nxt;
        }
        free(node);
    }
}

/*
 * \brief Insert a new node object after a given node.
 *
 * Note, that list attributes are ignored. Anyway, special applications may 
 * use this function instead of LiLiPushItem().
 *
 * \param list Pointer to a list object, obtained from a previous call
 *             to LiLiCreate().
 * \param node Pointer to the node after which the a new node will be 
 *             inserted. If the list is empty, this must be set to NULL.
 * \param ref  The reference of the item to associate with the new node.
 *             If a function for creating an item copy has been specified, 
 *             it will be called before inserting the new node.
 *
 * \return Pointer to the new node object or NULL on errors.
 */
LILI_NODE *LiLiInsertItemAfterNode(LILI *list, LILI_NODE *node, LILI_ITEMREF ref)
{
    LILI_NODE *newnode;

    /* Sanity check. */
    NUTASSERT(list != NULL);

    newnode = malloc(sizeof(LILI_NODE));
    if (newnode) {
        /* Optionally create a duplicate of the item. */
        newnode->nod_itm = list->lst_icreate ? list->lst_icreate(ref) : ref;
        if (node) {
            /* A node is provided. The list must have members. */
            NUTASSERT(list->lst_head != NULL);
            NUTASSERT(list->lst_tail != NULL);
            /* Link the new node to the list. */
            newnode->nod_nxt = node->nod_nxt;
            newnode->nod_prv = node;
            node->nod_nxt = newnode;
            if (newnode->nod_nxt) {
                newnode->nod_nxt->nod_prv = newnode;
            }
            if (node == list->lst_tail) {
                list->lst_tail = newnode;
            }
        } else {
            /* No node provided. Add the first node to an empty list. */
            LiLiNodeAddFirst(list, newnode);
        }
    }
    return newnode;
}

/*
 * \brief Insert new data node before a given node.
 *
 * Note, that list attributes are ignored. Anyway, special applications may 
 * use this function instead of LiLiPushItem().
 *
 * \param list Pointer to a list object, obtained from a previous call
 *             to LiLiCreate().
 * \param ref  The reference of the item to associate with the new node.
 *             If a function for creating an item copy has been specified, 
 *             it will be called before inserting the new node.
 *
 * \return Pointer to the new node object or NULL on errors.
 */
LILI_NODE *LiLiInsertItemBeforeNode(LILI *list, LILI_NODE *node, LILI_ITEMREF ref)
{
    LILI_NODE *newnode;

    /* Sanity checks. */
    NUTASSERT(list != NULL);

    newnode = malloc(sizeof(LILI_NODE));
    if (newnode) {
        newnode->nod_itm = list->lst_icreate ? list->lst_icreate(ref) : ref;
        if (node) {
            /* A node is provided. The list must have members. */
            NUTASSERT(list->lst_head != NULL);
            NUTASSERT(list->lst_tail != NULL);
            /* Link the new node to the list. */
            newnode->nod_nxt = node;
            newnode->nod_prv = node->nod_prv;
            if (newnode->nod_prv) {
                newnode->nod_prv->nod_nxt = newnode;
            }
            node->nod_prv = newnode;
            if (node == list->lst_head) {
                list->lst_head = newnode;
            }
        } else {
            /* No node provided. Add the first node to an empty list. */
            LiLiNodeAddFirst(list, newnode);
        }
    }
    return newnode;
}

/*!
 * \brief Create a linked list.
 *
 * \param flags Attribute flags, either LILI_F_LIFO for a last-in
 *              first-out queue, LILI_F_FIFO for a first-in first-out 
 *              queue, or LILI_F_SORT for a sorted list. Any of them
 *              may be or'ed with the attribute flag LILI_F_UNIQUE to
 *              avoid duplicate entries.
 * \param icre  Pointer to a function, which is called to create an
 *              item for adding to the list. If NULL, the item reference 
 *              itself is used in the list. See LiLiCreateStringItemCopy().
 * \param ides  Pointer to a function, which is called to destroy an
 *              item. Set to NULL, if the item reference is used in the 
 *              list. See LiLiDestroyStringItemCopy().
 * \param icmp  Pointer to a function, which is called to compare items.
 *              If NULL, the item reference values are directly compared.
 *              See LiLiCompareStringItems().
 *
 * \return Pointer to a list object on success, otherwise a NULL pointer 
 *         is returned. 
 */
LILI *LiLiCreate(uint8_t flags, LiLiItemCreateFunc icre, LiLiItemDestroyFunc ides, LiLiItemCompareFunc icmp)
{
    LILI *list = calloc(1, sizeof(LILI));

    if (list) {
        list->lst_icreate = icre;
        list->lst_idestroy = ides;
        if (icmp) {
            list->lst_icompare = icmp;
        } else {
            list->lst_icompare = LiLiDefaultItemCompare;
        }
        list->lst_flags = flags;
    }
    return list;
}

/*!
 * \brief Remove all items from a list.
 *
 * This function will not release the list object. Use LiLiDestroy()
 * to release the complete list.
 *
 * \param list Pointer to a list object, obtained from a previous call
 *             to LiLiCreate().
 */
void LiLiClean(LILI *list)
{
    LILI_NODE *node;

    /* Sanity checks. */
    NUTASSERT(list != NULL);

    if (list->lst_head) {
        while ((node = list->lst_head) != NULL) {
            list->lst_head = node->nod_nxt;
            if (list->lst_idestroy) {
                list->lst_idestroy(LiLiNodeItem(node));
            }
            free(node);
        }
        list->lst_tail = NULL;
    }
}

/*!
 * \brief Destroy a linked list.
 *
 * This function will remove all nodes, destroy item object copies and
 * finally destroy the list object.
 *
 * \param list Pointer to a list object to destroy, obtained from a 
 *             previous call to LiLiCreate(),
 */
void LiLiDestroy(LILI *list)
{
    /* Remove all node objects. */
    LiLiClean(list);
    /* Release the list object. */
    free(list);
}

/*@}*/
