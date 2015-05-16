/*
 * Copyright 2007 by egnite Software GmbH
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

#include <sys/confnet.h>

#include <pro/snmp.h>
#include <pro/snmp_api.h>

#include <stdlib.h>
#include <string.h>

#include "mib2if.h"

#define MIB_IF_ENTRIES   4

#if defined(ETHERNUT1) || defined(XNUT_100) || defined(XNUT_105) || defined(CHARON2)
#define PHY_NAME    "RTL8019AS 10 Mbit Ethernet"
#elif defined(ETHERNUT2) || defined(INTECH21)
#define PHY_NAME    "LAN91C111 100Mbit Ethernet"
#elif defined(ETHERNUT3)
#define PHY_NAME    "DM9000E 100Mbit Ethernet"
#elif defined(AT91SAM7X_EK) || defined(AT91SAM9260_EK)
#define PHY_NAME    "DM9161A 100Mbit Ethernet"
#elif defined(ETHERNUT5)
#define PHY_NAME    "LAN8700I 100Mbit Ethernet"
#else
#define PHY_NAME    "Ethernet"
#endif

static uint8_t *MibVarsIfGet(CONST SNMPVAR *, OID *, size_t *, int, size_t *, WMETHOD **);

#define MAG_IF_NUMBER           0

#define MAG_IF_INDEX            1
#define MAG_IF_DESCR            2
#define MAG_IF_TYPE             3
#define MAG_IF_MTU              4
#define MAG_IF_SPEED            5
#define MAG_IF_PHYSADDRESS      6
#define MAG_IF_ADMINSTATUS      7
#define MAG_IF_OPERSTATUS       8
#define MAG_IF_LASTCHANGE       9
#define MAG_IF_INOCTETS         10
#define MAG_IF_INUCASTPKTS      11
#define MAG_IF_INNUCASTPKTS     12
#define MAG_IF_INDISCARDS       13
#define MAG_IF_INERRORS         14
#define MAG_IF_INUNKNOWNPROTOS  15
#define MAG_IF_OUTOCTETS        16
#define MAG_IF_OUTUCASTPKTS     17
#define MAG_IF_OUTNUCASTPKTS    18
#define MAG_IF_OUTDISCARDS      19
#define MAG_IF_OUTERRORS        20
#define MAG_IF_OUTQLEN          21
#define MAG_IF_SPECIFIC         22

static OID base_oid[] = { SNMP_OID_MIB2, 2 };
static size_t base_oidlen = sizeof(base_oid) / sizeof(OID);

static SNMPVAR mib_variables[] = {
    {MAG_IF_NUMBER, ASN_INTEGER, ACL_RONLY, MibVarsIfGet, 1, {1}},
    {MAG_IF_INDEX, ASN_INTEGER, ACL_RONLY, MibVarsIfGet, 3, {2, 1, 1}},
    {MAG_IF_DESCR, ASN_OCTET_STR, ACL_RONLY, MibVarsIfGet, 3, {2, 1, 2}},
    {MAG_IF_TYPE, ASN_INTEGER, ACL_RONLY, MibVarsIfGet, 3, {2, 1, 3}},
    {MAG_IF_MTU, ASN_INTEGER, ACL_RONLY, MibVarsIfGet, 3, {2, 1, 4}},
    {MAG_IF_SPEED, ASN_GAUGE, ACL_RONLY, MibVarsIfGet, 3, {2, 1, 5}},
    {MAG_IF_PHYSADDRESS, ASN_OCTET_STR, ACL_RONLY, MibVarsIfGet, 3, {2, 1, 6}},
    {MAG_IF_ADMINSTATUS, ASN_INTEGER, ACL_RWRITE, MibVarsIfGet, 3, {2, 1, 7}},
    {MAG_IF_OPERSTATUS, ASN_INTEGER, ACL_RONLY, MibVarsIfGet, 3, {2, 1, 8}},
    {MAG_IF_LASTCHANGE, ASN_TIMETICKS, ACL_RONLY, MibVarsIfGet, 3, {2, 1, 9}},
    {MAG_IF_INOCTETS, ASN_COUNTER, ACL_RONLY, MibVarsIfGet, 3, {2, 1, 10}},
    {MAG_IF_INUCASTPKTS, ASN_COUNTER, ACL_RONLY, MibVarsIfGet, 3, {2, 1, 11}},
    {MAG_IF_INNUCASTPKTS, ASN_COUNTER, ACL_RONLY, MibVarsIfGet, 3, {2, 1, 12}},
    {MAG_IF_INDISCARDS, ASN_COUNTER, ACL_RONLY, MibVarsIfGet, 3, {2, 1, 13}},
    {MAG_IF_INERRORS, ASN_COUNTER, ACL_RONLY, MibVarsIfGet, 3, {2, 1, 14}},
    {MAG_IF_INUNKNOWNPROTOS, ASN_COUNTER, ACL_RONLY, MibVarsIfGet, 3, {2, 1, 15}},
    {MAG_IF_OUTOCTETS, ASN_COUNTER, ACL_RONLY, MibVarsIfGet, 3, {2, 1, 16}},
    {MAG_IF_OUTUCASTPKTS, ASN_COUNTER, ACL_RONLY, MibVarsIfGet, 3, {2, 1, 17}},
    {MAG_IF_OUTNUCASTPKTS, ASN_COUNTER, ACL_RONLY, MibVarsIfGet, 3, {2, 1, 18}},
    {MAG_IF_OUTDISCARDS, ASN_COUNTER, ACL_RONLY, MibVarsIfGet, 3, {2, 1, 19}},
    {MAG_IF_OUTERRORS, ASN_COUNTER, ACL_RONLY, MibVarsIfGet, 3, {2, 1, 20}},
    {MAG_IF_OUTQLEN, ASN_GAUGE, ACL_RONLY, MibVarsIfGet, 3, {2, 1, 21}},
    {MAG_IF_SPECIFIC, ASN_OBJECT_ID, ACL_RONLY, MibVarsIfGet, 3, {2, 1, 22}}
};

static long if_number;
static long if_type;
static long if_mtu;
static uint32_t if_speed;
static long if_index;
static char *if_descr;
static OID if_specific[] = { 0, 0 };

static long if_admin_status[MIB_IF_ENTRIES];

/*!
 * \brief Register MIB II variables of the interfaces.
 *
 * \note Preliminary code with hard coded values.
 */
int MibRegisterIfVars(void)
{
    int i;

    if_number = 1;
    for (i = 0; i < if_number; i++) {
        if_admin_status[i] = 1;
    }
    if_type = 6;
    if_mtu = 1500;
    if_speed = 100000000UL;
    if_index = 1;
    if ((if_descr = malloc(strlen(PHY_NAME) + 1)) != NULL) {
        strcpy(if_descr, PHY_NAME);
    }
    return SnmpMibRegister(base_oid, base_oidlen, mib_variables, sizeof(mib_variables) / sizeof(SNMPVAR));
}

static int MibVarsIfSet(int action, uint8_t * var_val, uint8_t var_val_type, size_t var_val_len, OID * name, size_t name_len)
{
    size_t bigsize = 1000;
    uint32_t value = 0;

    if (action != SNMP_ACT_COMMIT) {
        return 0;
    }
    switch (name[9]) {
    case 7:
        if (var_val_type != ASN_INTEGER)
            return SNMP_ERR_WRONGTYPE;
        if (var_val_len > sizeof(uint32_t))
            return SNMP_ERR_WRONGLENGTH;
        AsnUnsignedParse(var_val, &bigsize, &var_val_type, &value);
        if (name[10] > 0 && name[10] < MIB_IF_ENTRIES) {
            if_admin_status[name[10] - 1] = value;
        }
        break;
    }
    return 0;
}

static uint8_t *MibVarsIfGet(CONST SNMPVAR * vp, OID * name, size_t * namelen, int exact, size_t * varlen, WMETHOD ** wmethod)
{
    int rc;
    int ifc = 0;
    OID index = 0;
    static long zero = 0;
    OID *fullname;
    size_t fullnamelen = base_oidlen + vp->var_namelen + 1;

    fullname = malloc(fullnamelen * sizeof(OID));
    memcpy(fullname, base_oid, base_oidlen * sizeof(OID));
    memcpy(fullname + base_oidlen, vp->var_name, vp->var_namelen * sizeof(OID));

    if (vp->var_magic == MAG_IF_NUMBER) {
        /* Determine the number of interfaces. */
        *(fullname + fullnamelen - 1) = index;
        rc = SnmpOidCmp(name, *namelen, fullname, fullnamelen);
        if ((exact && rc) || (!exact && rc >= 0)) {
            free(fullname);
            return NULL;
        }
    } else {
        /* Determine the interface number. */
        for (; ifc < if_number; ifc++) {
            *(fullname + fullnamelen - 1) = ifc + 1;
            rc = SnmpOidCmp(name, *namelen, fullname, fullnamelen);
            if ((exact && rc == 0) || (!exact && rc < 0)) {
                index = ifc + 1;
                break;
            }
        }
        if (index == 0) {
            free(fullname);
            return NULL;
        }
    }

    memcpy(name, fullname, fullnamelen * sizeof(OID));
    free(fullname);
    *namelen = fullnamelen;
    *wmethod = NULL;
    *varlen = sizeof(long);

    switch (vp->var_magic) {
    case MAG_IF_NUMBER:
        return (uint8_t *) & if_number;
    case MAG_IF_INDEX:
        return (uint8_t *) & if_index;
    case MAG_IF_DESCR:
        *varlen = strlen(if_descr);
        return (uint8_t *) if_descr;
    case MAG_IF_TYPE:
        return (uint8_t *) & if_type;
    case MAG_IF_MTU:
        return (uint8_t *) & if_mtu;
    case MAG_IF_SPEED:
        return (uint8_t *) & if_speed;
    case MAG_IF_PHYSADDRESS:
        *varlen = 6;
        return confnet.cdn_mac;
    case MAG_IF_ADMINSTATUS:
    case MAG_IF_OPERSTATUS:
        *wmethod = MibVarsIfSet;
        return (uint8_t *) & if_admin_status[ifc];
    case MAG_IF_LASTCHANGE:
    case MAG_IF_INOCTETS:
    case MAG_IF_INUCASTPKTS:
    case MAG_IF_INNUCASTPKTS:
    case MAG_IF_INDISCARDS:
    case MAG_IF_INERRORS:
    case MAG_IF_INUNKNOWNPROTOS:
    case MAG_IF_OUTOCTETS:
    case MAG_IF_OUTUCASTPKTS:
    case MAG_IF_OUTNUCASTPKTS:
    case MAG_IF_OUTDISCARDS:
    case MAG_IF_OUTERRORS:
    case MAG_IF_OUTQLEN:
        return (uint8_t *) & zero;
    case MAG_IF_SPECIFIC:
        *varlen = sizeof(if_specific);
        return (uint8_t *) if_specific;
    }
    return NULL;
}
