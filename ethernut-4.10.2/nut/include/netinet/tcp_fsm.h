#ifndef _NETINET_TCP_FSM_H_
#define _NETINET_TCP_FSM_H_

/*
 * Copyright (C) 2001-2003 by egnite Software GmbH. All rights reserved.
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
 * Portions Copyright (c) 1983, 1993 by
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
 * Revision 1.2  2004/07/30 19:36:29  drsung
 * Added a comment and changed the prototype of NutTcpStateRetranTimeout.
 *
 * Revision 1.1.1.1  2003/05/09 14:41:17  haraldkipp
 * Initial using 3.2.1
 *
 * Revision 1.6  2003/02/04 18:00:46  harald
 * Version 3 released
 *
 * Revision 1.5  2002/06/26 17:29:22  harald
 * First pre-release with 2.4 stack
 *
 */

/*!
 * \file netinet/tcp_fsm.h
 * \brief TCP state machine definitions.
 */

#ifdef __cplusplus
extern "C" {
#endif

#define TCP_NSTATES         11  /*!< \brief Total number of possible states. */

/*
 * Warning: Do NOT change the values or the order of the TCP states.
 * Some functions in tcpsm.c rely on the the order below.
 */
#define TCPS_CLOSED         0   /*!< \brief closed */
#define TCPS_LISTEN         1   /*!< \brief listening for connection */
#define TCPS_SYN_SENT       2   /*!< \brief active, have sent syn */
#define TCPS_SYN_RECEIVED   3   /*!< \brief have sent and received syn */
#define TCPS_ESTABLISHED    4   /*!< \brief established */
#define TCPS_CLOSE_WAIT     5   /*!< \brief rcvd fin, waiting for close */
#define TCPS_FIN_WAIT_1     6   /*!< \brief have closed, sent fin */
#define TCPS_CLOSING        7   /*!< \brief closed xchd FIN; await FIN ACK */
#define TCPS_LAST_ACK       8   /*!< \brief had fin and close; await FIN ACK */
#define TCPS_FIN_WAIT_2     9   /*!< \brief have closed, fin is acked */
#define TCPS_TIME_WAIT      10  /*!< \brief in 2*msl quiet wait after close */

extern void NutTcpStateMachine(NETBUF *nb);
extern int NutTcpInitStateMachine(void);

extern int NutTcpStatePassiveOpenEvent(TCPSOCKET *sock);
extern int NutTcpStateActiveOpenEvent(TCPSOCKET *sock);
extern int NutTcpStateCloseEvent(TCPSOCKET *sock);
extern int NutTcpStateWindowEvent(TCPSOCKET *sock);

extern int NutTcpStateRetranTimeout(TCPSOCKET *sock);

#ifdef __cplusplus
}
#endif

#endif
