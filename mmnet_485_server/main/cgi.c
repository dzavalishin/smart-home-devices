/**
 *
 * DZ-MMNET-MODBUS: Modbus/TCP I/O module based on MMNet101.
 *
 * HTTPD/CGI code for program-specific (not OS) data.
 *
**/

#include <sys/thread.h>

#include "defs.h"
#include "runtime_cfg.h"

#include "cgi.h"
#include "dports.h"
#include "adc.h"
#include "dht.h"
//#include "freq.h"
//#include "temperature.h"

#include <sys/confnet.h>
#include <arpa/inet.h>

#include <string.h>

#include "web.h"












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
        unsigned char pv = dio_read_port( row_no );
        unsigned char ddrv = dio_get_port_ouput_mask( row_no );

        static prog_char tfmt[] = "<TR><TD> Dig </TD><TD> %u </TD><TD> 0x%02X (";
        static prog_char dfmt[] = "<TR><TD> &nbsp;ddr </TD><TD> %u </TD><TD> 0x%02X (";
        static prog_char te[] = " ) </TD></TR>\r\n";

        fprintf_P(stream, tfmt,  row_no, pv );	//dig_value[row_no] );

        char bit, c = pv;
        for( bit = 7; bit >=0; bit-- )
        {
            fputs(" ", stream);
            fputs( c & 0x80 ? "1" : "0", stream);
            c <<= 1;
        }

        fputs_P(te, stream);

        fprintf_P(stream, dfmt,  row_no, ddrv );

        c = ddrv;
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





static void put_addr_row( FILE * stream, char *n1, char *n2, uint32_t addr )
{
    static prog_char tfmt[] = "<TR><TD>&nbsp;%s&nbsp;</TD><TD>&nbsp;%s&nbsp;</TD><TD>&nbsp;%s&nbsp;</TD></TR>\r\n";
    if( n1 == 0 ) n1 = "IP Address";
    fprintf_P( stream, tfmt, n1, n2, inet_ntoa(addr) );
}



static int CgiNetworkRow( FILE * stream, int row_no )
{

    if(row_no < 0)
    {
        static prog_char th[] = "<TR><TH> Type </TH><TH> Name </TH><TH> Value </TH></TR>\r\n";
        fputs_P(th, stream);
        return 1;
    }

/*

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
*/
    switch( row_no )
    {
    case 0:
    {
        static prog_char tfmt[] = "<TR><TD>&nbsp;MAC Address&nbsp;</TD><TD>&nbsp;MAC&nbsp;</TD><TD>&nbsp;%02X:%02X:%02X:%02X:%02X:%02X&nbsp;</TD></TR>\r\n";
        fprintf_P(stream, tfmt,
                  ee_cfg.mac_addr[0], ee_cfg.mac_addr[1], ee_cfg.mac_addr[2],
                  ee_cfg.mac_addr[3], ee_cfg.mac_addr[4], ee_cfg.mac_addr[5]
                 );

    }
    break;

    case 1: put_addr_row( stream, 0, "Net Mask", confnet.cdn_ip_mask );		break;
    case 2: put_addr_row( stream, 0, "Current", confnet.cdn_ip_addr );       	break;
    case 3: put_addr_row( stream, 0, "Configured", confnet.cdn_cip_addr );	break;
    case 4: put_addr_row( stream, 0, "Gateway", confnet.cdn_gateway );		break;

    case 5: put_addr_row( stream, 0, "EEPROM Net Mask", ee_cfg.ip_mask );	break;
    case 6: put_addr_row( stream, 0, "EEPROM Address", ee_cfg.ip_addr );       	break;

    case 7: put_addr_row( stream, 0, "EEPROM NNTP server", ee_cfg.ip_nntp );	break;
    case 8: put_addr_row( stream, 0, "EEPROM Syslog server", ee_cfg.ip_syslog );break;

    default:
        return 0;
    }

    return 1;
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

#if 0 // inputs shows actual walue
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
#endif

    return 0;
}


int CgiOutputs( FILE * stream, REQUEST * req )
{
    return ShowTableCgi( stream, req, "Outputs", CgiOutputsRow );
}


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
        web_header_200(stream, req);
        static prog_char h1[] = "<HTML><body> Must have ?name= or ?name=&value=, name: adc{0-7}, dig{0-63}, temp{0-7}, q='%s' </body></HTML>";
        fprintf_P( stream, h1, tmp );
        return 0;
    }

    char *data = getNamedParameter( name );
    if( data == 0 )
    {
        //        goto errmsg;
        web_header_200(stream, req);
        static prog_char h1[] = "<HTML><body>no data for %s</body></HTML>";
        fprintf_P(stream, h1, name );
        return 0;
    }

    httpSendString( stream, req, data );

    return 0;
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

        unsigned char port = nin / 8;
        unsigned char bit = nin % 8;

        //sprintf( out, "%d", (dig_value[port] >> bit) & 1 );
        sprintf( out, "%d", dio_read_port_bit( port, bit ) );
        return out;
    }
#endif

    return 0;
}




