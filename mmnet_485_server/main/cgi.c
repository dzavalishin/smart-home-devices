#include <sys/thread.h>

#include "cgi.h"
#include "defs.h"
#include "dports.h"
#include "dht.h"
//#include "freq.h"
//#include "temperature.h"

#include <sys/confnet.h>
#include <arpa/inet.h>

#include <string.h>


static char *html_mt = "text/html";




int
    ShowTableCgi(FILE * stream, REQUEST * req,
                 prog_char *header, table_print_func_t print_next )
{
    static prog_char thead[] =
        //"<TABLE BORDER><TR><TH>Handle</TH><TH>Countdown</TH><TH>Tick Reload</TH><TH>Callback<BR>Address</TH><TH>Callback<BR>Argument</TH></TR>\r\n";
        "<TABLE BORDER>\r\n";

#if defined(__AVR_ATmega128__) || defined(__AVR_ATmega103__)
    //static prog_char tfmt[] = "<TR><TD>%04X</TD><TD>%lu</TD><TD>%lu</TD><TD>%04X</TD><TD>%04X</TD></TR>\r\n";
#else
    //static prog_char tfmt[] = "<TR><TD>%08lX</TD><TD>%lu</TD><TD>%lu</TD><TD>%08lX</TD><TD>%08lX</TD></TR>\r\n";
#endif

    static prog_char foot[] = "</TABLE></BODY></HTML>";

    NutHttpSendHeaderTop(stream, req, 200, "Ok");
    NutHttpSendHeaderBottom(stream, req, html_mt, -1);


    {
        static prog_char h1[] = "<HTML><HEAD><TITLE>";
        fputs_P(h1, stream);
        fputs(header, stream);
        static prog_char h2[] = "</TITLE>\r\n<link href=\"/screen.css\" rel=\"stylesheet\" type=\"text/css\">\r\n</HEAD><BODY><H1>";
        fputs_P(h2, stream);
        fputs(header, stream);
        static prog_char h3[] = "</H1>\r\n";
        fputs_P(h3, stream);
    }

    fputs_P(thead, stream);
    //fputs_P(columns, stream);
    print_next( stream, -1 ); // Show table header

    int i;
    for(i = 0; 1; i++) {

        //fprintf_P(stream, tfmt, (uptr_t) tnp, ticks_left, tnp->tn_ticks, (uptr_t) tnp->tn_callback, (uptr_t) tnp->tn_arg);

        if( 0 == print_next( stream, i ) )
            break;

        NutThreadYield();
    }


    fputs_P(foot, stream);
    fflush(stream);

    return 0;
}









static int CgiAnalogueInputsRow( FILE * stream, int row_no )
{

    if(row_no < 0)
    {
        static prog_char th[] = "<TR><TH> Type </TH><TH> Number </TH><TH> Value </TH></TR>\r\n";
        fputs_P(th, stream);
        return 1;
    }

#if SERVANT_NADC > 0
    if( row_no  < SERVANT_NADC )
    {
        static prog_char tfmt[] = "<TR><TD> ADC </TD><TD> %u </TD><TD> 0x%03X </TD></TR>\r\n";
        fprintf_P(stream, tfmt, row_no, adc_value[row_no] );
        return 1;
    }

    row_no -= SERVANT_NADC;
#endif

#if SERVANT_NFREQ > 0
    if( row_no  < SERVANT_NFREQ )
    {
        static prog_char tfmt[] = "<TR><TD> %s </TD><TD> %u </TD><TD> 0x%02X </TD></TR>\r\n";
        fprintf_P(stream, tfmt,  row_no&1 ? "Duty" : "Freq", row_no, freq_outs[row_no] );

        return 1;
    }

    row_no -= SERVANT_NFREQ;
#endif

#if SERVANT_NDIG > 0
    if( row_no  < SERVANT_NDIG )
    {
        static prog_char tfmt[] = "<TR><TD> Dig </TD><TD> %u </TD><TD> 0x%02X (";
        static prog_char te[] = " ) </TD></TR>\r\n";
        fprintf_P(stream, tfmt,  row_no, dig_value[row_no] );

        char bit, c = dig_value[row_no];
        for( bit = 7; bit >=0; bit-- )
        {
            fputs(" ", stream);
            fputs( c & 0x80 ? "1" : "0", stream);
            c <<= 1;
        }

        fputs_P(te, stream);

        return 1;
    }

    row_no -= SERVANT_NDIG;
#endif

#if N_TEMPERATURE_IN > 0
    if( row_no  < N_TEMPERATURE_IN )
    {
        static prog_char tfmt[] = "<TR><TD> Temp </TD><TD> %u </TD><TD> 0x%04X </TD></TR>\r\n";
        fprintf_P(stream, tfmt,  row_no, oldTemperature[row_no] );

        return 1;
    }

    row_no -= N_TEMPERATURE_IN;
#endif

#if SERVANT_DHT11
    if( row_no == 0 )
    {
        static prog_char tfmt[] = "<TR><TD> DHT11 Temp/Humid </TD><TD> %u </TD><TD> %u </TD></TR>\r\n";
        fprintf_P(stream, tfmt, dht_temperature, dht_humidity );

        return 1;
    }

    row_no--;
#endif // SERVANT_DHT11


    return 0;
}


int CgiInputs( FILE * stream, REQUEST * req )
{
    return ShowTableCgi( stream, req, "Inputs", CgiAnalogueInputsRow );
}









static int CgiNetworkRow( FILE * stream, int row_no )
{

    if(row_no < 0)
    {
        static prog_char th[] = "<TR><TH> Type </TH><TH> Name </TH><TH> Value </TH></TR>\r\n";
        fputs_P(th, stream);
        return 1;
    }


    if( row_no  == 0 )
    {
        static prog_char tfmt[] = "<TR><TD> Address </TD><TD> MAC </TD><TD> %02X:%02X:%02X:%02X:%02X:%02X </TD></TR>\r\n";
        fprintf_P(stream, tfmt,
                  mac[0],mac[1],mac[2],
                  mac[3],mac[4],mac[5]
                 );
        return 1;
    }

    if( row_no  == 1 )
    {
        static prog_char tfmt[] = "<TR><TD> Address </TD><TD> IP </TD><TD> %s </TD></TR>\r\n";
        fprintf_P(stream, tfmt, inet_ntoa(confnet.cdn_ip_addr)
                 );
        return 1;
    }

    if( row_no  == 2 )
    {
        static prog_char tfmt[] = "<TR><TD> Address </TD><TD> Configured IP </TD><TD> %s </TD></TR>\r\n";
        fprintf_P(stream, tfmt, inet_ntoa(confnet.cdn_cip_addr)
                 );
        return 1;
    }

    if( row_no  == 3 )
    {
        static prog_char tfmt[] = "<TR><TD> Address </TD><TD> Gateway </TD><TD> %s </TD></TR>\r\n";
        fprintf_P(stream, tfmt, inet_ntoa(confnet.cdn_gateway)
                 );
        return 1;
    }

    return 0;
}



int CgiNetwork( FILE * stream, REQUEST * req )
{
    return ShowTableCgi( stream, req, "Network", CgiNetworkRow );
}













static int CgiOutputsRow( FILE * stream, int row_no )
{

    if(row_no < 0)
    {
        static prog_char th[] = "<TR><TH> Type </TH><TH> Number </TH><TH> Value </TH></TR>\r\n";
        fputs_P(th, stream);
        return 1;
    }


    if( row_no  < SERVANT_NPWM )
    {
        static prog_char tfmt[] = "<TR><TD> (PWM) unimpl </TD><TD> %u </TD><TD> 0x%03X </TD></TR>\r\n";
        fprintf_P(stream, tfmt, row_no, /*adc_value[row_no]*/ 0 );
        return 1;
    }

    row_no -= SERVANT_NPWM;

#if SERVANT_NFREQ > 0
    if( row_no  < SERVANT_NFREQ )
    {
        static prog_char tfmt[] = "<TR><TD> %s </TD><TD> %u </TD><TD> 0x%02X </TD></TR>\r\n";
        fprintf_P(stream, tfmt,  row_no&1 ? "Duty" : "Freq", row_no, freq_outs[row_no] );

        return 1;
    }

    row_no -= SERVANT_NFREQ;
#endif

    if( row_no  < SERVANT_NDIGOUT )
    {
        static prog_char tfmt[] = "<TR><TD> Dig </TD><TD> %u </TD><TD> 0x%02X (";
        static prog_char te[] = " ) </TD></TR>\r\n";
        fprintf_P(stream, tfmt,  row_no, get_dig_out(row_no) );

        char bit, c = get_dig_out(row_no);
        for( bit = 7; bit >=0; bit-- )
        {
            fputs(" ", stream);
            fputs( c & 0x80 ? "1" : "0", stream);
            c <<= 1;
        }

        fputs_P(te, stream);

        return 1;
    }

    row_no -= SERVANT_NDIGOUT;


    return 0;
}


int CgiOutputs( FILE * stream, REQUEST * req )
{
    return ShowTableCgi( stream, req, "Outputs", CgiOutputsRow );
}


static void  httpSendString( FILE * stream, REQUEST * req, char *data );
static char * getNamedParameter( const char *name );


int CgiNetIO( FILE * stream, REQUEST * req )
{
    char *name = 0;
    char *value = 0;

    char tmp[100] = "";

#if 0

    if (req->req_query)
    {
        char *qp1;
        u_char i;

        strncpy( tmp, req->req_query, 99 );

        /* Extract 2 parameters. */
        for (i = 0; i < 2; i++)
        {
            char *c;
            char *p;

            if(i == 0)                c = strtok_r(req->req_query, "=", &qp1);
            else                      c = strtok_r(0, "=", &qp1);

            p = strtok_r(0, "&", &qp1);

            if( (c != 0) && (p != 0 ) )
            {
                if( 0 == strcmp( c, "name" ) )		name = p;
                if( 0 == strcmp( c, "item" ) )		name = p;
                if( 0 == strcmp( c, "val" ) )		value = p;
                if( 0 == strcmp( c, "value" ) )		value = p;
            }
        }


        //        for (i = 0; i < 3; i++) {
        //            fprintf_P(stream, PSTR("%s: %s<BR>\r\n"), c[i], p[i]);
        //        }

    }

    NutHttpProcessQueryString( req );

    if(req->req_qptrs)
    {
        char **pp;

        for( pp = req->req_qptrs; *pp; pp++ )
        {
            char *qp1;

            char *c = strtok_r(*pp, "=", &qp1);
            char *p = strtok_r(0, "=", &qp1);

            if( (c != 0) && (p != 0 ) )
            {
                if( 0 == strcmp( c, "name" ) )		name = p;
                if( 0 == strcmp( c, "item" ) )		name = p;
                if( 0 == strcmp( c, "val" ) )		value = p;
                if( 0 == strcmp( c, "value" ) )		value = p;
            }
        }
    }
#else
    if (req->req_query)
    {
        char *pname;
        char *pvalue;
        int i;
        int count;

        strncpy( tmp, req->req_query, 99 );

        count = NutHttpGetParameterCount(req);
        /* Extract count parameters. */
        for (i = 0; i < count; i++) {
            pname = NutHttpGetParameterName(req, i);
            pvalue = NutHttpGetParameterValue(req, i);

            /* Send the parameters back to the client. */

//            fprintf_P(stream, PSTR("%s: %s<BR>\r\n"), pname, pvalue);

            if( 0 == strcmp( pname, "name" ) )		name = pvalue;
            if( 0 == strcmp( pname, "item" ) )		name = pvalue;
            if( 0 == strcmp( pname, "val" ) )		value = pvalue;
            if( 0 == strcmp( pname, "value" ) )		value = pvalue;


        }
    }

#endif

    if( name == 0 )
    {
        //httpSendString( stream, "Error: must be 'name' and, possibly, 'value' parameters" );
//    errmsg:
        //NutHttpSendHeaderTop(stream, req, 500, "Must have ?name= or ?name=&value=, name: adc{0-7}, dig{0-63}, temp{0-7}");
        NutHttpSendHeaderTop(stream, req, 200, "Ok");
        NutHttpSendHeaderBottom(stream, req, html_mt, -1);
        static prog_char h1[] = "<HTML><body> Must have ?name= or ?name=&value=, name: adc{0-7}, dig{0-63}, temp{0-7}, q='%s' </body></HTML>";
        fprintf_P( stream, h1, tmp );
        return 0;
    }

    char *data = getNamedParameter( name );
    if( data == 0 )
    {
        //        goto errmsg;
        NutHttpSendHeaderTop(stream, req, 200, "Ok");
        NutHttpSendHeaderBottom(stream, req, html_mt, -1);
        static prog_char h1[] = "<HTML><body>no data for %s</body></HTML>";
        fprintf_P(stream, h1, name );
        return 0;
    }

    httpSendString( stream, req, data );

    return 0;
}



static void  httpSendString( FILE * stream, REQUEST * req, char *data )
{
    NutHttpSendHeaderTop(stream, req, 200, "Ok");
    NutHttpSendHeaderBottom(stream, req, html_mt, -1);

    static prog_char h1[] = "<HTML><body>";
    fputs_P(h1, stream);

    fputs(data, stream);

    static prog_char h2[] = "</body></HTML>";
    fputs_P(h2, stream);

    fflush(stream);
}

static char * getNamedParameter( const char *name )
{
    int nin;
    static char out[16];


    if( 0 == strncmp( name, "adc", 3 ) )
    {
        nin = atoi( name + 3 );
        if( nin >= SERVANT_NADC ) nin = -1; // SERVANT_NADC-1;

        if( nin < 0 ) return 0;

        sprintf( out, "%d", adc_value[nin] );
        return out;
    }

#if SERVANT_NDIG > 0
    if( 0 == strncmp( name, "dig", 3 ) )
    {
        nin = atoi( name + 3 );
        if( nin >= SERVANT_NDIG*8 ) nin = -1; //(SERVANT_NDIG*8) - 1;

        if( nin < 0 ) return 0;

        char port = nin / 8;
        char bit = nin % 8;

        sprintf( out, "%d", (dig_value[port] >> bit) & 1 );
        return out;
    }
#endif

    return 0;
}




