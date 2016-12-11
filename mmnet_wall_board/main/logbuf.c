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

#include <string.h>


#define LOG_MEM_SZ 1024

static char logbuf[LOG_MEM_SZ];

static char *put_pos = logbuf;
static char *http_get_pos = logbuf;
static char *syslog_get_pos = logbuf;


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
// General syslog entry point
//
// -----------------------------------------------------------------------



// -----------------------------------------------------------------------
//
// HTTP log view
//
// -----------------------------------------------------------------------


static void log_http_send( FILE * stream )
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

