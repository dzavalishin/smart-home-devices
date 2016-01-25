/**
 *
 * DZ-MMNET-CHARGER: Acc charger module based on MMNet101.
 *
 * General HTML code/helpers.
 *
**/


#include <string.h>
#include <stdio.h>

#include <arpa/inet.h>
#include <sys/thread.h>
#include <sys/timer.h>
#include <sys/socket.h>
#include <pro/httpd.h>

// httpd
THREAD(Service, arg);




#define HTML(__text) do {     static prog_char form[] = __text;    fputs_P(form, stream); } while(0)

extern char *html_mtype; // "text/html"


void web_header_200(FILE * stream, REQUEST * req);

void httpSendString( FILE * stream, REQUEST * req, char *data ); // Send full HTTP reply with this string in body. Good for err msg.


// Returns 0 if nothing to show more
typedef int (*table_print_func_t)( FILE * stream, int row_no ) ;

// Generate page with table-structured data printout
int ShowTableCgi(FILE * stream, REQUEST * req, prog_char *header, table_print_func_t print_next );

void ShowTableRow2(FILE * stream, char *c1, char *c2 );
void ShowTableRow2i(FILE * stream, char *c1, int c2 );
void ShowTableRow2b(FILE * stream, char *c1, int c2 ); // Bool - yes/No

void ShowTableRow3(FILE * stream, char *c1, char *c2, char *c3 );





