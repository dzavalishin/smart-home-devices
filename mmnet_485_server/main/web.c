/**
 *
 * DZ-MMNET-MODBUS: Modbus/TCP I/O module based on MMNet101.
 *
 * General HTML code/helpers.
 *
**/


#include "defs.h"
#include "runtime_cfg.h"

#include "web.h"


char *html_mtype = "text/html";


void web_header_200(FILE * stream, REQUEST * req)
{
    NutHttpSendHeaderTop(stream, req, 200, "Ok");
    NutHttpSendHeaderBottom(stream, req, html_mtype, -1);
}

void  httpSendString( FILE * stream, REQUEST * req, char *data )
{
    web_header_200(stream, req);

    static prog_char h1[] = "<HTML><body>";
    fputs_P(h1, stream);

    fputs(data, stream);

    static prog_char h2[] = "</body></HTML>";
    fputs_P(h2, stream);

    fflush(stream);
}


int
ShowTableCgi(FILE * stream, REQUEST * req, prog_char *header, table_print_func_t print_next )
{
    web_header_200(stream, req);

    HTML("<HTML><HEAD><TITLE>");
    fputs(header, stream);
    HTML("</TITLE>\r\n<link href=\"/screen.css\" rel=\"stylesheet\" type=\"text/css\">\r\n</HEAD><BODY><H1>");
    fputs(header, stream);
    HTML("</H1>\r\n");


    HTML("<TABLE>\r\n");

    print_next( stream, -1 ); // Show table header

    int i;
    for(i = 0; 1; i++)
    {
        if( 0 == print_next( stream, i ) )
            break;

        NutThreadYield();
    }

    HTML("</TABLE>");
    HTML("<hr><br><a href=\"/\">Main page</a>");
    HTML("</BODY></HTML>");

    fflush(stream);

    return 0;
}

void
ShowTableRow2(FILE * stream, char *c1, char *c2 )
{
    static prog_char tfmt[] = "<TR><TD>&nbsp;%s&nbsp;</TD><TD>&nbsp;%s&nbsp;</TD></TR>\r\n";
    fprintf_P(stream, tfmt, c1, c2 );
}


void
ShowTableRow2i(FILE * stream, char *c1, int c2 )
{
    static prog_char tfmt[] = "<TR><TD>&nbsp;%s&nbsp;</TD><TD>&nbsp;%d&nbsp;</TD></TR>\r\n";
    fprintf_P(stream, tfmt, c1, c2 );
}


void
ShowTableRow3(FILE * stream, char *c1, char *c2, char *c3 )
{
    static prog_char tfmt[] = "<TR><TD>&nbsp;%s&nbsp;</TD><TD>&nbsp;%s&nbsp;</TD><TD>&nbsp;%s&nbsp;</TD></TR>\r\n";
    fprintf_P(stream, tfmt, c1, c2, c3 );
}





