/**
 *
 * DZ-MMNET-MODBUS: Modbus/TCP I/O module based on MMNet101.
 *
 * HTTPD/CGI code for web configuration.
 *
 * URL: cgi-bin/form.cgi
 *
**/


#include "defs.h"
#include "runtime_cfg.h"

#include "web.h"

#include <time.h>

static void printTime( FILE * stream, struct _tm *stm )
{
            fprintf(stream, "%02d/%02d/%04d, %02d:%02d:%02d%s",
                   stm->tm_mday, stm->tm_mon + 1, stm->tm_year + 1900,
                   stm->tm_hour, stm->tm_min, stm->tm_sec,
                   stm->tm_isdst ? " DST" : ""
                  );
}


static int CgiStatusRow( FILE * stream, int row_no )
{
    time_t secs = time(NULL);

    switch( row_no )
    {

    case -1:
        HTML("<TR><TH> Type </TH><TH> Value </TH><TH> Comment </TH></TR>\r\n");    break;

    case 0:
        {
            HTML("<TR><TH> Time </TH><TH> ");
            printTime( stream, gmtime(&secs) );
            HTML(" </TH><TH> GMT </TH></TR>\r\n");

            //ShowTableRow3( stream, char *c1, char *c2, char *c3 );

        }
        break;

    case 1:
        {
            HTML("<TR><TH> Time </TH><TH> ");
            printTime( stream, localtime(&secs) );
            HTML(" </TH><TH> Local </TH></TR>\r\n");

            //ShowTableRow3( stream, char *c1, char *c2, char *c3 );

        }
        break;

    case 2:
        {
        uint32_t seconds = NutGetSeconds();
        uint32_t minutes = seconds / 60UL;
        uint32_t hours = minutes / 60UL;
        uint32_t days = hours / 24UL;
        minutes %= 60UL;
        seconds %= 60UL;
        hours %= 24UL;

        HTML("<TR><TH> UpTime </TH><TH> ");
        fprintf(stream, "%lu days  %lu:%lu:%lu", days, hours, minutes, seconds);
        HTML(" </TH><TH>  </TH></TR>\r\n");


        }
        break;


    default:
        return 0;
    }

    return 1;
}



int CgiStatus( FILE * stream, REQUEST * req )
{
    return ShowTableCgi( stream, req, "Status", CgiStatusRow );
}




