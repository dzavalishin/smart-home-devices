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

#include <sys/version.h>
#include <sys/confos.h>
#include <sys/confnet.h>
#include <sys/environ.h>

#include <pro/snmp.h>
#include <pro/snmp_api.h>

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#include "mib2sys.h"

#if defined(STK501)
#define BOARD_NAME  "STK 501"
#elif defined(INTECH21)
#define BOARD_NAME  "Intech 21"
#elif defined(XNUT_100)
#define BOARD_NAME  "XNUT 100"
#elif defined(XNUT_105)
#define BOARD_NAME  "XNUT 105"
#elif defined(AT91SAM7X_EK)
#define BOARD_NAME  "AT91SAM7X-EK"
#elif defined(AT91SAM9260_EK)
#define BOARD_NAME  "AT91SAM9260-EK"
#elif defined(CHARON2)
#define BOARD_NAME  "Charon 2"
#elif defined(ARTHERNET1)
#define BOARD_NAME  "Arthernet 1"
#elif defined(MMNET02)
#define BOARD_NAME  "MMNET02"
#elif defined(ETHERNUT1)
#define BOARD_NAME  "Ethernut 1"
#elif defined(ETHERNUT2)
#define BOARD_NAME  "Ethernut 2"
#elif defined(ETHERNUT3)
#define BOARD_NAME  "Ethernut 3"
#elif defined(ETHERNUT5)
#define BOARD_NAME  "Ethernut 5"
#else
#define BOARD_NAME  "Custom"
#endif

#define MAG_SYS_DESCR	    1
#define MAG_SYS_OID	    2
#define MAG_SYS_UPTIME      3
#define MAG_SYS_CONTACT	    4
#define MAG_SYS_NAME	    5
#define MAG_SYS_LOCATION    6
#define MAG_SYS_SERVICES    7

static uint8_t *MibVarsSysGet(CONST SNMPVAR *, OID *, size_t *, int, size_t *, WMETHOD **);

static OID base_oid[] = { SNMP_OID_MIB2, 1 };
static size_t base_oidlen = sizeof(base_oid) / sizeof(OID);

static SNMPVAR mib_variables[] = {
    {MAG_SYS_DESCR, ASN_OCTET_STR, ACL_RONLY, MibVarsSysGet, 1, {1}},
    {MAG_SYS_OID, ASN_OBJECT_ID, ACL_RONLY, MibVarsSysGet, 1, {2}},
    {MAG_SYS_UPTIME, ASN_TIMETICKS, ACL_RONLY, MibVarsSysGet, 1, {3}},
    {MAG_SYS_CONTACT, ASN_OCTET_STR, ACL_RWRITE, MibVarsSysGet, 1, {4}},
    {MAG_SYS_NAME, ASN_OCTET_STR, ACL_RWRITE, MibVarsSysGet, 1, {5}},
    {MAG_SYS_LOCATION, ASN_OCTET_STR, ACL_RWRITE, MibVarsSysGet, 1, {6}},
    {MAG_SYS_SERVICES, ASN_INTEGER, ACL_RONLY, MibVarsSysGet, 1, {7}}
};

static char sys_descr[MAX_SYSSTR_LEN];
static char *sys_contact;
static char *sys_name;
static char *sys_location;
static time_t sys_starttime;
static long sys_uptime;
static long sys_services = 72;
static OID sys_oid[] = {
    SNMP_OID_ENTERPRISES,
    3444,                       /* egnite */
    2,                          /* egnite products=1 local=2 experimental=3 */
    6,                          /* egnite Nut/OS */
};

static char *UpdateStringEnv(char *name, char *var, char *value)
{
    if (var) {
        free(var);
    }
    if ((var = malloc(strlen(value) + 1)) != NULL) {
        strcpy(var, value);
        if (name) {
            setenv(name, value, 1);
        }
    }
    return var;
}

/*!
 * \brief Register MIB II system variables.
 *
 * \note Preliminary code with hard coded values.
 */
int MibRegisterSysVars(void)
{
    char *cp;

    strcpy(sys_descr, BOARD_NAME);
    strcat(sys_descr, " Nut/OS ");
    strcat(sys_descr, NutVersionString());

    if ((cp = getenv("SYSCONTACT")) != NULL) {
        sys_contact = UpdateStringEnv(NULL, sys_contact, cp);
    }
    if ((cp = getenv("SYSNAME")) != NULL) {
        sys_name = UpdateStringEnv(NULL, sys_name, cp);
    } else {
        sys_name = UpdateStringEnv(NULL, sys_name, confos.hostname);
    }
    if ((cp = getenv("SYSLOCATION")) != NULL) {
        sys_location = UpdateStringEnv(NULL, sys_location, cp);
    }

    sys_starttime = time(NULL);

    return SnmpMibRegister(base_oid, base_oidlen, mib_variables, sizeof(mib_variables) / sizeof(SNMPVAR));
}

static int MibVarsSysSet(int action, uint8_t * var_val, uint8_t var_val_type, size_t var_val_len, OID * name, size_t name_len)
{
    size_t len = SNMP_MAX_LEN;
    uint8_t *value;
    uint8_t *cp;
    size_t size;

    if (action != SNMP_ACT_COMMIT) {
        return 0;
    }

    if (var_val_type != ASN_OCTET_STR) {
        return SNMP_ERR_WRONGTYPE;
    }
    if (var_val_len > MAX_SYSSTR_LEN) {
        return SNMP_ERR_WRONGLENGTH;
    }
    size = MAX_SYSSTR_LEN;
    if ((value = (uint8_t *)malloc(MAX_SYSSTR_LEN) + 1) == NULL) {
        return SNMP_ERR_RESOURCEUNAVAILABLE;
    }
    AsnOctetStringParse(var_val, &len, &var_val_type, value, &size);
    value[size] = 0;
    for (cp = value; *cp; cp++) {
        if (!isprint(*cp)) {
            free(value);
            return SNMP_ERR_WRONGVALUE;
        }
    }
    if (action == SNMP_ACT_COMMIT) {
        switch (name[7]) {
        case 4:
            sys_contact = UpdateStringEnv("SYSCONTACT", sys_contact, (char *) value);
            break;
        case 5:
            sys_name = UpdateStringEnv("SYSNAME", sys_name, (char *) value);
            break;
        case 6:
            sys_location = UpdateStringEnv("SYSLOCATION", sys_location, (char *) value);
            break;
        }
    }
    free(value);

    return 0;
}

/*!
 * \brief Access the specified MIB variable.
 *
 * \param vp
 * \param name    Contains the name to look for, either exact or one that 
 *                is in front. On return the exact name is stored here.
 * \param namelen Number of sub identifiers in the name upon entry. On
 *                return the length of the exact name is stored here.
 * \param exact   If not zero, the name must match exactly. Otherwise we
 *                want the first name that is following the given one.
 * \param varlen  Size of the variable.
 */
static uint8_t *MibVarsSysGet(CONST SNMPVAR * vp, OID * name, size_t * namelen, int exact, size_t * varlen, WMETHOD ** wmethod)
{
    static uint8_t empty[1];
    int rc;
    OID *fullname;
    size_t fullnamelen = base_oidlen + vp->var_namelen + 1;

    fullname = malloc(fullnamelen * sizeof(OID));
    memcpy(fullname, base_oid, base_oidlen * sizeof(OID));
    memcpy(fullname + base_oidlen, vp->var_name, vp->var_namelen * sizeof(OID));
    *(fullname + fullnamelen - 1) = 0;

    rc = SnmpOidCmp(name, *namelen, fullname, fullnamelen);
    if ((exact && rc) || (!exact && rc >= 0)) {
        free(fullname);
        return NULL;
    }
    memcpy(name, fullname, fullnamelen * sizeof(OID));
    free(fullname);
    *namelen = fullnamelen;

    *wmethod = NULL;
    *varlen = sizeof(long);
    switch (vp->var_magic) {
    case MAG_SYS_DESCR:
        *varlen = strlen(sys_descr);
        return (uint8_t *) sys_descr;
    case MAG_SYS_OID:
        *varlen = sizeof(sys_oid);
        return (uint8_t *) sys_oid;
    case MAG_SYS_UPTIME:
        sys_uptime = time(NULL) - sys_starttime;
        sys_uptime *= 100;
        return (uint8_t *) & sys_uptime;
    case MAG_SYS_CONTACT:
        *wmethod = MibVarsSysSet;
        if (sys_contact) {
            *varlen = strlen(sys_contact);
            return (uint8_t *) sys_contact;
        }
        *varlen = 0;
        return empty;
    case MAG_SYS_NAME:
        *wmethod = MibVarsSysSet;
        if (sys_name) {
            *varlen = strlen(sys_name);
            return (uint8_t *) sys_name;
        }
        *varlen = 0;
        return empty;
    case MAG_SYS_LOCATION:
        *wmethod = MibVarsSysSet;
        if (sys_location) {
            *varlen = strlen(sys_location);
            return (uint8_t *) sys_location;
        }
        *varlen = 0;
        return empty;
    case MAG_SYS_SERVICES:
        return (uint8_t *) & sys_services;
    }
    return NULL;
}
