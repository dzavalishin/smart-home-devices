/**
 *
 * DZ-MMNET-MODBUS: Modbus/TCP I/O module based on MMNet101.
 *
 * HTTPD/CGI code for web configuration.
 *
**/


#include "defs.h"
#include "runtime_cfg.h"

#include "web.h"


static char *html_mt = "text/html";


static void form_element( FILE * stream, char *title, char *field_name, char *curr_value );

/*
 * CGI Sample: Proccessing a form.
 *
 * This routine must have been registered by NutRegisterCgi() and is 
 * automatically called by NutHttpProcessRequest() when the client 
 * request the URL 'cgi-bin/form.cgi'.
 *
 * Thanks to Tom Boettger, who provided this sample for ICCAVR.
 */
int ShowForm(FILE * stream, REQUEST * req)
{
    //static prog_char html_head[] = "<H2>Configuration</H2><BR><table>";
    //static prog_char html_body[] = "</table><BR><BR><p><a href=\"/\">return to main</a> <a href=\"/form.html\">return to form</a></BODY></HTML></p>";

    char buf[64];
    static char modified = 0;

    NutHttpSendHeaderTop(stream, req, 200, "Ok");
    NutHttpSendHeaderBottom(stream, req, html_mt, -1);

    HTML("<HTML><head><link href=\"/screen.css\" rel=\"stylesheet\" type=\"text/css\"></head><BODY>\r\n");
    HTML("<H2>Configuration</H2><BR>\r\n");

    //fputs_P(html_head, stream);

    //static prog_char form[] = "<form>\r\n";    fputs_P(form, stream);

    if (req->req_query)
    {
        char *pname;
        char *pvalue;
        int i;
        int count;

        //strncpy( tmp, req->req_query, 99 );

        count = NutHttpGetParameterCount(req);

        for (i = 0; i < count; i++) {
            pname = NutHttpGetParameterName(req, i);
            pvalue = NutHttpGetParameterValue(req, i);

            modified = 1;

            /* Send the parameters back to the client. */
            //fprintf_P(stream, PSTR("%s: %s<BR>\r\n"), pname, pvalue);

            if( 0 == strcmp( pname, "ip_addr" ) )		    ee_cfg.ip_addr	= inet_addr( pvalue );
            if( 0 == strcmp( pname, "ip_mask" ) )		    ee_cfg.ip_mask	= inet_addr( pvalue );
            if( 0 == strcmp( pname, "ip_route" ) )		    ee_cfg.ip_route	= inet_addr( pvalue );

            if( 0 == strcmp( pname, "mac5" ) )		    	    ee_cfg.mac_addr[5]	= atoi( pvalue );
            if( 0 == strcmp( pname, "tz" ) )		    	    ee_cfg.timezone	= atoi( pvalue );

            if( 0 == strcmp( pname, "ip_syslog" ) )		    ee_cfg.ip_syslog	= inet_addr( pvalue );
            if( 0 == strcmp( pname, "ip_nntp" ) )		    ee_cfg.ip_nntp	= inet_addr( pvalue );

            if( (0 == strcmp( pname, "eeprom" )) && (0 == strcmp( pvalue, "save" )) )
            {
                if( 0 == runtime_cfg_eeprom_write() )
                {
                    modified = 0;
                    HTML("<p>EEPROM write done</p>");
                }
                else
                {
                    HTML("<p>EEPROM write FAILED!</p>");
                }
            }
        }
    }

    HTML("<form>\r\n");
    HTML("<table>\r\n");


    sprintf( buf, "%d", (signed char)ee_cfg.timezone );
    form_element( stream, "TimeZone", "tz", buf );

    sprintf( buf, "%u", ee_cfg.mac_addr[5] );
    form_element( stream, "MAC byte", "mac5", buf );

    form_element( stream, "IP Address", "ip_addr", inet_ntoa(ee_cfg.ip_addr) );
    form_element( stream, "Net Mask", "ip_mask", inet_ntoa(ee_cfg.ip_mask) );
    form_element( stream, "IP gateway", "ip_route", inet_ntoa(ee_cfg.ip_route) );

    form_element( stream, "Syslog server", "ip_syslog", inet_ntoa(ee_cfg.ip_syslog) );
    form_element( stream, "NNTP server", "ip_nntp", inet_ntoa(ee_cfg.ip_nntp) );


    //static prog_char button[] = "<input type = \"submit\" value=\"send\" size=\"8\"></form>\r\n";
    //fputs_P(button, stream);

    HTML("</table><BR>");
    HTML("<input type = \"submit\" value=\"Send\" size=\"8\"></form>\r\n");

    //fputs_P(html_body, stream);

    if( modified )
    {
        HTML("<p><a href=\"/cgi-bin/form.cgi?eeprom=save\"><button>Save to EEPROM</button></a> </p>");
    }
    HTML("<p><a href=\"/\">Return to main</a></p>");
    HTML("</BODY></HTML></p>");

    fflush(stream);

    return 0;
}


static void form_element( FILE * stream, char *title, char *field_name, char *curr_value )
{
    static prog_char fmt[] =
        "<tr> <TD height=\"32\" width=\"160\">&nbsp;%s:&nbsp;</TD> <TD height=\"32\" width=\"140\"><input type=\"text\" name=\"%s\" size=\"16\" value=\"%s\"></TD> </tr>";

    fprintf_P( stream, fmt, title, field_name, curr_value );
}





