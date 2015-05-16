#ifndef _PRO_UXML_H_
#define _PRO_UXML_H_

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

/*!
 * \file pro/uxml.h
 * \brief Micro XML stream parser.
 *
 * \verbatim
 * $Id: uxml.h 2023 2008-02-15 16:46:09Z haraldkipp $
 * \endverbatim
 */

#include <stdio.h>

/*!
 * \brief Linked list of XML attributes.
 */
typedef struct _UXML_ATTRIB {
    struct _UXML_ATTRIB *xmla_next;     /*!< \brief Links the next attribute. */
    char *xmla_name;            /*!< \brief Name of the attribute. */
    char *xmla_value;           /*!< \brief Value of the attribute. */
} UXML_ATTRIB;

/*!
 * \brief Linked list of XML nodes.
 */
typedef struct _UXML_NODE {
    struct _UXML_NODE *xmln_next;       /*!< \brief Pointer to the next sibling node. */
    struct _UXML_NODE *xmln_child;      /*!< \brief Pointer to the first child node. */
    struct _UXML_NODE *xmln_parent;     /*!< \brief Pointer to the parent node. */
    char *xmln_name;            /*!< \brief Name of the tag. */
    UXML_ATTRIB *xmln_attribs;  /*!< \brief Pointer to the linked list of attributes. */
} UXML_NODE;

__BEGIN_DECLS
/* Prototypes */
extern UXML_NODE *UxmlParseStream(FILE * stream, char **f_tags, char **f_attribs);
extern char *UxmlParseTag(char *data, char *tkn, size_t size);

extern UXML_NODE *UxmlTreeAddSibling(UXML_NODE * node, UXML_NODE * sibling);
extern UXML_NODE *UxmlTreeAddChild(UXML_NODE * node, UXML_NODE * child);
extern void UxmlTreeDestroy(UXML_NODE * node);

extern UXML_NODE *UxmlNodeCreate(char *name);
extern int UxmlNodeAddAttrib(UXML_NODE * node, char *name, char *value);

extern int UxmlFilterMatch(char *name, char **filt);

__END_DECLS
/* End of prototypes */
#endif
