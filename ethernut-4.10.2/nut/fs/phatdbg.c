/*
 * Copyright (C) 2005 by egnite Software GmbH. All rights reserved.
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
 * \file fs/phatdbg.c
 * \brief PHAT debugging support.
 *
 * \verbatim
 *
 * $Log$
 * Revision 1.2  2008/08/11 06:59:42  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.1  2006/01/05 16:31:27  haraldkipp
 * First check-in.
 *
 *
 * \endverbatim
 */

#include <errno.h>

#include <fs/fs.h>

#include <fs/phatfs.h>
#include <fs/phatvol.h>
#include <dev/blockdev.h>
#include <fs/phatio.h>

#include <stdio.h>

#include <fs/phatdbg.h>

/*!
 * \addtogroup xgPhatDbg
 */
/*@{*/

void PhatDbgAttrList(FILE * fp, uint8_t attr)
{
    if (attr & PHAT_FATTR_RDONLY) {
        fputc('R', fp);
    } else if (attr & PHAT_FATTR_HIDDEN) {
        fputc('H', fp);
    } else if (attr & PHAT_FATTR_SYSTEM) {
        fputc('S', fp);
    } else if (attr & PHAT_FATTR_VOLID) {
        fputc('V', fp);
    } else if (attr & PHAT_FATTR_DIR) {
        fputc('D', fp);
    } else if (attr & PHAT_FATTR_ARCHIV) {
        fputc('A', fp);
    }
}

void PhatDbgDirEntry(FILE * fp, CONST char *title, PHATDIRENT * dent)
{
    if (title) {
        fprintf(fp, "--- %s ---\n", title);
    }
    fprintf(fp, "DentFName: '%.11s'\n", dent->dent_name);
    fputs("DentFAttr: ", fp);
    PhatDbgAttrList(fp, dent->dent_attr);
    fputc('\n', fp);
    fprintf(fp, "DentRsrvd: 0x%02X\n", dent->dent_rsvdnt);
    fprintf(fp, "DentCTime: 0x%04X 0x%04X 0x%02X\n", dent->dent_cdate, dent->dent_ctime, dent->dent_ctsecs);
    fprintf(fp, "DentADate: 0x%04X\n", dent->dent_adate);
    fprintf(fp, "DentMTime: 0x%04X 0x%04X\n", dent->dent_mdate, dent->dent_mtime);
    fprintf(fp, "DentClust: %lu\n", (uint32_t) dent->dent_clust + ((uint32_t) dent->dent_clusthi << 16));
    fprintf(fp, "DentFSize: %lu\n", dent->dent_fsize);
}

void PhatDbgFileInfo(FILE * fp, CONST char *title, PHATFILE * nfp)
{
    if (title) {
        fprintf(fp, "--- %s ---\n", title);
    }
    fprintf(fp, "File Pos   : %lu\n", nfp->f_pos);
    fprintf(fp, "Cluster    : %lu  Prev %lu\n", nfp->f_clust, nfp->f_clust_prv);
    fprintf(fp, "Cluster Pos: %lu\n", nfp->f_clust_pos);
    fprintf(fp, "Sector Pos : %lu\n", nfp->f_sect_pos);
    PhatDbgDirEntry(fp, NULL, &nfp->f_dirent);
    fprintf(fp, "Parent Clust: %lu\n", (uint32_t) nfp->f_pde_clust + ((uint32_t) nfp->f_pde_clusthi << 16));
    fprintf(fp, "DirEnt Sect : %lu\n", nfp->f_de_sect);
    fprintf(fp, "DirEnt Offs : %lu\n", nfp->f_de_offs);
    fprintf(fp, "DirEnt Dirty: %u\n", nfp->f_de_dirty);
}

void PhatDbgVbr(FILE * fp, CONST char *title, PHATVBR * vbr)
{
    if (title) {
        fprintf(fp, "--- %s ---\n", title);
    }
    fprintf(fp, "Boot jump      : %02X %02X %02X\n", vbr->boot_jump[0], vbr->boot_jump[1], vbr->boot_jump[2]);
    fprintf(fp, "OEM name       : %.8s\n", vbr->boot_oem);
    fprintf(fp, "Bytes/sector   : %u\n", vbr->bios_sectsz);
    fprintf(fp, "Sectors/cluster: %u\n", vbr->bios_clustsz);
    fprintf(fp, "Rsvd. sectors  : %u\n", vbr->bios_rsvd_sects);
    fprintf(fp, "Alloc. tables  : %u\n", vbr->bios_ntabs);
    fprintf(fp, "Root entries   : %u\n", vbr->bios_rootsz);
    fprintf(fp, "Total sectors  : %lu\n", vbr->bios_volsz ? (uint32_t) vbr->bios_volsz : vbr->bios_volsz_big);
    fprintf(fp, "Media type     : 0x%02X\n", vbr->bios_media);
    fprintf(fp, "Sectors/table  : %u %lu\n", vbr->bios_tabsz, vbr->bios_tabsz_big);
    fprintf(fp, "Sectors/track  : %u\n", vbr->bios_sects_p_trk);
    fprintf(fp, "Heads          : %u\n", vbr->bios_heads);
    fprintf(fp, "Hidden sectors : %lu\n", vbr->bios_sects_hidd);
    fprintf(fp, "Mirror flags   : 0x%04u\n", vbr->bios_xflags);
    fprintf(fp, "File system ver: %u\n", vbr->bios_fsver);
    fprintf(fp, "Root dir clust : %lu\n", vbr->bios_root_clust);
    fprintf(fp, "FSInfo sector  : %u\n", vbr->bios_fsinfo);
    fprintf(fp, "Boot back sect : %u\n", vbr->bios_boot_bak);
    fprintf(fp, "Logical drive  : %u\n", vbr->boot_drive);
    fprintf(fp, "Rsvd NT(head)  : %u\n", vbr->boot_rsvd_nt);
    fprintf(fp, "Ext. signature : 0x%02X\n", vbr->boot_xsig);
    if (vbr->boot_xsig == 0x28 || vbr->boot_xsig == 0x29) {
        fprintf(fp, "Volume ID      : 0x%08lX\n", vbr->boot_vol_id);
        fprintf(fp, "Volume label   : %.11s\n", vbr->boot_vol_lbl);
        fprintf(fp, "Filesystem     : %.8s\n", vbr->boot_vol_fs);
    }
}

/*@}*/
