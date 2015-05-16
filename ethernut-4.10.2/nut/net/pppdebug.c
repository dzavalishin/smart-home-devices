/*
 * Copyright (C) 2001-2004 by egnite Software GmbH. All rights reserved.
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
 */

/*
 * $Log$
 * Revision 1.7  2008/08/11 07:00:32  haraldkipp
 * BSD types replaced by stdint types (feature request #1282721).
 *
 * Revision 1.6  2006/10/05 17:25:41  haraldkipp
 * Avoid possible alignment errors. Fixes bug #1567748.
 *
 * Revision 1.5  2006/03/29 01:23:52  olereinhardt
 *  Signednes of strings
 *
 * Revision 1.4  2005/04/08 15:20:51  olereinhardt
 * added <sys/types.h> (__APPLE__) and <netinet/in.h> (__linux__)
 * for htons and simmilar.
 *
 * Revision 1.3  2004/03/08 11:27:24  haraldkipp
 * Accept incoming header compression.
 *
 * Revision 1.2  2003/08/14 15:16:22  haraldkipp
 * Echo, discard and protocol reject added
 *
 * Revision 1.1.1.1  2003/05/09 14:41:36  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.1  2003/05/06 17:30:28  harald
 * Put in seperate module
 *
 */

#include <net/netdebug.h>
#include <sys/types.h>
#include <dev/ahdlc.h>

#include <arpa/inet.h>
#include <netinet/ppp_fsm.h>
#include <netinet/if_ppp.h>
#include <netinet/in.h>
FILE *__ppp_trs;                /*!< \brief PPP trace output stream. */
uint_fast8_t __ppp_trf;               /*!< \brief PPP trace flags. */

static uint8_t ppp_header_sz;    /* Size of the PPP header. */

static prog_char dbg_confreq[] = "[CONFREQ]";
static prog_char dbg_confack[] = "[CONFACK]";
static prog_char dbg_confnak[] = "[CONFNAK]";
static prog_char dbg_confrej[] = "[CONFREJ]";
static prog_char dbg_termreq[] = "[TERMREQ]";
static prog_char dbg_termack[] = "[TERMACK]";
static prog_char dbg_coderej[] = "[CODEREJ]";
static prog_char dbg_protrej[] = "[PROTREJ]";
static prog_char dbg_echoreq[] = "[ECHOREQ]";
static prog_char dbg_echorsp[] = "[ECHORSP]";
static prog_char dbg_discreq[] = "[DISCREQ]";


void NutDumpLcpOption(FILE * stream, NETBUF * nb)
{
    XCPOPT *xcpo;
    uint16_t len;

    if ((len = nb->nb_ap.sz) != 0)
        xcpo = nb->nb_ap.vp;
    else {
        len = nb->nb_dl.sz - ppp_header_sz - sizeof(XCPHDR);
        xcpo = (XCPOPT *) (((char *) nb->nb_dl.vp) + ppp_header_sz + sizeof(XCPHDR));
    }
    fprintf(stream, "[OPT(%u)]", len);
    while (len) {
        switch (xcpo->xcpo_type) {
        case LCP_MRU:
            fprintf(stream, "[MRU=%u]", ntohs(xcpo->xcpo_.us));
            break;
        case LCP_ASYNCMAP:
            fprintf(stream, "[ACCM=0x%08lX]", ntohl(xcpo->xcpo_.ul));
            break;
        case LCP_AUTHTYPE:
            fprintf(stream, "[AUTH=0x%04X]", ntohs(xcpo->xcpo_.us));
            break;
        case LCP_MAGICNUMBER:
            fprintf(stream, "[MAGIC=0x%08lX]", ntohl(xcpo->xcpo_.ul));
            break;
        case LCP_PCOMPRESSION:
            fputs("[PCOMP]", stream);
            break;
        case LCP_ACCOMPRESSION:
            fputs("[ACOMP]", stream);
            break;
        default:
            fprintf(stream, "[OPT%u?]", xcpo->xcpo_type);
            break;
        }
        if (len < xcpo->xcpo_len) {
            fputs("[LEN?]", stream);
            break;
        }
        len -= xcpo->xcpo_len;
        xcpo = (XCPOPT *) ((char *) xcpo + xcpo->xcpo_len);
    }
}

void NutDumpLcp(FILE * stream, NETBUF * nb)
{
    XCPHDR *lcp;
    uint16_t len;

    if ((len = nb->nb_nw.sz) != 0) {
        len = nb->nb_nw.sz + nb->nb_ap.sz;
        lcp = nb->nb_nw.vp;
    } else {
        len = nb->nb_dl.sz - ppp_header_sz;
        lcp = (XCPHDR *) (((char *) nb->nb_dl.vp) + ppp_header_sz);
    }
    fprintf(stream, "[LCP-%03u(%u)]", lcp->xch_id, ntohs(lcp->xch_len));
    if (len < sizeof(XCPHDR)) {
        fputs("[LEN?]", stream);
        return;
    }

    switch (lcp->xch_code) {
    case XCP_CONFREQ:
        fputs_P(dbg_confreq, stream);
        NutDumpLcpOption(stream, nb);
        break;

    case XCP_CONFACK:
        fputs_P(dbg_confack, stream);
        NutDumpLcpOption(stream, nb);
        break;

    case XCP_CONFNAK:
        fputs_P(dbg_confnak, stream);
        NutDumpLcpOption(stream, nb);
        break;

    case XCP_CONFREJ:
        fputs_P(dbg_confrej, stream);
        NutDumpLcpOption(stream, nb);
        break;

    case XCP_TERMREQ:
        fputs_P(dbg_termreq, stream);
        break;

    case XCP_TERMACK:
        fputs_P(dbg_termack, stream);
        break;

    case XCP_CODEREJ:
        fputs_P(dbg_coderej, stream);
        break;

    case LCP_PROTREJ:
        fputs_P(dbg_protrej, stream);
        break;
    case LCP_ERQ:
        fputs_P(dbg_echoreq, stream);
        break;
    case LCP_ERP:
        fputs_P(dbg_echorsp, stream);
        break;
    case LCP_DRQ:
        fputs_P(dbg_discreq, stream);
        break;

    default:
        fprintf(stream, "[CODE%u?]", lcp->xch_code);
        break;
    }
}

void NutDumpPapOption(FILE * stream, NETBUF * nb)
{
    char *xcpo;
    uint16_t len;
    uint8_t i;

    if ((len = nb->nb_ap.sz) != 0)
        xcpo = nb->nb_ap.vp;
    else {
        len = nb->nb_dl.sz - ppp_header_sz - sizeof(XCPHDR);
        xcpo = ((char *) nb->nb_dl.vp) + ppp_header_sz + sizeof(XCPHDR);
    }
    fprintf(stream, "[OPT(%u)]", len);
    while (len) {
        if (*xcpo) {
            fputc('[', stream);
            for (i = 1; i <= *xcpo; i++)
                fputc(*(xcpo + i), stream);
            fputc(']', stream);
        }
        if (len < (uint16_t) (*xcpo + 1)) {
            fputs("[LEN?]", stream);
            break;
        }
        len -= *xcpo + 1;
        xcpo = xcpo + *xcpo + 1;
    }
}

void NutDumpPap(FILE * stream, NETBUF * nb)
{
    XCPHDR *pap;
    uint16_t len;

    if ((len = nb->nb_nw.sz) != 0)
        pap = nb->nb_nw.vp;
    else {
        len = nb->nb_dl.sz - ppp_header_sz;
        pap = (XCPHDR *) (((char *) nb->nb_dl.vp) + ppp_header_sz);
    }
    fprintf(stream, "[PAP-%03u(%u)]", pap->xch_id, ntohs(pap->xch_len));
    if (len < sizeof(XCPHDR)) {
        fputs("[LEN?]", stream);
        return;
    }

    switch (pap->xch_code) {
    case XCP_CONFREQ:
        fputs_P(dbg_confreq, stream);
        NutDumpPapOption(stream, nb);
        break;

    case XCP_CONFACK:
        fputs_P(dbg_confack, stream);
        break;

    case XCP_CONFNAK:
        fputs_P(dbg_confnak, stream);
        break;

    default:
        fprintf(stream, "[CODE%u?]", pap->xch_code);
        break;
    }
}

void NutDumpIpcpOption(FILE * stream, NETBUF * nb)
{
    XCPOPT *xcpo;
    uint16_t len;

    if ((len = nb->nb_ap.sz) != 0)
        xcpo = nb->nb_ap.vp;
    else {
        len = nb->nb_dl.sz - ppp_header_sz - sizeof(XCPHDR);
        xcpo = (XCPOPT *) (((char *) nb->nb_dl.vp) + ppp_header_sz + sizeof(XCPHDR));
    }
    fprintf(stream, "[OPT(%u)]", len);
    while (len) {
        switch (xcpo->xcpo_type) {
        case IPCP_ADDR:
            fprintf(stream, "[ADDR=%s]", inet_ntoa(xcpo->xcpo_.ul));
            break;
        case IPCP_COMPRESSTYPE:
            fputs("[COMP]", stream);
            break;
        case IPCP_MS_DNS1:
            fprintf(stream, "[DNS1=%s]", inet_ntoa(xcpo->xcpo_.ul));
            break;
        case IPCP_MS_DNS2:
            fprintf(stream, "[DNS2=%s]", inet_ntoa(xcpo->xcpo_.ul));
            break;
        default:
            fprintf(stream, "[OPT%u?]", xcpo->xcpo_type);
            break;
        }
        if (len < xcpo->xcpo_len) {
            fputs("[LEN?]", stream);
            break;
        }
        len -= xcpo->xcpo_len;
        xcpo = (XCPOPT *) ((char *) xcpo + xcpo->xcpo_len);
    }
}

void NutDumpIpcp(FILE * stream, NETBUF * nb)
{
    XCPHDR *ipcp;
    uint16_t len;

    if ((len = nb->nb_nw.sz) != 0)
        ipcp = nb->nb_nw.vp;
    else {
        len = nb->nb_dl.sz - ppp_header_sz;
        ipcp = (XCPHDR *) (((char *) nb->nb_dl.vp) + ppp_header_sz);
    }
    fprintf(stream, "[IPCP-%03u(%u)]", ipcp->xch_id, ntohs(ipcp->xch_len));
    if (len < sizeof(XCPHDR)) {
        fputs("[LEN?]", stream);
        return;
    }

    switch (ipcp->xch_code) {
    case XCP_CONFREQ:
        fputs_P(dbg_confreq, stream);
        NutDumpIpcpOption(stream, nb);
        break;

    case XCP_CONFACK:
        fputs_P(dbg_confack, stream);
        NutDumpIpcpOption(stream, nb);
        break;

    case XCP_CONFNAK:
        fputs_P(dbg_confnak, stream);
        NutDumpIpcpOption(stream, nb);
        break;

    case XCP_CONFREJ:
        fputs_P(dbg_confrej, stream);
        NutDumpIpcpOption(stream, nb);
        break;

    case XCP_TERMREQ:
        fputs_P(dbg_termreq, stream);
        break;

    case XCP_TERMACK:
        fputs_P(dbg_termack, stream);
        break;

    case XCP_CODEREJ:
        fputs_P(dbg_coderej, stream);
        break;

    default:
        fprintf(stream, "[CODE%u?]", ipcp->xch_code);
        break;
    }
}

void NutDumpPpp(FILE * stream, NETBUF * nb)
{
    PPPHDR *ph = (PPPHDR *) nb->nb_dl.vp;
    uint16_t protocol;

    fprintf(stream, "(%u)", nb->nb_dl.sz + nb->nb_nw.sz + nb->nb_tp.sz + nb->nb_ap.sz);

    /*
     * Check if the address and control field is compressed.
     * Thanks to Francois Rademeyer.
     */
    if (ph->address != AHDLC_ALLSTATIONS) {

        /*
         * Check for protocol compression.
         * LSB of 2nd octet for protocol is always 1.
         */
        if (((uint8_t *) nb->nb_dl.vp)[0] & 0x01) {
            ppp_header_sz = 1;
            protocol = *(uint8_t *) nb->nb_dl.vp;
        } else {
            char *cp = (char *)nb->nb_dl.vp;
            ppp_header_sz = 2;
            protocol = ntohs(((uint16_t)cp[0] << 8) | cp[1]);
        }
    } else {
        ppp_header_sz = sizeof(PPPHDR);
        protocol = ntohs(ph->prot_type);
    }

    if (nb->nb_dl.sz < ppp_header_sz) {
        fputs("[LEN?]", stream);
        return;
    }

    switch (protocol) {
    case PPP_IP:
        break;

    case PPP_LCP:
        NutDumpLcp(stream, nb);
        break;

    case PPP_IPCP:
        NutDumpIpcp(stream, nb);
        break;

    case PPP_PAP:
        NutDumpPap(stream, nb);
        break;

    default:
        fprintf(stream, "[TYPE 0x%04X?]", ntohs(ph->prot_type));
        break;
    }
}

/*!
 * \brief Control PPP tracing.
 *
 * \param stream Pointer to a previously opened stream or null to 
 *               disable trace output.
 * \param flags  Flags to enable specific traces.
 */
void NutTracePPP(FILE * stream, uint8_t flags)
{
    if (stream)
        __ppp_trs = stream;
    if (__ppp_trs)
        __ppp_trf = flags;
    else
        __ppp_trf = 0;
}
