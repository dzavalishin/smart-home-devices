/*
 * Copyright (C) 2008 by egnite GmbH. All rights reserved.
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
 *
 */

/*
 * \file pro/uxmltree.c
 * \brief Micro XML tree support functions.
 *
 * \verbatim
 * $Id: uxmltree.c 3021 2010-06-22 18:45:33Z thiagocorrea $
 * \endverbatim
 */

#include <sys/types.h>

#include <stdlib.h>
#include <string.h>
#include <memdebug.h>

#include <pro/uxml.h>

/*!
 * \addtogroup xgUXML
 */
/*@{*/

/*!
 * \brief Allocate a new tree node.
 *
 * \param name Name of the node to add.
 *
 * \return Pointer to the new node. In case of an error, NULL is returned.
 */
UXML_NODE *UxmlNodeCreate(char *name)
{
    UXML_NODE *node;
    size_t nlen;

    if ((node = malloc(sizeof(UXML_NODE))) != NULL) {
        memset(node, 0, sizeof(UXML_NODE));
        nlen = strlen(name) + 1;
        if ((node->xmln_name = malloc(nlen)) != NULL) {
            memcpy(node->xmln_name, name, nlen);
        }
    }
    return node;
}

/*!
 * \brief Add an attribute to the specified node.
 *
 * \param node  The attribute is added to this node.
 * \param name  Attribute's name.
 * \param value Attribute's value.
 *
 * \return 0 on success, -1 otherwise.
 */
int UxmlNodeAddAttrib(UXML_NODE * node, char *name, char *value)
{
    UXML_ATTRIB *attr;
    UXML_ATTRIB *ap;

    attr = malloc(sizeof(UXML_ATTRIB));
    if (attr) {
        attr->xmla_next = NULL;
        attr->xmla_name = strdup(name);
        if (attr->xmla_name) {
            attr->xmla_value = strdup(value);
            if (attr->xmla_value) {
                if (node->xmln_attribs == NULL) {
                    node->xmln_attribs = attr;
                } else {
                    ap = node->xmln_attribs;
                    for (;;) {
                        if (ap->xmla_next == NULL) {
                            ap->xmla_next = attr;
                            break;
                        }
                        ap = ap->xmla_next;
                    }
                }
                return 0;
            }
            free(attr->xmla_name);
        }
        free(attr);
    }
    return -1;
}

static void UxmlNodeDestroy(UXML_NODE * node)
{
    UXML_ATTRIB *ap;
    UXML_ATTRIB *attr;

    if (node) {
        if (node->xmln_name) {
            free(node->xmln_name);
        }
        ap = node->xmln_attribs;
        while (ap) {
            attr = ap;
            ap = ap->xmla_next;
            if (attr->xmla_name) {
                free(attr->xmla_name);
            }
            if (attr->xmla_value) {
                free(attr->xmla_value);
            }
            free(attr);
        }
        free(node);
    }
}

/*!
 * \brief Add a sibling to the specified node.
 *
 * \param node  The sibling is added to this node.
 * \param child Sibling to add.
 *
 * \return Pointer to the sibling node.
 */
UXML_NODE *UxmlTreeAddSibling(UXML_NODE * node, UXML_NODE * sibling)
{
    for (;;) {
        if (node->xmln_next == NULL) {
            node->xmln_next = sibling;
            sibling->xmln_parent = node->xmln_parent;
            break;
        }
        node = node->xmln_next;
    }
    return sibling;
}

/*!
 * \brief Add a child to the specified node.
 *
 * \param node  The child is added to this node.
 * \param child Child to add.
 *
 * \return Pointer to the child node.
 */
UXML_NODE *UxmlTreeAddChild(UXML_NODE * node, UXML_NODE * child)
{
    if (node->xmln_child == NULL) {
        node->xmln_child = child;
        child->xmln_parent = node;
    } else {
        UxmlTreeAddSibling(node->xmln_child, child);
    }
    return child;
}

/*!
 * \brief Release all memory of an XML tree structure.
 *
 * \param node Pointer to the root entry.
 */
void UxmlTreeDestroy(UXML_NODE * node)
{
    UXML_NODE *np = node;

    while (np) {
        node = np;
        np = np->xmln_next;
        if (node->xmln_child) {
            UxmlTreeDestroy(node->xmln_child);
        }
        UxmlNodeDestroy(node);
    }
}

/*@}*/
