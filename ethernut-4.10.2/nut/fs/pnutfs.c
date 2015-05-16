/*
 * Copyright (C) 2004-2006 by egnite Software GmbH. All rights reserved.
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
 */

/*!
 * \file fs/pnutfs.c
 * \brief Peanut File System.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.16  2009/02/13 14:52:05  haraldkipp
 * Include memdebug.h for heap management debugging support.
 *
 * Revision 1.15  2009/01/17 11:26:46  haraldkipp
 * Getting rid of two remaining BSD types in favor of stdint.
 * Replaced 'u_int' by 'unsinged int' and 'uptr_t' by 'uintptr_t'.
 *
 * Revision 1.14  2008/08/27 06:45:23  thornen
 * Added bank support for MMnet03..04 MMnet102..104
 *
 * Revision 1.13  2008/08/26 17:36:45  haraldkipp
 * Revoked changes 2008/08/26 by thornen.
 *
 * Revision 1.11  2008/08/11 06:59:42  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.10  2006/08/01 07:43:48  haraldkipp
 * PNUT file system failed after some modifications done previously for the
 * PHAT file system. During directory open, the NUTFILE structure must be
 * allocated in the file system driver. PnutDirRead() must return -1 if the
 * end of a directory is reached. Reading unused directory entries must update
 * the file position pointer.
 *
 * Revision 1.9  2006/03/02 20:01:17  haraldkipp
 * Added implementation of dev_size makes _filelength() work, which in turn
 * enables the use of this file system in pro/httpd.c.
 *
 * Revision 1.8  2006/01/05 16:45:20  haraldkipp
 * Dynamic NUTFILE allocation for detached block device.
 *
 * Revision 1.7  2005/09/08 10:12:44  olereinhardt
 * Added #ifdef statement in NutSegBufEnable to avoid compiler warning
 * if no banked mem is used.
 *
 * Revision 1.6  2005/09/07 16:23:41  christianwelzel
 * Added support for MMnet02. Bankswitching is now handled in bankmem.h
 *
 * Revision 1.5  2005/08/02 17:46:47  haraldkipp
 * Major API documentation update.
 *
 * Revision 1.4  2005/05/16 08:33:59  haraldkipp
 * Added banking support for Arthernet.
 *
 * Revision 1.3  2005/02/21 11:10:21  olereinhardt
 * Changed deckaration of the "root" variable to compile with unix emulation
 *
 * Revision 1.2  2005/02/07 18:57:47  haraldkipp
 * ICCAVR compile errors fixed
 *
 * Revision 1.1  2005/02/05 20:35:21  haraldkipp
 * Peanut added
 *
 * \endverbatim
 */

#include <compiler.h>

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <memdebug.h>

#include <sys/stat.h>
#include <sys/file.h>
#include <sys/bankmem.h>

#include <fs/fs.h>
#include <dev/pnut.h>

/*!
 * \addtogroup xgPNut
 */
/*@{*/

/*!
 * \name Peanut File System Configuration
 *
 * The Nut/OS Configurator may be used to override the default values.
 */
/*@{*/

/*! \brief Size of a filesystem block. 
 *
 * \showinitializer
 */
#ifndef PNUT_BLOCK_SIZE
#define PNUT_BLOCK_SIZE     512
#endif

/*! 
 * \brief Size of a directory entry.
 *
 * 
 * \showinitializer
 */
#ifndef PNUT_DIRENT_SIZE
#define PNUT_DIRENT_SIZE  32
#endif

/*! 
 * \brief Maximum number of blocks per node.
 *
 * Peanut supports only one node per file. Thus, this number
 * multiplied by PNUT_BLOCK_SIZE specifies the maximum file
 * size.
 *
 * Changings this number will change the size of the node
 * structure, which must fit into a single filesystem block.
 * 
 * \showinitializer
 */
#ifndef PNUT_BLOCKS_PER_NODE
#define PNUT_BLOCKS_PER_NODE      250
#endif

#ifndef PNUTBANK_COUNT
#ifdef ARTHERNET1
/* Default for Arthernet 1. */
#define PNUTBANK_COUNT 15
#elif MMNET02  || MMNET03  || MMNET04 ||\
	  MMNET102 || MMNET103 || MMNET104  
/* Default for MMnte02. */
#define PNUTBANK_COUNT 6
#else
/* Default for Ethernet 2. */
#define PNUTBANK_COUNT 30
#endif
#endif

/*@}*/


#ifndef SEEK_SET
#  define SEEK_SET        0     /* Seek from beginning of file.  */
#  define SEEK_CUR        1     /* Seek from current position.  */
#  define SEEK_END        2     /* Set file pointer to EOF plus "offset" */
#endif

#define NODETYPE_REG        0
#define NODETYPE_DIR        1


typedef short PNUT_BLKNUM;

/*!
 * \brief Node structure.
 *
 * The Peanut filesystem divides the total space into blocks. It
 * distinguishes between unallocated blocks, data blocks and blocks
 * containing node informations. Each allocated data block is 
 * associated to a node block. Unallocated blocks are collected
 * in a linked list.
 *
 * Peanut knows two types of nodes, file nodes and directory nodes.
 * Each file node represents a file entry and the associated data 
 * blocks contain the contents of this file. Each directory node
 * represents a directory and the associated data blocks contain
 * the entries in this directory. Each directory entry in use
 * contains a link to to a node block.
 *
 * \note The node structure must fit into a block.
 */
typedef struct {
    uint8_t node_type;           /*!< Type of this node, 0=file, 1=dir */
    uint8_t node_refs;           /*!< Reference counter: Number of active open calls. */
    /*!
     * \brief Number of links.
     *
     * For file nodes this number keeps track of the number of directory 
     * entries pointing to it. PNUT currently doesn't support link type
     * entries, thus in file entries this is always 1.
     *
     * For directory nodes the number represents the number
     * of subdirectories plus 2.
     */
    uint16_t node_links;
    uint32_t node_size;           /*!< Total size of the data area. */
    time_t node_mtime;          /*!< Time of last modification. */
    PNUT_BLKNUM node_blocks[PNUT_BLOCKS_PER_NODE];      /*!< Allocated data blocks. */
} PNUT_NODE;

/*!
 * \brief Maximum length of a base name.
 *
 * This is a calculated value and depends on the definition of the
 * block size and the size of a directory entry.
 */
#define PNUT_MAX_NAMELEN    (PNUT_DIRENT_SIZE - sizeof(PNUT_BLKNUM) - sizeof(uint8_t) - 1)

/*!
 * \brief Directory entry structure.
 *
 * Each directory node contains a list of data blocks. Each of these
 * data blocks contains a number of directory entries. Each directory
 * entry points to a node block, which may be a file node or a
 * directory node.
 */
typedef struct {
    /*!
     * \brief Node block index of this entry.
     *
     * This may point to a file or directory node.
     */
    PNUT_BLKNUM dir_node;
    /*!
     * \brief Allocation flag.
     *
     * Set to one if this entry is in use.
     */
    uint8_t dir_inuse;
    /*!
     * \brief Name of this entry.
     *
     * Contains the name ot the file or directory.
     */
    char dir_name[PNUT_MAX_NAMELEN + 1];
} PNUT_DIRENTRY;

/*! 
 * \brief Maximum size  of a file or directory.
 *
 * This is a calculated value and depends on the definition of the
 * block size and the number of blocks per directory entry.
 */
#define PNUT_MAX_FILESIZE    (PNUT_BLOCKS_PER_NODE * PNUT_BLOCK_SIZE)

/*!
 * \brief Directory search result information structure.
 */
typedef struct {
    /*!
     * \brief Node of the entry found. 
     */
    PNUT_BLKNUM fr_node;

    /*!
     * \brief Parent node of the entry found.
     */
    PNUT_BLKNUM fr_pnode;

    /*! 
     * \brief Last path component.
     *
     * Set if all parents found. 
     */
    CONST char *fr_name;
} PNUT_FINDRESULT;

/*!
 * \brief PNUT file descriptor type.
 */
typedef struct _PNUTFILE PNUTFILE;

/*!
 * \brief PNUT file descriptor structure.
 */
struct _PNUTFILE {
    PNUT_BLKNUM f_node;         /* Node of the file or directory. */
    uint32_t f_pos;               /* Current file pointer position. */
    unsigned int f_flag;               /* File mode. */
};

/*! \brief Root node of the filesystem. */
static PNUT_BLKNUM root;

/* -------------------- Banking hardware routines ------------------- */

/*! \brief Size of each memory bank. */
#ifndef NUTBANK_SIZE
#define NUTBANK_SIZE 16384
#endif

/*! 
 * \brief Total number of blocks on this device.
 *
 * This value is calulated by multiplying the number of memory banks 
 * reserved for the file system by the number of blocks per bank.
 *
 * For example, if all 30 banks, which are available on Ethernut 2, are 
 * reserved for PNUT and if the size of a block is 512 bytes, then 
 * 960 blocks are available.
 */
#ifndef PNUT_TOTAL_BLOCKS
#define PNUT_TOTAL_BLOCKS   (PNUTBANK_COUNT * (NUTBANK_SIZE / PNUT_BLOCK_SIZE))
#endif

#define BLOCKS_PER_BANK  (NUTBANK_SIZE / PNUT_BLOCK_SIZE)

#ifndef NUTBANK_SR
#define NUTBANK_SR 0xFF00
#endif

#ifndef NUTBANK_START
#define NUTBANK_START 0x8000
#endif

#define NUTBANK_PTR ((char *)NUTBANK_START)

/*!
 * \brief Make the bank visible, which contains the specified block.
 *
 * On Ethernut 2 we select a bank by writing the bank number to
 * the bank register base address plus the bank number.
 *
 * For example, writing 0x12 to 0xFF12 selects bank 18 (0x12),
 * assuming that 0xFF00 is the bank register base address.
 *
 * \param blk Block number to access.
 */
void BankSelect(PNUT_BLKNUM blk)
{

// This is a hack to avoid compiler warning if no banking is enabled... 
// But I don't like moving code to header files at all.. (Ole Reinhardt)
#if NUTBANK_COUNT 
    int bank = blk / BLOCKS_PER_BANK;
#endif
    // Bankswitching is now handled in bankmem.h
    NutSegBufEnable(bank);
}

/*!
 * \brief Select specified bank and return pointer to block.
 */
PNUT_NODE *BankNodePointer(PNUT_BLKNUM blk)
{
    if (blk < 0) {
        return NULL;
    }
    BankSelect(blk);
    return (PNUT_NODE *) & NUTBANK_PTR[(blk % BLOCKS_PER_BANK) * PNUT_BLOCK_SIZE];
}


/* ------------------------ Block routines ------------------------ */

/*
 * \brief Index of the first free block.
 *
 * All free blocks are collected in a linked list. This global variable 
 * contains the index of the first free block. Each free block contains 
 * nothing but the index of the next free block, which is -1 in the last 
 * member of this list.
 *
 * If this variable is set to -1, then there are no more free blocks 
 * available. During file system initialization all available blocks are 
 * added to this list.
 */
static PNUT_BLKNUM blockFreeList = -1;

/*!
 * \brief Allocate a block.
 *
 * Removes the block in front of the list of free blocks.
 *
 * \return Index of the free block or -1 if none is available.
 */
static PNUT_BLKNUM PnutBlockAlloc(void)
{
    PNUT_BLKNUM rc = blockFreeList;

    if (rc >= 0) {
        PNUT_BLKNUM *bankptr = (PNUT_BLKNUM *) BankNodePointer(blockFreeList);
        blockFreeList = *bankptr;
        /* Clear the block contents. */
        memset(bankptr, 0, PNUT_BLOCK_SIZE);
    }
    return rc;
}

/*!
 * \brief Release a block.
 *
 * Inserts a given block in front of the list of free blocks.
 *
 * \param blk Index of the block to be released.
 */
static void PnutBlockRelease(PNUT_BLKNUM blk)
{
    PNUT_BLKNUM *bankptr = (PNUT_BLKNUM *) BankNodePointer(blk);

    *bankptr = blockFreeList;
    blockFreeList = blk;
}

/* ------------------------ Node routines ------------------------ */

/*!
 * \brief Allocate a node.
 *
 * Creates a new node of a specified type.
 *
 * \param type Type of this node, either NODETYPE_REG or NODETYPE_DIR.
 *
 * \return Block index of this node or -1 if there are no more blocks.
 */
static PNUT_BLKNUM PnutNodeAlloc(uint8_t type)
{
    PNUT_BLKNUM node = PnutBlockAlloc();

    if (node >= 0) {
        int i;
        PNUT_NODE *np = BankNodePointer(node);

        /* Clear the data block list of this node. */
        for (i = 0; i < PNUT_BLOCKS_PER_NODE; i++) {
            np->node_blocks[i] = -1;
        }

        /* Set the type and the last modification date. */
        np->node_type = type;
        np->node_mtime = time(0);
    }
    return node;
}

/*
 * \brief Free all blocks of a specified node.
 */
static void PnutNodeDataRelease(PNUT_BLKNUM node)
{
    int i;

    for (i = 0; i < PNUT_BLOCKS_PER_NODE; i++) {
        if (BankNodePointer(node)->node_blocks[i] >= 0) {
            PnutBlockRelease(BankNodePointer(node)->node_blocks[i]);
            BankNodePointer(node)->node_blocks[i] = -1;
        }
    }
    BankNodePointer(node)->node_size = 0;
    BankNodePointer(node)->node_mtime = time(0);
}

/*!
 * \brief Release a node and all its data blocks.
 */
static void PnutNodeRelease(PNUT_BLKNUM node)
{
    PnutNodeDataRelease(node);
    PnutBlockRelease(node);
}

/*!
 * \brief Retrieve a pointer into a node's data block.
 *
 * This function also returns the number of bytes available within
 * the data block at the retrieved pointer position.
 *
 * \param node   Node number.
 * \param pos    Data position.
 * \param buffer Receives the pointer to the specified position.
 * \param size   Receives the remaining number of bytes within the block.
 * \param create If the position is beyond the current data size and if
 *               this parameter is not equal zero, then a new data block 
 *               will be allocated.
 *
 * \return 0 on success, otherwise returns an error code.
 */
static int PnutNodeGetDataPtr(PNUT_BLKNUM node, uint32_t pos, uint8_t **buffer, size_t * size, int create)
{
    int blkidx;                 /* Number of full blocks */
    int rc = EINVAL;

    *buffer = NULL;
    *size = 0;

    /* Calculate the block index. Make sure it fits in our maximum file size. */
    if ((blkidx = pos / PNUT_BLOCK_SIZE) < PNUT_BLOCKS_PER_NODE) {
        PNUT_BLKNUM blk;

        /* Get the data block number. Optionally allocate a new block. */
        if ((blk = BankNodePointer(node)->node_blocks[blkidx]) < 0 && create) {
            if ((blk = PnutBlockAlloc()) < 0) {
                rc = ENOSPC;
            } else {
                BankNodePointer(node)->node_blocks[blkidx] = blk;
            }
        }

        /* 
         * If we got a valid block, then set the pointer at the specified
         * position and the remaining bytes within this block.
         */
        if (blk >= 0) {
            uint8_t *blkptr = (uint8_t *) BankNodePointer(blk);
            int blkpos = pos % PNUT_BLOCK_SIZE; /* Position within block */

            *buffer = blkptr + blkpos;
            *size = PNUT_BLOCK_SIZE - blkpos;
            rc = 0;
        }
    }
    return rc;
}


/* ------------------------ Directory routines ------------------------ */

/*!
 * \brief Check if a directory is empty.
 */
static int PnutDirIsEmpty(PNUT_BLKNUM node)
{
    int rc = 1;
    uint32_t pos;
    size_t size;
    PNUT_DIRENTRY *entry = NULL;

    /* Loop through the data blocks of this directory node. */
    for (pos = 0; pos < BankNodePointer(node)->node_size; pos += PNUT_DIRENT_SIZE) {

        /* Get the pointer to the next directory entry. */
        uint8_t *ptr = (uint8_t *)entry;
        if (PnutNodeGetDataPtr(node, pos, &ptr, &size, 0) || size == 0) {
            /* No more entries. */
            break;
        }
        entry = (PNUT_DIRENTRY *)ptr;

        if (size >= PNUT_DIRENT_SIZE) {
            if (entry->dir_inuse && strcmp(entry->dir_name, ".") && strcmp(entry->dir_name, "..")) {
                /* Entry found */
                rc = 0;
                break;
            }
        }
    }
    return rc;
}

/*!
 * \brief Find a directory entry with a specified name.
 *
 * \param node  Node of the directory to search.
 * \param path  Path name of the entry to retrieve.
 * \param len   Number of characters in the path name to consider,
 *              starting from the beginning. If the path name
 *              contains more than one component, the caller must
 *              set this parameter to the length of the first one.
 * \param entry Receives the pointer to the directory entry.
 *
 * \return 0 on succes, otherwise an error code is returned.
 */
static int PnutDirFindEntry(PNUT_BLKNUM node, CONST char *path, size_t len, PNUT_DIRENTRY ** entry)
{
    int rc = ENOENT;
    uint32_t pos;
    size_t size;

    /* Loop through the data blocks of this directory node. */
    for (pos = 0; pos < BankNodePointer(node)->node_size; pos += PNUT_DIRENT_SIZE) {

        /* Get the pointer to the next directory entry. */
        uint8_t *ptr = (uint8_t *)(*entry);
        if (PnutNodeGetDataPtr(node, pos, &ptr, &size, 0) || size == 0) {
            /* No more entries. */
            break;
        }
        *entry = (PNUT_DIRENTRY *)ptr;

        /* Compare this entry. */
        if (size >= PNUT_DIRENT_SIZE) { /* Skip entries across block boundaries. */
            if ((*entry)->dir_inuse &&  /* Skip unused entries. */
                strlen((*entry)->dir_name) == len &&    /* Skip non-matching names. */
                strncmp((*entry)->dir_name, path, len) == 0) {
                /* Requested entry found */
                rc = 0;
                break;
            }
        }
    }
    return rc;
}

/*
 * \brief Find directory entry of a specified path.
 *
 * \param node   First node block to search.
 * \param path   Path to find.
 * \param result Search result.
 *
 * \return Error code.
 */
static int PnutDirFindPath(PNUT_BLKNUM node, CONST char *path, PNUT_FINDRESULT * result)
{
    int rc = 0;
    size_t len;
    PNUT_DIRENTRY *entry = NULL;

    result->fr_pnode = node;
    result->fr_node = -1;
    result->fr_name = NULL;

    while (*path == '/') {
        path++;
    }

    if (*path == 0) {
        path = ".";
    }

    /* 
     * Loop for each path component.
     */
    while (*path) {
        CONST char *cp;

        /* Make sure that this is a directory node. */
        if (BankNodePointer(node)->node_type != NODETYPE_DIR) {
            rc = ENOTDIR;
            break;
        }

        /* Retrieve the length of first path component. */
        for (len = 0, cp = path; *cp && *cp != '/'; cp++) {
            len++;
        }

        /* 
         * If this component is last, we found all parents.
         * Keep the last one stored in the result.
         */
        if (*cp == 0) {
            result->fr_name = path;
        }

        /* Try to find this component. */
        if ((rc = PnutDirFindEntry(node, path, len, &entry)) != 0) {
            rc = ENOENT;
            break;
        }

        /* Component found. Return if this was the last one. */
        result->fr_node = entry->dir_node;
        if (*cp == 0) {
            break;
        }

        /* Not the last component. Store node as previous node. */
        result->fr_pnode = result->fr_node;

        /* Move forward to the next path component. */
        node = result->fr_node;
        path += len;
        while (*path == '/') {
            path++;
        }
    }
    return rc;
}


/*!
 * \brief Open a directory.
 *
 * \param dev Specifies the file system device.
 * \param dir Pointer to the internal directory information structure.
 *            The pathname of the directory to open must have been
 *            copied to the data buffer prior calling this routine.
 *
 * \return 0 on success, -1 otherwise.
 */
static int PnutDirOpen(NUTDEVICE * dev, DIR * dir)
{
    PNUTFILE *fp;
    PNUT_FINDRESULT found;
    int rc;

    /* Locate the entry in our directory structure. */
    if ((rc = PnutDirFindPath(root, dir->dd_buf, &found)) != 0) {
        errno = rc;
        rc = -1;
    } else {
        if (BankNodePointer(found.fr_node)->node_type != NODETYPE_DIR) {
            errno = ENOTDIR;
            rc = -1;
        }
        /* Allocate a PNUT file descriptor. */
        else if ((fp = malloc(sizeof(PNUTFILE))) == 0) {
            rc = -1;
        }
        /* 
         * Initialize the descriptor and store it in the directory
         * information structure.
         */
        else {
            fp->f_node = found.fr_node;
            fp->f_pos = 0;

            if ((dir->dd_fd = malloc(sizeof(NUTFILE))) == 0) {
                free(fp);
                rc = -1;
            }
            else {
                memset(dir->dd_fd, 0, sizeof(NUTFILE));
                dir->dd_fd->nf_dev = dev;
                dir->dd_fd->nf_fcb = fp;
                /* Keep track of the number of open calls. */
                BankNodePointer(fp->f_node)->node_refs++;
            }
        }
    }
    return rc;
}

/*!
 * \brief Close a directory.
 */
static int PnutDirClose(DIR * dir)
{
    if (dir && dir->dd_fd) {
        if (dir->dd_fd->nf_fcb) {
            PNUTFILE *fp = dir->dd_fd->nf_fcb;

            BankNodePointer(fp->f_node)->node_refs--;
            free(fp);
        }
        free(dir->dd_fd);
        return 0;
    }
    return EINVAL;
}

/*!
 * \brief Read the next directory entry.
 */
static int PnutDirRead(DIR * dir)
{
    int rc = -1;
    uint32_t pos;
    PNUT_DIRENTRY *entry = NULL;
    size_t size;
    PNUTFILE *fp = dir->dd_fd->nf_fcb;
    struct dirent *ent = (struct dirent *) dir->dd_buf;

    ent->d_name[0] = 0;
    for (pos = fp->f_pos; pos < BankNodePointer(fp->f_node)->node_size; pos += PNUT_DIRENT_SIZE) {
        uint8_t *ptr = (uint8_t *)entry;
        /* Retrieve the entry at the current position. */
        rc = PnutNodeGetDataPtr(fp->f_node, pos, &ptr, &size, 0);
        if (rc || size == 0) {
            /* No more entries. */
            rc = -1;
            break;
        }
        entry = (PNUT_DIRENTRY *)ptr;
        fp->f_pos = pos + PNUT_DIRENT_SIZE;

        /* Skip entries across block boundaries and unused entires. */
        if (size >= PNUT_DIRENT_SIZE && entry->dir_inuse) {
            memset(ent, 0, sizeof(struct dirent));
            ent->d_fileno = entry->dir_node;
            ent->d_namlen = (uint8_t) strlen(entry->dir_name);
            strcpy(ent->d_name, entry->dir_name);
            break;
        }
    }
    return rc;
}

/*!
 * \brief Add directory entry.
 *
 * \param dnode The entry is added to this directory node.
 * \param name  The entry's name.
 * \param enode  The entry's node.
 *
 * \return 0 if successful. Otherwise returns an error code.
 */
static int PnutDirAddEntry(PNUT_BLKNUM dnode, CONST char *name, PNUT_BLKNUM enode)
{
    int rc = 0;
    uint32_t pos = 0;
    size_t size;
    PNUT_DIRENTRY *entry = NULL;
    PNUT_NODE *np;

    /*
     * Loop through all directory entries until an unused one
     * is found. If required, a new data block is allocated,
     * so success is guaranteed unless we run out of free
     * blocks.
     */
    for (;;) {
        /* 
         * Get the data pointer to the specified position. Automatically
         * create a new block if we reached the end of the data.
         */
        uint8_t *ptr = (uint8_t *)entry;
        if ((rc = PnutNodeGetDataPtr(dnode, pos, &ptr, &size, 1)) != 0) {
            break;
        }
        entry = (PNUT_DIRENTRY *)ptr;
        pos += PNUT_DIRENT_SIZE;

        /* Process entry if it doesn't cross block boundaries. */
        if (size >= PNUT_DIRENT_SIZE) {
            /* Did we find a previously released or a newly allocated entry? */
            if (entry->dir_inuse == 0) {
                /* Update the entry. */
                entry->dir_node = enode;
                entry->dir_inuse = 1;
                strcpy(entry->dir_name, name);

                /* Update the directory node. */
                np = BankNodePointer(dnode);
                np->node_mtime = time(0);
                if (pos > np->node_size) {
                    np->node_size = pos;
                }

                /* Update the entry's node. */
                np = BankNodePointer(enode);
                np->node_links++;
                break;
            }
        }
    }
    return rc;
}

/*!
 * \brief Remove a directory entry.
 *
 * Entries referring to nodes with active open calls are not deleted.
 *
 * Entries referring to directory nodes with more than two links left
 * are not deleted.
 *
 * File nodes are released if the entry to remove was the last link
 * to them.
 *
 * \param node Remove the entry from the directory node in this block.
 * \param name Name of the entry to be removed.
 *
 * \return 0 if successful. Otherwise returns an error code.
 */
static int PnutDirDelEntry(PNUT_BLKNUM node, CONST char *name)
{
    int rc;
    PNUT_DIRENTRY *entry = NULL;
    PNUT_NODE *rnp;
    PNUT_BLKNUM rnode;

    /* Make sure this entry exists. */
    if ((rc = PnutDirFindEntry(node, name, strlen(name), &entry)) != 0) {
        return rc;
    }

    /* The node to remove. */
    rnode = entry->dir_node;
    rnp = BankNodePointer(rnode);

    /* Make sure that this node has no pending open calls. */
    if (rnp->node_refs) {
        return EACCES;
    }

    /* We only remove empty directories. */
    if (rnp->node_type == NODETYPE_DIR) {
        if (rnp->node_links > 2 || !PnutDirIsEmpty(rnode)) {
            return EACCES;
        }
        rnp = BankNodePointer(node);
        rnp->node_links--;
        PnutNodeRelease(rnode);
    }

    /* Remove a file. */
    else {
        PnutNodeRelease(rnode);
    }

    /* Mark this entry unused. */
    PnutDirFindEntry(node, name, strlen(name), &entry);
    entry->dir_inuse = 0;

    return 0;
}

/*!
 * \brief Create a new subdirectory.
 *
 * \param path Full path to the directory.
 *
 * \return 0 on success. Otherwise -1 is returned.
 */
static int PnutDirCreate(CONST char *path)
{
    PNUT_BLKNUM node;
    PNUT_FINDRESULT found;
    int ec;

    /* Return an error if this entry already exists. */
    if ((ec = PnutDirFindPath(root, path, &found)) == 0) {
        ec = EEXIST;
    }

    /* If this component does not exist, we can create it... */
    else if (ec == ENOENT) {
        /* ...provided that all parents had been found. */
        if (found.fr_name) {
            /* Allocate a new node block. */
            if ((node = PnutNodeAlloc(NODETYPE_DIR)) < 0) {
                ec = ENOSPC;
            }
            /* Create a reference to itself. */
            else if ((ec = PnutDirAddEntry(node, ".", node)) != 0) {
                PnutNodeRelease(node);
            }
            /* Create a reference to the parent. */
            else if ((ec = PnutDirAddEntry(node, "..", found.fr_pnode)) != 0) {
                PnutNodeRelease(node);
            }
            /* Create a reference in the parent. */
            else if ((ec = PnutDirAddEntry(found.fr_pnode, found.fr_name, node)) != 0) {
                PnutNodeRelease(node);
            }
        }
    }

    /* Something went wrong. */
    if (ec) {
        errno = ec;
        return -1;
    }
    return 0;
}

/* ------------------------ File routines ------------------------ */

/*!
 * \brief Open a file.
 *
 * This function is called by the low level open routine of the C runtime 
 * library, using the _NUTDEVICE::dev_open entry.
 *
 * \param dev  Pointer to the NUTDEVICE structure.
 * \param path Pathname of the file to open.
 * \param mode Operation mode. Any of the following values may be or-ed:
 * - \ref _O_RDONLY
 * - \ref _O_WRONLY
 * - \ref _O_RDWR
 * - \ref _O_APPEND
 * - \ref _O_CREAT
 * - \ref _O_TRUNC
 * - \ref _O_EXCL
 * - \ref _O_TEXT
 * - \ref _O_BINARY
 * \param acc Ignored, should be zero.
 *
 * \return Pointer to a NUTFILE structure if successful or NUTFILE_EOF otherwise.
 */
static NUTFILE *PnutFileOpen(NUTDEVICE * dev, CONST char *path, int mode, int acc)
{
    PNUT_BLKNUM node = -1;
    PNUT_FINDRESULT found;
    int rc;
    PNUTFILE *file;
    NUTFILE *nfp = NUTFILE_EOF;

    /* Try to find an exisiting file. */
    if ((rc = PnutDirFindPath(root, path, &found)) == 0) {
        /* Return an error, if this is no regular file. */
        if (BankNodePointer(found.fr_node)->node_type != NODETYPE_REG) {
            errno = EISDIR;
        }

        /*
         * We return an error, if the file exists and _O_EXCL has been
         * set with _O_CREAT.
         */
        else if ((mode & (_O_CREAT | _O_EXCL)) == (_O_CREAT | _O_EXCL)) {
            errno = EEXIST;
        } else {
            node = found.fr_node;
            if (mode & _O_TRUNC) {
                PnutNodeDataRelease(node);
            }
        }
    }

    else if (rc == ENOENT) {
        /*
         * If the search failed on the last path component, then
         * PnutDirFindPath() set fr_name. Only in this case we can
         * create a new file.
         */
        if (found.fr_name && (mode & _O_CREAT)) {
            node = PnutNodeAlloc(NODETYPE_REG);

            if (node < 0) {
                errno = ENOSPC;
                return NUTFILE_EOF;
            }

            rc = PnutDirAddEntry(found.fr_pnode, found.fr_name, node);

            if (rc) {
                PnutBlockRelease(node);
            }
        }
    }

    if (rc == 0 && node >= 0) {
        if ((file = malloc(sizeof(PNUTFILE))) != 0) {
            file->f_flag |= mode & (_O_RDONLY | _O_WRONLY | _O_APPEND);
            file->f_pos = (mode & _O_APPEND) ? BankNodePointer(node)->node_size : 0;
            file->f_node = node;

            if ((nfp = malloc(sizeof(NUTFILE))) == 0) {
                free(file);
                nfp = NUTFILE_EOF;
            } else {
                nfp->nf_next = 0;
                nfp->nf_dev = dev;
                nfp->nf_fcb = file;
                /* Keep track of the number of open calls. */
                BankNodePointer(node)->node_refs++;
            }
        }
    }
    return nfp;
}

/*!
 * \brief Close a file.
 */
static int PnutFileClose(NUTFILE * nfp)
{
    if (nfp != NUTFILE_EOF) {
        PNUTFILE *fp = nfp->nf_fcb;

        if (fp) {
            BankNodePointer(fp->f_node)->node_refs--;
            free(fp);
        }
        free(nfp);

        return 0;
    }
    return EINVAL;
}

/*!
 * \brief Delete a file or directory.
 *
 * \param path Full path to the directory.
 *
 * \return 0 on success. Otherwise -1 is returned.
 */
static int PnutDelete(char *path)
{
    int ec;
    PNUT_FINDRESULT found;

    if ((ec = PnutDirFindPath(root, path, &found)) == 0) {
        ec = PnutDirDelEntry(found.fr_pnode, found.fr_name);
    }
    if (ec) {
        errno = ec;
        return -1;
    }
    return 0;
}

/*!
 * \brief Return information about a specified file entry.
 *
 * \param path   Pointer to the full pathname of the file.
 * \param status Pointer to a stat structure for the information returned.
 *
 * \return 0 on success. Otherwise -1 is returned.
 */
static int PnutStatus(CONST char *path, struct stat *status)
{
    int rc;
    PNUT_FINDRESULT found;

    if ((rc = PnutDirFindPath(root, path, &found)) == 0) {
        status->st_mode = BankNodePointer(found.fr_node)->node_type;
        status->st_ino = found.fr_node;
        status->st_nlink = BankNodePointer(found.fr_node)->node_links;
        status->st_size = BankNodePointer(found.fr_node)->node_size;
        status->st_mtime = BankNodePointer(found.fr_node)->node_mtime;
    }
    return rc;
}

/*!
 * \brief Return information about a previously opened file.
 *
 * \param nfp    Pointer to a \ref NUTFILE structure, obtained by a previous 
 *               call to PnutFileOpen().
 * \param status Pointer to a stat structure for the information returned.
 *
 * \return 0 on success. Otherwise -1 is returned.
 */
static int PnutFileStatus(PNUTFILE * fp, struct stat *status)
{
    status->st_mode = BankNodePointer(fp->f_node)->node_type;
    status->st_ino = fp->f_node;
    status->st_nlink = BankNodePointer(fp->f_node)->node_links;
    status->st_size = BankNodePointer(fp->f_node)->node_size;
    status->st_mtime = BankNodePointer(fp->f_node)->node_mtime;

    return 0;
}

/*!
 * \brief Write data to a file.
 *
 * \param nfp    Pointer to a \ref NUTFILE structure, obtained by a previous 
 *               call to PnutFileOpen().
 * \param buffer Pointer to the data to be written. If zero, then the
 *               output buffer will be flushed.
 * \param len    Number of bytes to write.
 *
 * \return The number of bytes written. A return value of -1 indicates an 
 *         error.
 */
static int PnutFileWrite(NUTFILE * nfp, CONST void *buffer, int len)
{
    PNUTFILE *fp = nfp->nf_fcb;
    int ec = 0;
    int rc = 0;
    PNUT_BLKNUM node = fp->f_node;
    uint8_t *blkptr;
    size_t blksiz;
    CONST char *buf = buffer;

    while (len) {
        if ((ec = PnutNodeGetDataPtr(node, fp->f_pos, &blkptr, &blksiz, 1)) != 0) {
            break;
        }
        if (blksiz >= len) {
            blksiz = len;
            len = 0;
        } else {
            len -= blksiz;
        }
        memcpy(blkptr, buf, blksiz);
        rc += blksiz;
        buf += blksiz;
        fp->f_pos += blksiz;
    }
    if (ec == 0 || ec == ENOSPC) {
        if (BankNodePointer(node)->node_size < fp->f_pos) {
            BankNodePointer(node)->node_size = fp->f_pos;
        }
        BankNodePointer(node)->node_mtime = time(0);
    }
    return rc;
}

#ifdef __HARVARD_ARCH__
static int PnutFileWrite_P(NUTFILE * nfp, PGM_P buffer, int len)
{
    return -1;
}
#endif

/*!
 * \brief Read data from a file.
 *
 * \param nfp    Pointer to a ::NUTFILE structure, obtained by a previous 
 *               call to PnutFileOpen().
 * \param buffer Pointer to the data buffer to fill.
 * \param len    Maximum number of bytes to read.
 *
 * \return The number of bytes actually read. A return value of -1 indicates 
 *         an error.
 */
static int PnutFileRead(NUTFILE * nfp, void *buffer, int len)
{
    PNUTFILE *fp = nfp->nf_fcb;
    int ec = 0;
    int rc = 0;
    PNUT_BLKNUM node = fp->f_node;
    uint8_t *blkptr;
    size_t blksiz;
    char *buf = buffer;

    /* Respect end of file. */
    if (len > BankNodePointer(node)->node_size - fp->f_pos) {
        len = (size_t) (BankNodePointer(node)->node_size - fp->f_pos);
    }
    while (len) {
        if ((ec = PnutNodeGetDataPtr(node, fp->f_pos, &blkptr, &blksiz, 0)) != 0) {
            break;
        }
        if (blksiz >= len) {
            blksiz = len;
            len = 0;
        } else {
            len -= blksiz;
        }
        memcpy(buf, blkptr, blksiz);
        rc += blksiz;
        buf += blksiz;
        fp->f_pos += blksiz;
    }
    return rc;
}

static int PnutFileSeek(PNUTFILE * fp, long *pos, int whence)
{
    int rc = 0;
    long npos = *pos;

    switch (whence) {
    case SEEK_CUR:
        npos += fp->f_pos;
        break;
    case SEEK_END:
        npos += BankNodePointer(fp->f_node)->node_size;
        break;
    }

    if (npos < 0 || npos > (long) BankNodePointer(fp->f_node)->node_size) {
        rc = EINVAL;
    } else {
        fp->f_pos = npos;
        *pos = npos;
    }
    return rc;
}

/* ------------------------ Special function interface ------------------------ */

/*!
 * \brief Device specific functions.
 */
int PnutIOCtl(NUTDEVICE * dev, int req, void *conf)
{
    int rc = -1;

    switch (req) {
    case FS_STATUS:
        {
            FSCP_STATUS *par = (FSCP_STATUS *) conf;

            rc = PnutStatus(par->par_path, par->par_stp);
        }
        break;
    case FS_DIR_CREATE:
        rc = PnutDirCreate((char *) conf);
        break;
    case FS_DIR_REMOVE:
        rc = PnutDelete((char *) conf);
        break;
    case FS_DIR_OPEN:
        rc = PnutDirOpen(dev, (DIR *) conf);
        break;
    case FS_DIR_CLOSE:
        rc = PnutDirClose((DIR *) conf);
        break;
    case FS_DIR_READ:
        rc = PnutDirRead((DIR *) conf);
        break;
    case FS_FILE_STATUS:
        rc = PnutFileStatus((PNUTFILE *) ((IOCTL_ARG2 *) conf)->arg1,   /* */
                            (struct stat *) ((IOCTL_ARG2 *) conf)->arg2);
        break;
    case FS_FILE_DELETE:
        rc = PnutDelete((char *) conf);
        break;
    case FS_FILE_SEEK:
        PnutFileSeek((PNUTFILE *) ((IOCTL_ARG3 *) conf)->arg1,  /* */
                     (long *) ((IOCTL_ARG3 *) conf)->arg2,      /* */
                     (int) ((IOCTL_ARG3 *) conf)->arg3);
        break;
    }
    return rc;
}

/*!
 * \brief Retrieve the size of a previously opened file.
 *
 * This function is called by the low level size routine of the C runtime 
 * library, using the _NUTDEVICE::dev_size entry.
 *
 * \param nfp Pointer to a \ref _NUTFILE structure, obtained by a 
 *            previous call to PhatFileOpen().
 *
 * \return Size of the file.
 */
static long PnutFileSize(NUTFILE *nfp)
{
    PNUTFILE *fp = nfp->nf_fcb;

    return BankNodePointer(fp->f_node)->node_size;
}

/* ------------------------ Initialization ------------------------ */

/*!
 * \brief Initialize the Peanut filesystem.
 *
 * Peanut is a volatile RAM filesystem. All contents is lost when
 * power supply is removed. After initialization we start with
 * a clean filesystem.
 *
 * \param Pointer to the device information structure.
 *
 * \return Zero on success. Otherwise an error code is returned.
 */
static int PnutInit(NUTDEVICE * dev)
{
    PNUT_BLKNUM i;
    int rc;

    /* Add all blocks to the free list. */
    for (i = 0; i < PNUT_TOTAL_BLOCKS; i++) {
        PnutBlockRelease(i);
    }

    /* Initialize the root directory. */
    if ((root = PnutNodeAlloc(NODETYPE_DIR)) == -1) {
        rc = ENOSPC;
    } else {
        if ((rc = PnutDirAddEntry(root, ".", root)) == 0) {
            rc = PnutDirAddEntry(root, "..", root);
        }
        if (rc) {
            PnutBlockRelease(root);
        }
    }
    return rc;
}

/*!
 * \brief Peanut device information structure.
 */
NUTDEVICE devPnut = {
    0,                          /*!< Pointer to next device, dev_next. */
    {'P', 'N', 'U', 'T', 0, 0, 0, 0, 0}
    ,                           /*!< Unique device name, dev_name. */
    IFTYP_RAM,                  /*!< Type of device, dev_type. Obsolete. */
    0,                          /*!< Base address, dev_base. Unused. */
    0,                          /*!< First interrupt number, dev_irq. Unused. */
    0,                          /*!< Interface control block, dev_icb. Unused. */
    0,                          /*!< Driver control block, dev_dcb. Unused. */
    PnutInit,                   /*!< Driver initialization routine, dev_init. */
    PnutIOCtl,                  /*!< Driver specific control function, dev_ioctl. */
    PnutFileRead,               /*!< Read data from a file, dev_read. */
    PnutFileWrite,              /*!< Write data to a file, dev_write. */
#ifdef __HARVARD_ARCH__
    PnutFileWrite_P,            /*!< Write data from program space to a file, dev_write_P. */
#endif
    PnutFileOpen,               /*!< Open a file, dev_open. */
    PnutFileClose,              /*!< Close a file, dev_close. */
    PnutFileSize                /*!< Return file size, dev_size. */
};

/*@}*/
