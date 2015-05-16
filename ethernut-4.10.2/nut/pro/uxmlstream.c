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
 * \file pro/uxmlstream.c
 * \brief Micro XML stream parser.
 *
 * \verbatim
 * $Id: uxmlstream.c 3452 2011-06-01 08:51:23Z haraldkipp $
 * \endverbatim
 */

#include <sys/types.h>
#include <sys/heap.h>

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <memdebug.h>

#include <pro/uxml.h>

/*!
 * \addtogroup xgUXML
 */
/*@{*/

#ifndef MAX_UXMLTAG_SIZE
/*!
 * \brief Maximum tag size including brackets.
 *
 * Larger tags will be silently discarded.
 */
#define MAX_UXMLTAG_SIZE     512
#endif

#ifndef MAX_UXMLTKN_SIZE
/*!
 * \brief Maximum token size.
 *
 * Larger tokens will be cut to the specified size. This may be fine for
 * attribute values containing lengthy descriptions, but may be disastrous
 * for tag or attribute names.
 */
#define MAX_UXMLTKN_SIZE     64
#endif

static int UxmlReadTag(FILE * stream, char *data, size_t size)
{
    int rc = -1;
    int ch;
    int qc = 0;
    int state = 1;
    char *dp = NULL;

    while (state) {
        ch = fgetc(stream);
        if (ch == EOF || ch == 0) {
            break;
        }
        switch (state) {
        case 1:
            /* Searching first bracket. */
            if (ch == '<') {
                /* Opening bracket found. Start collecting. */
                dp = data;
                state = 4;
            } else if (ch == '"' || ch == '\'') {
                /* Quote found. Skip quoted string. */
                qc = ch;
                state++;
            }
            break;
        case 2:
            /* Skipping quoted string. */
        case 5:
            /* Collecting quoted string. */
            if (ch == qc) {
                /* End quote found. */
                state--;
            }
            break;
        case 3:
            /* Compressing spaces. */
            if (isspace(ch)) {
                ch = 0;
                break;
            }
            state = 4;
            /* Fall through. */
        case 4:
            /* Collecting data. */
            if (ch == '>') {
                rc = 0;
                state = 0;
            } else if (ch == '"' || ch == '\'') {
                qc = ch;
                state++;
            } else if (isspace(ch)) {
                ch = ' ';
                state = 3;
            }
            break;
        }
        if (dp && ch) {
            if (size > 1) {
                size--;
                *dp++ = ch;
            } else {
                break;
            }
        }
    }
    if (dp) {
        *dp = 0;
    }
    return rc;
}

/*!
 * \brief Parse XML stream.
 *
 * This is the main routine of the Micro XML Stream Parser. It reads the
 * XML document from a previously opened stream and creates a
 * corresponding tree structure.
 *
 * Note, that this is a minimal and probably incomplete implementation,
 * which had been specifically created to parse the SHOUTcast radio
 * station list. On the other hand, it offers a practical XML parser
 * solution for embedded systems with very low memory resources. Unlike
 * most other implementations, it does not require to copy the whole XML
 * document into internal memory. Instead, the parser reads and interprets
 * individual tags. Furthermore, the caller may specify tag and attribute
 * filters to reduce the resulting tree size. Be aware, that because of
 * filtering the root of the tree may have siblings.
 *
 * Normally the parser will return when the end of a file is reached.
 * On TCP connections this may be either on connection close or timeout.
 * If closing and re-opening connections may create too much overhead
 * and timeouts are too slow, an EOF (ASCII 0) may be sent alternatively.
 *
 * \param stream The stream to read from.
 * \param f_tags Optional tag filter, which points to an array of tag
 *               names to include. All other tags will be discarded.
 *               This can be used to limit memory consumption of the
 *               tree structure. Note, that this filtering may result
 *               in a tree structure, which is different from the
 *               structure of the original document. Set to NULL to
 *               disable tag filtering.
 *
 * \param f_attr Optional attribute filter, which points to an array of
 *               attribute names to include. All other attributes will be
 *               discarded. Like the tag filter, it can be used to limit
 *               memory consumption of the tree structure. Set to NULL
 *               to disable attribute filtering.
 *
 * \return Pointer to a newly allocated UXML_NODE tree structure. NULL
 *         may be returned in case of an error. The caller should use
 *         UxmlTreeDestroy() to release the memory allocated by the
 *         tree.
 */
UXML_NODE *UxmlParseStream(FILE * stream, char **f_tags, char **f_attr)
{
    char *tag;
    char *tkn;
    char *tp;
    UXML_NODE *root = NULL;
    UXML_NODE *node = NULL;
    UXML_NODE *nn;

    /* Allocate the tag buffer. */
    if ((tag = malloc(MAX_UXMLTAG_SIZE)) == NULL) {
        return NULL;
    }
    /* Allocate the token buffer. */
    if ((tkn = malloc(MAX_UXMLTKN_SIZE)) == NULL) {
        free(tag);
        return NULL;
    }
    for (;;) {
        if (NutHeapAvailable() < 8192) {
            break;
        }
        /* Read the next tag. */
        if (UxmlReadTag(stream, tag, MAX_UXMLTAG_SIZE)) {
            /* No more tags or error. */
            break;
        }
        /* Parse the tag. */
        if ((tp = UxmlParseTag(tag + 1, tkn, MAX_UXMLTKN_SIZE)) != NULL) {
            if (isalpha((unsigned char)*tkn) && UxmlFilterMatch(tkn, f_tags)) {
                /* Save pointer to tp because needed to determine self closing tag */
                char *old_tp = tp;

                /*
                 * New node.
                 */
                if ((nn = UxmlNodeCreate(tkn)) == NULL) {
                    break;
                }
                if (root == NULL) {
                    /* Root entry. */
                    root = nn;
                    node = nn;
                } else if (node == NULL) {
                    /* No active node. Add root siblings. */
                    node = UxmlTreeAddSibling(root, nn);
                } else {
                    /* New node is a child of the currently active one. */
                    node = UxmlTreeAddChild(node, nn);
                }
                /* Parse the attributes. */
                for (;;) {
                    if ((tp = UxmlParseTag(tp, tkn, MAX_UXMLTKN_SIZE)) == NULL || *tkn == '>') {
                        /* End of this tag or error. */
                        break;
                    }
                    if (isalpha((unsigned char)*tkn) && UxmlFilterMatch(tkn, f_attr)) {
                        char *name = strdup(tkn);

                        if (name) {
                            if ((tp = UxmlParseTag(tp, tkn, MAX_UXMLTKN_SIZE)) == NULL || *tkn != '=') {
                                free(name);
                                break;
                            }
                            if ((tp = UxmlParseTag(tp, tkn, MAX_UXMLTKN_SIZE)) == NULL || *tkn == '>') {
                                free(name);
                                break;
                            }
                            UxmlNodeAddAttrib(node, name, tkn);
                            free(name);
                        }
                    }
                }
                /* Check if tag is self closing */
                if (node && strlen(old_tp) > 1 && old_tp[strlen(old_tp) - 2]=='/') {
                    node = node->xmln_parent;
                }
            } else if (*tkn == '/') {
                /*
                 * End of the active node.
                 */
                tp = UxmlParseTag(tp, tkn, MAX_UXMLTKN_SIZE);
                if (tp && node && strcasecmp(node->xmln_name, tkn) == 0) {
                    node = node->xmln_parent;
                }
            }
        }
    }
    /* Clean up. */
    free(tag);
    free(tkn);

    return root;
}

/*@}*/
