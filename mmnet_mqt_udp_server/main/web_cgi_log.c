/**
 *
 * DZ-MMNET-WALL: Wall control panel based on MMNet01.
 *
 * Web access to app log.
 *
**/

#include <sys/thread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
//#include <sys/timer.h>

#include "cgi.h"
#include "web.h"


int CgiLog(FILE * stream, REQUEST * req)
{
    static prog_char head[] = "<HTML><HEAD><link href=\"/screen.css\" rel=\"stylesheet\" type=\"text/css\"><TITLE>OS State</TITLE></HEAD><BODY>";
    static prog_char foot[] = "</BODY></HTML>";

    web_header_200(stream, req);
    fputs_P(head, stream);

#if ENABLE_LOGBUF
    log_http_send( stream );
#else
    fputs( "(logging is not compiled in)", stream );
#endif
    fputs_P(foot, stream);
    fflush(stream);

    return 0;
}
