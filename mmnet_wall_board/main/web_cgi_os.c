/**
 *
 * DZ-MMNET-MODBUS: Modbus/TCP I/O module based on MMNet101.
 *
 * HTTPD/CGI - OS stats (threads, sockets, etc).
 *
**/

#include <sys/thread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/timer.h>

#include "cgi.h"
#include "web.h"

#if 0
/*
 * CGI Sample: Show request parameters.
 *
 * See httpd.h for REQUEST structure.
 *
 * This routine must have been registered by NutRegisterCgi() and is 
 * automatically called by NutHttpProcessRequest() when the client 
 * request the URL 'cgi-bin/test.cgi'.
 */
int ShowQuery(FILE * stream, REQUEST * req)
{
    char *cp;
    /*
     * This may look a little bit weird if you are not used to C programming
     * for flash microcontrollers. The special type 'prog_char' forces the
     * string literals to be placed in flash ROM. This saves us a lot of
     * precious RAM.
     */
    static prog_char head[] = "<HTML><HEAD><link href=\"/screen.css\" rel=\"stylesheet\" type=\"text/css\"><TITLE>Parameters</TITLE></HEAD><BODY><H1>Parameters</H1>";
    static prog_char foot[] = "</BODY></HTML>";
    static prog_char req_fmt[] = "Method: %s<BR>\r\nVersion: HTTP/%d.%d<BR>\r\nContent length: %ld<BR>\r\n";
    static prog_char url_fmt[] = "URL: %s<BR>\r\n";
    static prog_char query_fmt[] = "Argument: %s<BR>\r\n";
    static prog_char type_fmt[] = "Content type: %s<BR>\r\n";
    static prog_char cookie_fmt[] = "Cookie: %s<BR>\r\n";
    static prog_char auth_fmt[] = "Auth info: %s<BR>\r\n";
    static prog_char agent_fmt[] = "User agent: %s<BR>\r\n";

    web_header_200(stream, req);

    /* Send HTML header. */
    fputs_P(head, stream);

    /* 
     * Send request parameters. 
     */
    switch (req->req_method) {
    case METHOD_GET:
        cp = "GET";
        break;
    case METHOD_POST:
        cp = "POST";
        break;
    case METHOD_HEAD:
        cp = "HEAD";
        break;
    default:
        cp = "UNKNOWN";
        break;
    }
    fprintf_P(stream, req_fmt, cp, req->req_version / 10, req->req_version % 10, req->req_length);
    if (req->req_url)
        fprintf_P(stream, url_fmt, req->req_url);
    if (req->req_query)
        fprintf_P(stream, query_fmt, req->req_query);
    if (req->req_type)
        fprintf_P(stream, type_fmt, req->req_type);
    if (req->req_cookie)
        fprintf_P(stream, cookie_fmt, req->req_cookie);
    if (req->req_auth)
        fprintf_P(stream, auth_fmt, req->req_auth);
    if (req->req_agent)
        fprintf_P(stream, agent_fmt, req->req_agent);

    /* Send HTML footer and flush output buffer. */
    fputs_P(foot, stream);
    fflush(stream);

    return 0;
}
#endif

/*
 * CGI Sample: Show list of threads.
 *
 * This routine must have been registered by NutRegisterCgi() and is 
 * automatically called by NutHttpProcessRequest() when the client 
 * request the URL 'cgi-bin/threads.cgi'.
 */
int ShowThreads(FILE * stream, REQUEST * req)
{
    static prog_char head[] = "<HTML><HEAD><link href=\"/screen.css\" rel=\"stylesheet\" type=\"text/css\"><TITLE>Threads</TITLE></HEAD><BODY><H1>Threads</H1>\r\n"
        "<TABLE BORDER><TR><TH>Handle</TH><TH>Name</TH><TH>Priority</TH><TH>Status</TH><TH>Event<BR>Queue</TH><TH>Timer</TH><TH>Stack-<BR>pointer</TH><TH>Free<BR>Stack</TH></TR>\r\n";
#if defined(__AVR_ATmega128__) || defined(__AVR_ATmega103__)
    static prog_char tfmt[] =
        "<TR><TD>%04X</TD><TD>%s</TD><TD>%u</TD><TD>%s</TD><TD>%04X</TD><TD>%04X</TD><TD>%04X</TD><TD>%u</TD><TD>%s</TD></TR>\r\n";
#else
    static prog_char tfmt[] =
        "<TR><TD>%08lX</TD><TD>%s</TD><TD>%u</TD><TD>%s</TD><TD>%08lX</TD><TD>%08lX</TD><TD>%08lX</TD><TD>%lu</TD><TD>%s</TD></TR>\r\n";
#endif
    static prog_char foot[] = "</TABLE></BODY></HTML>";
    static char *thread_states[] = { "TRM", "<FONT COLOR=#CC0000>RUN</FONT>", "<FONT COLOR=#339966>RDY</FONT>", "SLP" };
    NUTTHREADINFO *tdp = nutThreadList;

    /* Send HTTP response. */
    web_header_200(stream, req);

    /* Send HTML header. */
    fputs_P(head, stream);
    for (tdp = nutThreadList; tdp; tdp = tdp->td_next) {
        fprintf_P(stream, tfmt, (uptr_t) tdp, tdp->td_name, tdp->td_priority,
                  thread_states[tdp->td_state], (uptr_t) tdp->td_queue, (uptr_t) tdp->td_timer,
                  (uptr_t) tdp->td_sp, (uptr_t) tdp->td_sp - (uptr_t) tdp->td_memory,
                  *((u_long *) tdp->td_memory) != DEADBEEF ? "Corr" : "OK");
    }
    fputs_P(foot, stream);
    fflush(stream);

    return 0;
}

/*
 * CGI Sample: Show list of timers.
 *
 * This routine must have been registered by NutRegisterCgi() and is 
 * automatically called by NutHttpProcessRequest() when the client 
 * request the URL 'cgi-bin/timers.cgi'.
 */
int ShowTimers(FILE * stream, REQUEST * req)
{
    static prog_char head[] = "<HTML><HEAD><link href=\"/screen.css\" rel=\"stylesheet\" type=\"text/css\"><TITLE>Timers</TITLE></HEAD><BODY><H1>Timers</H1>\r\n";
    static prog_char thead[] =
        "<TABLE BORDER><TR><TH>Handle</TH><TH>Countdown</TH><TH>Tick Reload</TH><TH>Callback<BR>Address</TH><TH>Callback<BR>Argument</TH></TR>\r\n";
#if defined(__AVR_ATmega128__) || defined(__AVR_ATmega103__)
    static prog_char tfmt[] = "<TR><TD>%04X</TD><TD>%lu</TD><TD>%lu</TD><TD>%04X</TD><TD>%04X</TD></TR>\r\n";
#else
    static prog_char tfmt[] = "<TR><TD>%08lX</TD><TD>%lu</TD><TD>%lu</TD><TD>%08lX</TD><TD>%08lX</TD></TR>\r\n";
#endif
    static prog_char foot[] = "</TABLE></BODY></HTML>";
    NUTTIMERINFO *tnp;
    u_long ticks_left;

    web_header_200(stream, req);

    /* Send HTML header. */
    fputs_P(head, stream);
    if ((tnp = nutTimerList) != 0) {
        fputs_P(thead, stream);
        ticks_left = 0;
        while (tnp) {
            ticks_left += tnp->tn_ticks_left;
            fprintf_P(stream, tfmt, (uptr_t) tnp, ticks_left, tnp->tn_ticks, (uptr_t) tnp->tn_callback, (uptr_t) tnp->tn_arg);
            tnp = tnp->tn_next;
        }
    }

    fputs_P(foot, stream);
    fflush(stream);

    return 0;
}

/*
 * CGI Sample: Show list of sockets.
 *
 * This routine must have been registered by NutRegisterCgi() and is 
 * automatically called by NutHttpProcessRequest() when the client 
 * request the URL 'cgi-bin/sockets.cgi'.
 */
int ShowSockets(FILE * stream, REQUEST * req)
{
    /* String literals are kept in flash ROM. */
    static prog_char head[] = "<HTML><HEAD><link href=\"/screen.css\" rel=\"stylesheet\" type=\"text/css\"><TITLE>Sockets</TITLE></HEAD>"
        "<BODY><H1>Sockets</H1>\r\n"
        "<TABLE BORDER><TR><TH>Handle</TH><TH>Type</TH><TH>Local</TH><TH>Remote</TH><TH>Status</TH></TR>\r\n";
#if defined(__AVR_ATmega128__) || defined(__AVR_ATmega103__)
    static prog_char tfmt1[] = "<TR><TD>%04X</TD><TD>TCP</TD><TD>%s:%u</TD>";
#else
    static prog_char tfmt1[] = "<TR><TD>%08lX</TD><TD>TCP</TD><TD>%s:%u</TD>";
#endif
    static prog_char tfmt2[] = "<TD>%s:%u</TD><TD>";
    static prog_char foot[] = "</TABLE></BODY></HTML>";
    static prog_char st_listen[] = "LISTEN";
    static prog_char st_synsent[] = "SYNSENT";
    static prog_char st_synrcvd[] = "SYNRCVD";
    static prog_char st_estab[] = "<FONT COLOR=#CC0000>ESTABL</FONT>";
    static prog_char st_finwait1[] = "FINWAIT1";
    static prog_char st_finwait2[] = "FINWAIT2";
    static prog_char st_closewait[] = "CLOSEWAIT";
    static prog_char st_closing[] = "CLOSING";
    static prog_char st_lastack[] = "LASTACK";
    static prog_char st_timewait[] = "TIMEWAIT";
    static prog_char st_closed[] = "CLOSED";
    static prog_char st_unknown[] = "UNKNOWN";
    const prog_char *st_P;
    extern TCPSOCKET *tcpSocketList;
    TCPSOCKET *ts;

    web_header_200(stream, req);

    /* Send HTML header. */
    fputs_P(head, stream);

    for (ts = tcpSocketList; ts; ts = ts->so_next) {
        switch (ts->so_state) {
        case TCPS_LISTEN:
            st_P = st_listen;
            break;
        case TCPS_SYN_SENT:
            st_P = st_synsent;
            break;
        case TCPS_SYN_RECEIVED:
            st_P = st_synrcvd;
            break;
        case TCPS_ESTABLISHED:
            st_P = st_estab;
            break;
        case TCPS_FIN_WAIT_1:
            st_P = st_finwait1;
            break;
        case TCPS_FIN_WAIT_2:
            st_P = st_finwait2;
            break;
        case TCPS_CLOSE_WAIT:
            st_P = st_closewait;
            break;
        case TCPS_CLOSING:
            st_P = st_closing;
            break;
        case TCPS_LAST_ACK:
            st_P = st_lastack;
            break;
        case TCPS_TIME_WAIT:
            st_P = st_timewait;
            break;
        case TCPS_CLOSED:
            st_P = st_closed;
            break;
        default:
            st_P = st_unknown;
            break;
        }
        /*
         * Fixed a bug reported by Zhao Weigang.
         */
        fprintf_P(stream, tfmt1, (uptr_t) ts, inet_ntoa(ts->so_local_addr), ntohs(ts->so_local_port));
        fprintf_P(stream, tfmt2, inet_ntoa(ts->so_remote_addr), ntohs(ts->so_remote_port));
        fputs_P(st_P, stream);
        fputs("</TD></TR>\r\n", stream);
        fflush(stream);
    }


    static prog_char utfmt1[] = "<TR><TD>%04X</TD><TD>UDP</TD><TD>%s:%u</TD>";
    UDPSOCKET *us;

    extern UDPSOCKET *udpSocketList;
    for (us = udpSocketList; us; us = us->so_next) {
        fprintf_P(stream, utfmt1, (uptr_t) us, ""/* was 0*/, ntohs(us->so_local_port));
        fprintf_P(stream, tfmt2, ""/* was 0*/, 0);
        //fputs_P("", stream);
        fputs("</TD></TR>\r\n", stream);
        fflush(stream);
    }


    fputs_P(foot, stream);
    fflush(stream);

    return 0;
}
