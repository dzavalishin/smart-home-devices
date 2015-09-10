#include "web.h"


static char *html_mt = "text/html";


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
    static prog_char html_head[] = "<HTML><BODY><BR><H1>Form Result</H1><BR><BR>";
    static prog_char html_body[] = "<BR><BR><p><a href=\"../index.html\">return to main</a></BODY></HTML></p>";

    NutHttpSendHeaderTop(stream, req, 200, "Ok");
    NutHttpSendHeaderBottom(stream, req, html_mt, -1);

    /* Send HTML header. */
    fputs_P(html_head, stream);

    if (req->req_query) {

        char *qp1;
        char *c[3];
        char *p[3];
        u_char i;

        /* Extract 3 parameters. */
        for (i = 0; i < 3; i++) {
            if(i == 0) {
                /*
                 * There's no strtok in the GCC library. So we take the chance to
                 * demonstrate strtok_r().
                 */
                c[0] = strtok_r(req->req_query, "=", &qp1);

            }
            else {
                c[i] = strtok_r(0, "=", &qp1);
            }
            p[i] = strtok_r(0, "&", &qp1);
        }

        /* Send the parameters back to the client. */
        for (i = 0; i < 3; i++) {
            fprintf_P(stream, PSTR("%s: %s<BR>\r\n"), c[i], p[i]);
        }
    }

    fputs_P(html_body, stream);
    fflush(stream);

    return 0;
}




