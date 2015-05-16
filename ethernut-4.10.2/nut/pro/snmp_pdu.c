/*
 * Copyright 2009 by egnite GmbH
 * Copyright 1998-2007 by egnite Software GmbH
 * Copyright 1988, 1989, 1991, 1992 by Carnegie Mellon University
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

#include <compiler.h>
#include <stdlib.h>
#include <string.h>

#include <pro/snmp_pdu.h>

/*!
 * \addtogroup xgSNMP
 */
/*@{*/

/*!
 * \brief Create PDU with a given type and object ID.
 *
 * \param cmd  PDU type.
 * \param name Object identifier.
 * \param nlen Number of sub IDs of the object identifier.
 */
SNMP_PDU *SnmpPduCreate(int cmd, CONST OID * name, size_t nlen)
{
    SNMP_PDU *pdu = calloc(1, sizeof(SNMP_PDU));

    if (pdu) {
        /* Set the PDU type. */
        pdu->pdu_cmd = cmd;
        /* Set the device identifier. */
        pdu->pdu_enterprise = malloc(nlen * sizeof(OID));
        if (pdu->pdu_enterprise) {
            memcpy(pdu->pdu_enterprise, name, nlen * sizeof(OID));
            pdu->pdu_enterprise_length = nlen;
        } else {
            free(pdu);
            pdu = NULL;
        }
    }
    return pdu;
}

/*!
 * \brief Add variable to PDU.
 *
 * \param pdu   Pointer to the PDU.
 * \param name  Variable name.
 * \param nlen  Number of sub IDs of the variable name.
 * \param type  Variable type.
 * \param value Pointer to the variable's value.
 * \param vlen  Length of the variable value in byts.
 */
int SnmpPduAddVariable(SNMP_PDU * pdu, OID * name, size_t nlen, uint8_t type, uint8_t * value, size_t vlen)
{
    SNMP_VARLIST *var = calloc(1, sizeof(SNMP_VARLIST));

    if (var == NULL) {
        return -1;
    }
    var->var_name = malloc(nlen * sizeof(OID));
    if (var->var_name == NULL) {
        free(var);
        return -1;
    }
    memcpy(var->var_name, name, nlen * sizeof(OID));
    var->var_nlen = nlen;
    var->var_type = type;
    switch (type) {
    case ASN_COUNTER:
    case ASN_GAUGE:
    case ASN_INTEGER:
    case ASN_TIMETICKS:
    case ASN_UINTEGER:
        if (vlen == 0) {
            vlen = sizeof(long);
        }
        var->var_vptr = (uint8_t *) & var->var_val;
        memcpy(var->var_vptr, value, vlen);
        var->var_vlen = sizeof(long);
        break;
    default:
        if (vlen) {
            var->var_vptr = malloc(vlen);
            if (var->var_vptr) {
                memcpy(var->var_vptr, value, vlen);
                var->var_vlen = vlen;
            }
        }
        break;
    }
    if (pdu->pdu_variables) {
        SNMP_VARLIST *vp;

        for (vp = pdu->pdu_variables; vp->var_next; vp = vp->var_next);
        vp->var_next = var;
    } else {
        pdu->pdu_variables = var;
    }
    return 0;
}

/*!
 * \brief Destroy PDU.
 *
 * \param pdu Pointer to the PDU info, obtained by a call to SnmpPduCreate().
 */
void SnmpPduDestroy(SNMP_PDU * pdu)
{
    SNMP_VARLIST *vp;
    SNMP_VARLIST *ovp;

    vp = pdu->pdu_variables;
    while (vp) {
        if (vp->var_name) {
            free(vp->var_name);
        }
        if (vp->var_vptr && vp->var_vptr != (uint8_t *) & vp->var_val) {
            free(vp->var_vptr);
        }
        ovp = vp;
        vp = vp->var_next;
        free(ovp);
    }
    if (pdu->pdu_enterprise) {
        free(pdu->pdu_enterprise);
    }
    free(pdu);
}

/*@}*/
