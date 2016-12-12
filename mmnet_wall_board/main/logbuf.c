/**
 *
 * DZ-MMNET-WALL: Wall control panel based on MMNet01.
 *
 * In-memory log buffer.
 *
 *
 * General logic:
 *
 * - we put logged data to circ buf
 *
 * - two separate access streams exist: web and syslog.
 *
 * - Web has access to whole buffer and shows what's we have thee at the moment
 *
 * - Syslogd thread in parallel attempts to send buffered data to syslog daemon
 *
**/

#include "runtime_cfg.h"
#include "defs.h"
#include "util.h"
#include "cgi.h"

#include <string.h>
#include <stdio.h>

#define SYSLOG_INTERNAL
#include <sys/syslog.h>
#include <time.h>

#include <sys/confnet.h>
#include <sys/confos.h>

#include <sys/thread.h>
#include <sys/timer.h>

#include <arpa/inet.h>
#include <sys/socket.h>



#define LOG_MEM_SZ 1024

static char logbuf[LOG_MEM_SZ];

static char *put_pos = logbuf;
static char *http_get_pos = logbuf;
static char *syslog_get_pos = logbuf;

static UDPSOCKET *syslog_sock;

THREAD(syslog_out, __arg);


#ifndef SYSLOG_PORT
#define SYSLOG_PORT     514
#endif

static uint32_t syslog_server;
static uint16_t syslog_port = SYSLOG_PORT;



// -----------------------------------------------------------------------
//
// Init
//
// -----------------------------------------------------------------------


void log_init( void )
{
}


void log_syslog_init( void )
{
    NutThreadCreate("SysLog", syslog_out, 0, 640);
    syslog_sock = NutUdpCreateSocket( syslog_port );
}



// -----------------------------------------------------------------------
//
// Buf ops
//
// -----------------------------------------------------------------------



static uint16_t place_to_end_of_buf( const char *p )
{
    return logbuf + LOG_MEM_SZ - p;
}


static void wrap_buf_ptr( char **get_pos_p )
{
    while( *get_pos_p >= logbuf+LOG_MEM_SZ )
        *get_pos_p -= LOG_MEM_SZ;
}


// Move forward get pos if put overlaps unread area
static void update_get_pos( char **get_pos_p, uint16_t len )
{

    if( *get_pos_p <= put_pos ) return; // we do not overlap, return; we assume put_part never crosses buffer end

    if( *get_pos_p <= (put_pos+len) )
        *get_pos_p = put_pos+len+1;

    wrap_buf_ptr( get_pos_p );
}


// -----------------------------------------------------------------------
//
// Write
//
// -----------------------------------------------------------------------


// put func worker
static uint16_t log_put_part( const char *data, uint16_t len )
{
    push_cli();

    uint16_t place_left = place_to_end_of_buf(put_pos); //logbuf + LOG_MEM_SZ - put_pos;

    update_get_pos( &http_get_pos, len );
    update_get_pos( &syslog_get_pos, len );

    if( len > place_left )
        len = place_left;

    memcpy( put_pos, data, len );

    put_pos += len;
    wrap_buf_ptr( &put_pos );

    pop_sti();

    return len;
}

// general log put func
static void log_put( const char *data, uint16_t len )
{
    while( len > 0 )
    {
        uint16_t done = log_put_part( data, len );
        if( done == 0 ) return; // can't be

        len -= done;
        data += done;
    }
}

// general log puts func
static void log_puts( const char *data )
{
    uint16_t len = strlen(data);
    log_put( data, len );
}


// -----------------------------------------------------------------------
//
// Read
//
// -----------------------------------------------------------------------


// get func worker
static uint16_t log_get_part( char *dest, uint16_t dest_sz, char **get_ptr_p )
{
    uint16_t len = 0;

    push_cli();

    if( *get_ptr_p < put_pos )
    {
        len = put_pos - *get_ptr_p;
        if( len > dest_sz )
            len = dest_sz;

        strlcpy( dest, *get_ptr_p, len );
    }
    else
    {
        len = place_to_end_of_buf( *get_ptr_p );

        if( len > dest_sz )
            len = dest_sz;

        strlcpy( dest, *get_ptr_p, len );
    }

    wrap_buf_ptr( get_ptr_p );

    pop_sti();
    return len;

}


// general log get func
static uint16_t log_get( char *data, uint16_t len, char **get_ptr_p )
{
    uint16_t rlen = 0;

    while( len > 0 )
    {
        uint16_t done = log_get_part( data, len, get_ptr_p );
        if( done == 0 )
            break;

        rlen += done;
        len -= done;
        data += done;
    }

    return rlen;
}


// -----------------------------------------------------------------------
//
// General syslog entry points
//
// -----------------------------------------------------------------------

static int syslog_fac = LOG_USER;
static int syslog_mask = 0xFF;
static int syslog_stat;

//static char *syslog_tag = ;

#define syslog_tag modbus_device_id

/*!
 * \brief Assemble syslog header.
 *
 * For internal use only.
 *
 * \param pri Value of the syslog PRI part.
 *
 */
static void mk_syslog_header(int pri)
{
    char tmp[64];

    pri &= LOG_PRIMASK | LOG_FACMASK;                   // Remove invalid bits.

    
    if( (LOG_MASK(LOG_PRI(pri)) & syslog_mask) == 0 )   // Check priority against setlog mask values
        return;

    if( (pri & LOG_FACMASK) == 0 )                      // Set default facility if none specified
        pri |= syslog_fac;

    if( snprintf(tmp, sizeof(tmp)-1, "<%d>", pri) > 0 ) // PRI field
        log_puts( tmp );

    log_put( "1", 1 );                                  // VERSION field. Note, that there is no space separator

    /* TIMESTAMP field. */
#ifdef SYSLOG_OMIT_TIMESTAMP

    log_put( " -", 2 );

#else
    {
        time_t now;
        struct _tm *tip;

        time(&now);

        tip = gmtime(&now);
        if( snprintf( tmp, sizeof(tmp)-1, " %04d-%02d-%02dT%02d:%02d:%02dZ",
                      tip->tm_year + 1900, tip->tm_mon + 1, tip->tm_mday,
                      tip->tm_hour, tip->tm_min, tip->tm_sec) > 0 )
            log_puts( tmp );

    }
#endif /* SYSLOG_OMIT_TIMESTAMP */

    /* HOSTNAME field. */
#ifdef SYSLOG_OMIT_HOSTNAME
    log_put( " -", 2 );
#else

    log_put( " ", 1 );

    if (confnet.cdn_cip_addr) {
        log_puts( inet_ntoa(confnet.cdn_cip_addr) );
    }
    else if( confos.hostname[0] ) {
        log_puts( confos.hostname );
    }
    else if (confnet.cdn_ip_addr) {
        log_puts( inet_ntoa(confnet.cdn_ip_addr) );
    } else {
        log_put( "-", 1 );
    }

#endif /* SYSLOG_OMIT_HOSTNAME */

    /* APP-NAME field. */
    //if( syslog_tag[0] ) {
        log_put( " ", 1 );
        log_puts( syslog_tag );
    //}    else        log_put( " -", 2 );

    /* No PROCID and MSGID fields. */
    log_put( " - - ", 5 );

}

/*!
 * \brief Print log message.
 *
 * Alternate form of syslog(), in which the arguments have already been captured
 * using the variable-length argument facilities.
 *
 * \param pri Priority level of this message. See syslog().
 * \param fmt Format string containing conversion specifications like printf.
 * \param ap  List of arguments.
 */
void vsyslog(int pri, const char *fmt, va_list ap)
{
    mk_syslog_header(pri); // Build the header
    char buf[128];
    if(fmt) vsnprintf( buf, sizeof(buf) - 1, fmt, ap );
}

/*!
 * \brief Print log message.
 *
 * The message is tagged with priority.
 *
 * \param pri Priority level of this message, selected from the following
 *            ordered list (high to low):
 *            - LOG_EMERG   A panic condition.
 *            - LOG_ALERT   A condition that should be corrected immediately.
 *            - LOG_CRIT    Critical conditions, e.g., hard device errors.
 *            - LOG_ERR     Errors.
 *            - LOG_WARNING Warning messages.
 *            - LOG_NOTICE  Conditions that are not error conditions, but should
 *                          possibly be handled specially.
 *            - LOG_INFO    Informational messages.
 *            - LOG_DEBUG   Messages that contain information normally of use only
 *                          when debugging a program.
 * \param fmt Format string containing conversion specifications like printf.
 */
void syslog(int pri, const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vsyslog(pri, (char *) fmt, ap);
    va_end(ap);
}



// -----------------------------------------------------------------------
//
// Syslog control
//
// -----------------------------------------------------------------------




/*!
 * \brief Set the log priority mask level.
 *
 * Calls to syslog() with a priority not set are ignored. The default allows all
 * priorities to be logged.
 *
 * \param logmask New priority mask.
 *
 * \return Previous mask.
 */
int setlogmask(int logmask)
{
    int rc = syslog_mask;

    if (logmask) {
        syslog_mask = logmask;
    }
    return rc;
}

/*!
 * \brief Set the log server's IP address.
 *
 * \param ip   IP address in network byte order. If 0, no messages will be sent out.
 * \param port Port number. If 0, then standard port is used.
 *
 * \return Previous IP or (uint32_t)-1 (all bits set) if UDP is not supported.
 */
uint32_t setlogserver(uint32_t ip, uint16_t port)
{
#ifdef SYSLOG_PERROR_ONLY
    return (uint32_t)-1;
#else
    uint32_t rc = syslog_server;

    syslog_server = ip;
    if (port) {
        syslog_port = port;
    }
    return rc;
#endif
}


#if 0
/*!
 * \brief Open logging for specialized processing.
 *
 * This function provides for more specialized processing of the messages sent by
 * syslog() and vsyslog().
 *
 * \param ident   This string that will be prepended to every message.
 * \param logstat A bit field specifying logging options, which is formed by
 *                OR'ing one or more of the following values:
 *                - LOG_CONS   Ignored but accepted for compatibility.
 *                - LOG_NDELAY Ignored but accepted for compatibility.
 *                - LOG_PERROR Additionally write the message to standard error output.
 *                - LOG_PID    Ignored but accepted for compatibility.
 * \param logfac  Encodes a default facility to be assigned to all messages that do not
 *                have an explicit facility encoded:
 *                - LOG_AUTH     The authorization system:
 *                - LOG_AUTHPRIV The same as LOG_AUTH.
 *                - LOG_CRON     The cron daemon,
 *                - LOG_DAEMON   System daemons, that are not provided for explicitly by
 *                               other facilities.
 *                - LOG_FTP      The file transfer protocol daemon,
 *                - LOG_KERN     Messages generated by the kernel. These should not be
 *                               generated by applications.
 *                - LOG_LPR      The line printer spooling system.
 *                - LOG_MAIL     The mail system.
 *                - LOG_NEWS     The network news system.
 *                - LOG_SYSLOG   Messages generated internally by syslog.
 *                - LOG_USER     This is the default facility identifier if none is specified.
 *                - LOG_UUCP     The UUCP system.
 *                - LOG_LOCAL0..LOG_LOCAL7 Reserved for local use.
 */
void openlog(const char *ident, int logstat, int logfac)
{
    closelog();

    syslog_stat = logstat;
    syslog_fac = logfac;

    if( ident && *ident )
    {
        size_t syslog_taglen = strlen(ident);
        syslog_tag = malloc(syslog_taglen + 1);
        strcpy(syslog_tag, ident);
    }

    if( syslog_sock == 0 )
        syslog_sock = NutUdpCreateSocket(514);

}

/*!
 * \brief Release system resources occupied by syslog().
 */
void closelog(void)
{
    if( syslog_tag )
    {
        free(syslog_tag);
        syslog_tag = 0;
    }

    if( syslog_sock )
    {
        NutUdpDestroySocket(syslog_sock);
        syslog_sock = 0;
    }
}
#endif

// -----------------------------------------------------------------------
//
// Syslog output thread
//
// -----------------------------------------------------------------------


THREAD(syslog_out, __arg)
{
    while(1) // to satisfy no return
    {
        if( !syslog_sock ) NutSleep(5000);

        // Have some data to send
        if( (syslog_get_pos != put_pos) && syslog_sock )
        {
            char buf[16];

            uint16_t len = log_get_part( buf, sizeof(buf), &syslog_get_pos );

            NutUdpSendTo( syslog_sock, syslog_server, syslog_port, buf, len );

            NutSleep(10); // Don't overflow
        }

        NutSleep(100);
    }
}





// -----------------------------------------------------------------------
//
// HTTP log view
//
// -----------------------------------------------------------------------


void log_http_send( FILE * stream )
{
    char *copy = http_get_pos; // don't update http_get_pos, put will push it around

    while( 1 )
    {
        char buf[16];

        uint16_t done = log_get_part( buf, sizeof(buf), &copy );
        if( done == 0 )
            break;

        fwrite( buf, done, 1, stream );
    }

}


#if 0
// We read maximum possible data chunk each call
uint16_t log_http_read( char *dest, uint16_t dest_sz )
{
    //uint16_t rlen = 0;

    if( http_get_pos == logbuf )
        return 0;

    char *copy = http_get_pos; // don't update http_get_pos, put will push it around

    return log_get( dest, dest_sz, &copy );
/*
    while( dest_sz > 0 )
    {
        uint16_t plen = log_get_part( dest, dest_sz, &copy );
        if( plen == 0 )
            break;

        rlen += plen;
        dest += plen;
        dest_sz -= plen;
    }

    return rlen;
*/
}
#endif

/*
// We read maximum possible data chunk each call
uint16_t log_http_read( char *dest, uint16_t dest_sz )
{
    uint16_t len = 0;

    if( http_get_pos == logbuf )
        return 0;

    push_cli();

    if( http_get_pos < put_pos )
    {
        len = put_pos-http_get_pos;
        if( len > dest_sz )
            len = dest_sz;

        strlcpy( dest, http_get_pos, len );
    }
    else
    {
        uint16_t part_len = place_to_end_of_buf( http_get_pos );

        if( part_len > dest_sz )
            part_len = dest_sz;

        strlcpy( dest, http_get_pos, part_len );

        len += part_len;
        dest_sz -= part_len;
        dest += part_len;

        part_len = put_pos - logbuf;

        if( part_len > dest_sz )
            part_len = dest_sz;

        strlcpy( dest, http_get_pos, part_len );

        len += part_len;
    }

    pop_sti();
    return len;
}
*/

