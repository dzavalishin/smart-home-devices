#ifndef _NETINET_IGMP_H_
#define _NETINET_IGMP_H_

/*
 * Copyright (C) 2007 by egnite Software GmbH. All rights reserved.
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
 *
 * -
 * Copyright (c) 1988 Stephen Deering.
 * Copyright (c) 1992, 1993
 *  The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * $Log$
 * Revision 1.3  2008/08/11 07:00:23  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.2  2007/08/29 07:43:54  haraldkipp
 * Documentation updated and corrected.
 *
 * Revision 1.1  2007/05/02 11:18:32  haraldkipp
 * IGMP support added. Incomplete.
 *
 */

#include <sys/types.h>
#include <dev/netbuf.h>
#include <net/if_var.h>

/*!
 * \file netinet/igmp.h
 * \brief IGMP protocol definitions.
 */

/*!
 * \addtogroup xgIGMP
 */
/*@{*/

/*!
 * \typedef IGMP
 * \brief IGMP packet type.
 */
typedef struct __attribute__ ((packed)) igmp IGMP;
typedef struct __attribute__ ((packed)) igmp_report IGMP_REPORT;

/*!
 * \struct igmp igmp.h netinet/igmp.h
 * \brief IGMP packet structure.
 */
struct __attribute__ ((packed)) igmp
{
    uint8_t igmp_type;           /*!< \brief Version and type of IGMP message. */
    uint8_t igmp_code;           /*!< \brief Subtype for routing messages. */
    uint16_t igmp_cksum;         /*!< \brief IP-style checksum. */
    uint32_t igmp_group;          /*!< \brief Group address being reported. */
};

/*!
 * \brief IGMPv3 query format
 */
struct __attribute__ ((packed)) igmpv3 {
    uint8_t igmp_type;           /*!< \brief Version and type of IGMP message. */
    uint8_t igmp_code;           /*!< \brief Subtype for routing messages. */
    uint16_t igmp_cksum;         /*!< \brief IP-style checksum. */
    uint32_t igmp_group;          /*!< \brief Group address being reported. */
    uint8_t igmp_misc;           /*!< \brief Reserved, supress and robustness var. */
    uint8_t igmp_qqi;            /*!< \brief Querier's query interval. */
    uint16_t igmp_numsrc;        /*!< \brief Number of sources. */
    uint32_t igmp_sources[1];     /*!< \brief Source addresses. */
};

/*!
 * \brief IGMPv3 group record.
 */
struct __attribute__ ((packed)) igmp_grouprec {
    uint8_t ig_type;             /*!< \brief Record type. */
    uint8_t ig_datalen;          /*!< \brief Amount of aux data. */
    uint16_t ig_numsrc;          /*!< \brief Number of sources. */
    uint32_t ig_group;            /*!< \brief The group being reported. */
    uint32_t ig_sources[1];       /*!< \brief Source addresses. */
};

/*!
 * \brief IGMPv3 report.
 */
struct __attribute__ ((packed)) igmp_report {
    uint8_t ir_type;             /*!< \brief Record type. */
    uint8_t ir_rsv1;             /*!< \brief Reserved. */
    uint16_t ir_cksum;           /*!< \brief Checksum. */
    uint16_t ir_rsv2;            /*!< \brief Reserved. */
    uint16_t ir_numgrps;         /*!< \brief Number of group records. */
    struct igmp_grouprec ir_groups[1];  /*!< \brief Group records. */
};

/*!
 * \brief Minimum IGMP packet length.
 */
#define IGMP_MINLEN          8

#define IGMP_MEMBERSHIP_QUERY       0x11        /*!< \brief Membership query. */
#define IGMP_V1_MEMBERSHIP_REPORT   0x12        /*!< \brief Ver. 1 membership report. */
#define IGMP_V2_MEMBERSHIP_REPORT   0x16        /*!< \brief Ver. 2 membership report. */
#define IGMP_V3_MEMBERSHIP_REPORT   0x22        /*!< \brief Ver. 3 membership report. */
#define IGMP_V2_LEAVE_GROUP         0x17        /*!< \brief Leave-group message. */

/*!
 * \brief Maximum delay for response query.
 */
#define IGMP_MAX_HOST_REPORT_DELAY  10

/*!
 * \brief Code field time scale.
 */
#define IGMP_TIMER_SCALE            10

/*!
 * \name IGMPv3 Report Types
 */
/*@{*/
#define	IGMP_REPORT_MODE_IN	1       /*!< \brief Mode-is-include. */
#define	IGMP_REPORT_MODE_EX	2       /*!< \brief Mode-is-exclude. */
#define	IGMP_REPORT_TO_IN	3       /*!< \brief Change-to-include. */
#define	IGMP_REPORT_TO_EX	4       /*!< \brief Change-to-exclude. */
#define	IGMP_REPORT_ALLOW_NEW	5       /*!< \brief Allow-new-sources. */
#define	IGMP_REPORT_BLOCK_OLD	6       /*!< \brief Block-old-sources. */
/*@}*/

/*!
 * \name IGMPv3 Report Type Flags
 */
/*@{*/
#define	IGMP_MASK_CUR_STATE	0x01    /*!< \brief Report current-state. */
#define	IGMP_MASK_ALLOW_NEW	0x02    /*!< \brief Report source as allow-new. */
#define	IGMP_MASK_BLOCK_OLD	0x04    /*!< \brief Report source as block-old. */
#define	IGMP_MASK_TO_IN		0x08    /*!< \brief Report source as to_in. */
#define	IGMP_MASK_TO_EX		0x10    /*!< \brief Report source as to_ex. */
#define	IGMP_MASK_STATE_T1	0x20    /*!< \brief State at T1. */
#define	IGMP_MASK_STATE_T2	0x40    /*!< \brief State at T2. */
#define	IGMP_MASK_IF_STATE	0x80    /*!< \brief Report current-state per interface. */

#define	IGMP_MASK_STATE_TX	(IGMP_MASK_STATE_T1 | IGMP_MASK_STATE_T2)
#define	IGMP_MASK_PENDING	(IGMP_MASK_CUR_STATE | IGMP_MASK_ALLOW_NEW | IGMP_MASK_BLOCK_OLD)
/*@}*/

/*!
 * \brief List identifiers
 */
#define	IGMP_EXCLUDE_LIST	1       /* Exclude list used to tag report. */
#define	IGMP_INCLUDE_LIST	2       /* Include list used to tag report. */
#define	IGMP_RECORDED_LIST	3       /* Recorded list used to tag report. */


__BEGIN_DECLS
/* Function prototypes */
extern void NutIgmpInput(NUTDEVICE * dev, NETBUF * nb);
extern int NutIgmpOutput(uint8_t type, uint32_t dest, NETBUF * nb);

extern void NutIgmpJoinGroup(NUTDEVICE *dev, uint32_t ip_addr);
extern void NutIgmpLeaveGroup(NUTDEVICE *dev, uint32_t ip_addr);

__END_DECLS
/* End of prototypes */
/*@}*/
#endif
