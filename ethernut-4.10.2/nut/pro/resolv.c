/*
 * Copyright (C) 2009 by egnite GmbH
 * Copyright (C) 2001-2003 by egnite Software GmbH
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
 * $Id: resolv.c 2786 2009-11-06 18:14:36Z haraldkipp $
 */

#include <cfg/os.h>

#include <sys/device.h>
#include <sys/timer.h>
#include <sys/heap.h>

#include <arpa/inet.h>
#include <net/if_var.h>
#include <sys/socket.h>

#include <stdlib.h>
#include <string.h>
#include <memdebug.h>

#include <netdb.h>

#ifdef NUTDEBUG
#include <stdio.h>
#endif

/*!
 * \addtogroup xgDNS
 */
/*@{*/

extern DNSCONFIG confdns;

typedef struct {
    uint16_t doh_id;
    uint16_t doh_flags;
    uint16_t doh_quests;
    uint16_t doh_answers;
    uint16_t doh_authrr;
    uint16_t doh_addrr;
} DNSHEADER;

typedef struct {
    uint8_t *doq_name;
    uint16_t doq_type;
    uint16_t doq_class;
} DNSQUESTION;

typedef struct {
    uint8_t *dor_name;
    uint16_t dor_type;
    uint16_t dor_class;
    uint32_t dor_ttl;
    uint16_t dor_len;
    uint8_t *dor_data;
} DNSRESOURCE;

#ifdef NUTDEBUG
void DumpDnsHeader(FILE * stream, DNSHEADER * doh)
{
    fprintf(stream, "HEADER: id=%u flg=%04X #q=%u #an=%u #au=%u #ad=%u\r\n",
            doh->doh_id, doh->doh_flags, doh->doh_quests, doh->doh_answers, 
            doh->doh_authrr, doh->doh_addrr);
}

void DumpDnsQuestion(FILE * stream, DNSQUESTION * doq)
{
    fprintf(stream, "QUESTION: name='%s' type=%u class=%u\r\n", 
        doq->doq_name, doq->doq_type, doq->doq_class);
}

void DumpDnsResource(FILE * stream, DNSRESOURCE * dor)
{
    uint16_t i;

    fprintf(stream, "RESOURCE: name='%s' type=%u class=%u ttl=%lu len=%u ",
            dor->dor_name, dor->dor_type, dor->dor_class, dor->dor_ttl, dor->dor_len);
    for (i = 0; i < dor->dor_len; i++)
        fprintf(stream, "%02X ", dor->dor_data[i]);
    fputc('\n', stream);
}
#endif

static uint16_t AddShort(uint8_t * cp, uint16_t val)
{
    *cp++ = (uint8_t) (val >> 8);
    *cp++ = (uint8_t) val;

    return 2;
}

static uint16_t AddName(uint8_t * cp, CONST uint8_t * name)
{
    uint8_t *lcp;
    uint16_t rc = strlen((char *) name) + 2;

    lcp = cp++;
    *lcp = 0;
    while (*name) {
        if (*name == '.') {
            lcp = cp++;
            *lcp = 0;
            name++;
        } else {
            *cp++ = *name++;
            (*lcp)++;
        }
    }
    *cp = 0;

    return rc;
}

static uint16_t ScanShort(uint8_t * cp, uint16_t * val)
{
    *val = (uint16_t) (*cp++) << 8;
    *val |= *cp;

    return 2;
}

static uint16_t ScanLong(uint8_t * cp, uint32_t * val)
{
    *val = *cp++;
    *val <<= 8;
    *val |= *cp++;
    *val <<= 8;
    *val |= *cp++;
    *val <<= 8;
    *val |= *cp;

    return 4;
}

static uint16_t ScanName(uint8_t * cp, uint8_t ** npp)
{
    uint8_t len;
    uint16_t rc;
    uint8_t *np;

    if (*npp) {
        free(*npp);
        *npp = 0;
    }

    if ((*cp & 0xC0) == 0xC0)
        return 2;

    rc = strlen((char *) cp) + 1;
    np = *npp = malloc(rc);
    len = *cp++;
    while (len) {
        while (len--)
            *np++ = *cp++;
        if ((len = *cp++) != 0)
            *np++ = '.';
    }
    *np = 0;

    return rc;
}

static uint16_t ScanBinary(uint8_t * cp, uint8_t ** npp, uint16_t len)
{
    if (*npp)
        free(*npp);
    *npp = malloc(len);
    memcpy(*npp, cp, len);

    return len;
}

static DNSHEADER *CreateDnsHeader(DNSHEADER * doh, uint16_t id)
{
    if (doh == NULL)
        doh = calloc(1, sizeof(DNSHEADER));
    if (doh) {
        doh->doh_id = id;
        doh->doh_flags = 0x0100;
        doh->doh_quests = 1;
    }
    return doh;
}

static void ReleaseDnsHeader(DNSHEADER * doh)
{
    if (doh)
        free(doh);
}

static uint16_t EncodeDnsHeader(uint8_t * buf, DNSHEADER * doh)
{
    uint16_t rc;

    rc = AddShort(buf, doh->doh_id);
    rc += AddShort(buf + rc, doh->doh_flags);
    rc += AddShort(buf + rc, doh->doh_quests);
    rc += AddShort(buf + rc, doh->doh_answers);
    rc += AddShort(buf + rc, doh->doh_authrr);
    rc += AddShort(buf + rc, doh->doh_addrr);

    return rc;
}

static uint16_t DecodeDnsHeader(DNSHEADER * doh, uint8_t * buf)
{
    uint16_t rc;

    rc = ScanShort(buf, &doh->doh_id);
    rc += ScanShort(buf + rc, &doh->doh_flags);
    rc += ScanShort(buf + rc, &doh->doh_quests);
    rc += ScanShort(buf + rc, &doh->doh_answers);
    rc += ScanShort(buf + rc, &doh->doh_authrr);
    rc += ScanShort(buf + rc, &doh->doh_addrr);

    return rc;
}

static DNSQUESTION *CreateDnsQuestion(DNSQUESTION * doq, CONST uint8_t * name, uint16_t type)
{
    if (doq == NULL)
        doq = calloc(1, sizeof(DNSQUESTION));
    if (doq) {
        if (doq->doq_name)
            free(doq->doq_name);
        doq->doq_name = (uint8_t *) strdup((char *) name);
        doq->doq_type = type;
        doq->doq_class = 1;
    }
    return doq;
}

static void ReleaseDnsQuestion(DNSQUESTION * doq)
{
    if (doq) {
        if (doq->doq_name)
            free(doq->doq_name);
        free(doq);
    }
}

static uint16_t EncodeDnsQuestion(uint8_t * buf, DNSQUESTION * doq)
{
    uint16_t rc;

    rc = AddName(buf, doq->doq_name);
    rc += AddShort(buf + rc, doq->doq_type);
    rc += AddShort(buf + rc, doq->doq_class);

    return rc;
}

static uint16_t DecodeDnsQuestion(DNSQUESTION * doq, uint8_t * buf)
{
    uint16_t rc;

    rc = ScanName(buf, &doq->doq_name);
    rc += ScanShort(buf + rc, &doq->doq_type);
    rc += ScanShort(buf + rc, &doq->doq_class);

    return rc;
}

static DNSRESOURCE *CreateDnsResource(DNSRESOURCE * dor)
{
    if (dor == NULL)
        dor = calloc(1, sizeof(DNSRESOURCE));
    return dor;
}

static void ReleaseDnsResource(DNSRESOURCE * dor)
{
    if (dor) {
        if (dor->dor_name)
            free(dor->dor_name);
        if (dor->dor_data)
            free(dor->dor_data);
        free(dor);
    }
}

static uint16_t DecodeDnsResource(DNSRESOURCE * dor, uint8_t * buf)
{
    uint16_t rc;

    rc = ScanName(buf, &dor->dor_name);
    rc += ScanShort(buf + rc, &dor->dor_type);
    rc += ScanShort(buf + rc, &dor->dor_class);
    rc += ScanLong(buf + rc, &dor->dor_ttl);
    rc += ScanShort(buf + rc, &dor->dor_len);
    rc += ScanBinary(buf + rc, &dor->dor_data, dor->dor_len);

    return rc;
}

/*!
 * \brief Sets DNS configuration.
 *
 * \deprecated New applications should use NutDnsConfig2().
 *
 * \param hostname DNS name of the local host.
 * \param domain Name of the domain of the local host.
 * \param dnsip IP address of the DNS server.
 */
void NutDnsConfig(CONST uint8_t * hostname, CONST uint8_t * domain, uint32_t dnsip)
{
    NutDnsConfig2(hostname, domain, dnsip, 0);
}

void NutDnsGetConfig2(char **hostname, char **domain, uint32_t * pdnsip, uint32_t * sdnsip)
{
    if (hostname) {
        *hostname = (char *) confdns.doc_hostname;
    }
    if (domain) {
        *domain = (char *) confdns.doc_domain;
    }
    if (pdnsip) {
        *pdnsip = confdns.doc_ip1;
    }
    if (sdnsip) {
        *sdnsip = confdns.doc_ip2;
    }
}

/*!
 * \brief Retrieves IP-address corresponding to a host name.
 *
 * This is a very simple implementation, which will not return
 * any other resource information than the IP address.
 *
 * \param hostname Fully qualified domain name of the host.
 * \param type     Request type.
 *
 * \return IP address, which is zero, if the name could not
 *         be resolved.
 */
uint32_t NutDnsGetResource(CONST uint8_t * hostname, CONST uint16_t type);

uint32_t NutDnsGetHostByName(CONST uint8_t * hostname)
{
    return NutDnsGetResource(hostname, 1);
}

/*!
 * \brief Retrieves all IP-address corresponding to a host name.
 *
 * This is a very simple implementation, which will not return
 * any other resource information than the IP address.
 *
 * \param hostname Fully qualified domain name of the host.
 * \param type     Request type.
 * \param ip_all   Array of IP Addresses.
 *
 * \return Number of IP address, which is zero, if the name could not
 *         be resolved.
 */
uint8_t NutDnsGetResourceAll(CONST uint8_t * hostname, CONST uint16_t type, uint32_t * ip_all);

uint8_t NutDnsGetHostsByName(CONST uint8_t * hostname, uint32_t * ip_all)
{
    return NutDnsGetResourceAll(hostname, 1, ip_all);
}

uint32_t NutDnsGetMxByDomain(CONST uint8_t * hostname)
{
    return NutDnsGetResource(hostname, 0x0F);
}

uint32_t NutDnsGetResource(CONST uint8_t * hostname, CONST uint16_t type)
{
    uint32_t ip = 0;
    uint8_t *pkt;
    uint16_t len;
    uint16_t id = 0;
    UDPSOCKET *sock;
    DNSHEADER *doh = 0;
    DNSQUESTION *doq = 0;
    DNSRESOURCE *dor = 0;
    int n;
    int retries;
    uint32_t raddr;
    uint16_t rport;

    /*
     * We need a configured DNS address.
     */
    if (confdns.doc_ip1 == 0 && confdns.doc_ip2 == 0)
        return 0;

    /*
     * Create client socket and allocate
     * a buffer for the UDP packet.
     */
    if ((sock = NutUdpCreateSocket(0)) == 0)
        return 0;
    pkt = malloc(512);

    for (retries = 0; retries < 6; retries++) {

        /*
         * Create standard header info structures.
         */
        doh = CreateDnsHeader(doh, ++id);
        doq = CreateDnsQuestion(doq, hostname, type);

#ifdef NUTDEBUG
        //DumpDnsHeader(doh);
        //DumpDnsQuestion(doq);
#endif

        /*
         * Encode the header info into the packet buffer
         * and send it to the DNS server.
         */
        len = EncodeDnsHeader(pkt, doh);
        len += EncodeDnsQuestion(pkt + len, doq);

        if ((retries & 1) == 0 || confdns.doc_ip2 == 0) {
            if (NutUdpSendTo(sock, confdns.doc_ip1, 53, pkt, len) < 0)
                break;
        } else {
            if (NutUdpSendTo(sock, confdns.doc_ip2, 53, pkt, len) < 0)
                break;
        }

        /*
         * Loop until we receive a response with the
         * expected id or until timeout.
         */
        for (;;) {
            len = 0;
            n = NutUdpReceiveFrom(sock, &raddr, &rport, pkt, 512, 1000);
            if (n <= 0)
                break;
            if (n > 12) {
                len = DecodeDnsHeader(doh, pkt);
#ifdef NUTDEBUG
                //DumpDnsHeader(doh);
#endif
                if (doh->doh_id == id)
                    break;
            }
        }

        /*
         * Decode the answer.
         */
        if (len && doh->doh_quests == 1) {
            len += DecodeDnsQuestion(doq, pkt + len);
#ifdef NUTDEBUG
            //DumpDnsQuestion(doq);
#endif
            if (doh->doh_answers < 1)
                break;
            else {
                for (n = 1; n <= (int) doh->doh_answers; n++) {
                    dor = CreateDnsResource(dor);
                    len += DecodeDnsResource(dor, pkt + len);
#ifdef NUTDEBUG
                    //DumpDnsResource(dor);
#endif
                    if (dor->dor_type == 1)
                        break;
                }
                if (dor->dor_len == 4) {
                    ip = *dor->dor_data;
                    ip += (uint32_t) (*(dor->dor_data + 1)) << 8;
                    ip += (uint32_t) (*(dor->dor_data + 2)) << 16;
                    ip += (uint32_t) (*(dor->dor_data + 3)) << 24;
                    break;
                }
                /* TBD: 18.3.2004 - for MX requests authoritative rrs should be skipped + additional rrs should be searched for IP address */
            }
        }
    }

    /*
     * Clean up.
     */
    ReleaseDnsHeader(doh);
    ReleaseDnsQuestion(doq);
    ReleaseDnsResource(dor);

    free(pkt);
    NutUdpDestroySocket(sock);

    return ip;
}

uint8_t NutDnsGetResourceAll(CONST uint8_t * hostname, CONST uint16_t type, uint32_t * ip_all)
{
    uint8_t n_ip;
    uint8_t *pkt;
    uint16_t len;
    uint16_t id = 0;
    UDPSOCKET *sock;
    DNSHEADER *doh = 0;
    DNSQUESTION *doq = 0;
    DNSRESOURCE *dor = 0;
    int n;
    int retries;
    uint32_t raddr;
    uint16_t rport;


    for (n_ip = 0; n_ip < 8; n_ip++)
        ip_all[n_ip] = 0;

    /*
     * We need a configured DNS address.
     */
    if (confdns.doc_ip1 == 0 && confdns.doc_ip2 == 0)
        return 0;

    /*
     * Create client socket and allocate
     * a buffer for the UDP packet.
     */
    if ((sock = NutUdpCreateSocket(0)) == 0)
        return 0;
    pkt = NutHeapAlloc(512);

    for (retries = 0; retries < 6; retries++) {

        /*
         * Create standard header info structures.
         */
        doh = CreateDnsHeader(doh, ++id);
        doq = CreateDnsQuestion(doq, hostname, type);

#ifdef NUTDEBUG
        //DumpDnsHeader(doh);
        //DumpDnsQuestion(doq);
#endif

        /*
         * Encode the header info into the packet buffer
         * and send it to the DNS server.
         */
        len = EncodeDnsHeader(pkt, doh);
        len += EncodeDnsQuestion(pkt + len, doq);

        if ((retries & 1) == 0 || confdns.doc_ip2 == 0) {
            if (NutUdpSendTo(sock, confdns.doc_ip1, 53, pkt, len) < 0)
                break;
        } else {
            if (NutUdpSendTo(sock, confdns.doc_ip2, 53, pkt, len) < 0)
                break;
        }

        /*
         * Loop until we receive a response with the
         * expected id or until timeout.
         */
        for (;;) {
            len = 0;
            n = NutUdpReceiveFrom(sock, &raddr, &rport, pkt, 512, 1000);
            if (n <= 0)
                break;
            if (n > 12) {
                len = DecodeDnsHeader(doh, pkt);
#ifdef NUTDEBUG
                //DumpDnsHeader(doh);
#endif
                if (doh->doh_id == id)
                    break;
            }
        }

        /*
         * Decode the answer.
         */
        if (len && doh->doh_quests == 1) {
            len += DecodeDnsQuestion(doq, pkt + len);
#ifdef NUTDEBUG
            //DumpDnsQuestion(doq);
#endif
            if (doh->doh_answers < 1)
                break;
            else {
                n_ip = 0;
                for (n = 1; n <= (int) doh->doh_answers; n++) {
                    dor = CreateDnsResource(dor);
                    len += DecodeDnsResource(dor, pkt + len);
#ifdef NUTDEBUG
                    //DumpDnsResource(dor);
#endif
                    if (dor->dor_type == 1) {
                        if (dor->dor_len == 4) {
                            ip_all[n_ip] = *dor->dor_data;
                            ip_all[n_ip] += (uint32_t) (*(dor->dor_data + 1)) << 8;
                            ip_all[n_ip] += (uint32_t) (*(dor->dor_data + 2)) << 16;
                            ip_all[n_ip] += (uint32_t) (*(dor->dor_data + 3)) << 24;
                            n_ip++;
                        }
                    }
                }
            }
        }
    }

    /*
     * Clean up.
     */
    ReleaseDnsHeader(doh);
    ReleaseDnsQuestion(doq);
    ReleaseDnsResource(dor);

    NutHeapFree(pkt);
    NutUdpDestroySocket(sock);

    return n_ip;
}

/*@}*/
