/**
 *
 * DZ-MMNET-CHARGER: Acc charger module based on MMNet101.
 *
 * HTTPD/CGI code for program-specific (not OS) data.
 *
**/

#include <sys/thread.h>

#include "defs.h"
#include "servant.h"
#include "runtime_cfg.h"

#include "io_dig.h"
#include "io_adc.h"
#include "io_pwm.h"
#include "io_temp.h"

#include "prop.h"
#include "errno.h"

#include "ds18x20.h"

#include <modbus.h> // err codes

#include <sys/confnet.h>
#include <arpa/inet.h>

#include <string.h>

#include "web.h"



#define DEBUG 1








static int CgiAnalogueInputsRow( FILE * stream, int row_no )
{

    if(row_no < 0)
    {
        static prog_char th[] = "<TR><TH> Driver </TH><TH> Property </TH><TH> Value </TH></TR>\r\n";
        fputs_P(th, stream);
        return 1;
    }

    // scan through drivers

    if( row_no >= n_major_total )
        return 0;

    dev_major *dev = devices[row_no];

    static prog_char tfmt[] = "<TR><TD> %s </TD><TD> %s </TD><TD> </TD></TR>\r\n";
    fprintf_P(stream, tfmt, dev->name, dev->started ? "run" : "stop" );

    if( dev->prop )
    {
        if(DEBUG) printf("%s has properties\n", dev->name );
        int nprop = 0;
        for(;;)
        {
            char pName[32];
            char buf2[32] = "(?)";

            errno_t rc = dev_drv_listproperties( dev, nprop++, pName, sizeof(pName)-1 );
            if( rc )
                break;

            uint8_t rw = dev_drv_property_rw( dev, pName );

            rc = dev_drv_getproperty( dev, pName, buf2, sizeof(buf2)-1 );
            if( rc && DEBUG )
                printf("dev_drv_getproperty() = %d\n", rc);

            static prog_char tfmt2[] = "<TR><TD> </TD><TD> %s </TD><TD>= %s %s</TD></TR>\r\n";
            fprintf_P(stream, tfmt2, pName, buf2, rw ? "(rw)" : "(ro)" );
        }

    }

    return 1;
}


int CgiInputs( FILE * stream, REQUEST * req )
{
    return ShowTableCgi( stream, req, "Drivers", CgiAnalogueInputsRow );
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
        //static prog_char th[] = "<TR><TH> Channel </TH><TH> &nbsp;&nbsp; </TH><TH> Value </TH></TR>\r\n";
        static prog_char th[] = "<TR><TH width=\"150\"> Dev </TH><TH width=\"150\"> Channel </TH><TH width=\"150\"> Value </TH><TH width=\"150\"> IO </TH><TH width=\"150\"> Err </TH></TR>\r\n";
        fputs_P(th, stream);
        return 1;
    }

    if( row_no  <  n_minor_total )
    {
        dev_minor *minor = dev_get_minor( row_no );

        char buf[32] = "";

        if( minor->number == 0 )
        {
            dev_major* d = minor->dev;

            if( d->to_string )
                d->to_string( minor, buf, sizeof(buf) );

            static prog_char dh[] = "<TR><td> %s </td><td>  </td><td> %s </td><td>  </td><td>  </td></TR>\r\n";
            fprintf_P(stream, dh, d->name, buf );
        }

        //buf[0] = '?';
        //buf[1] = 0;
        if(minor->to_string)
        {
            static prog_char mh[] = "<TR><td>  </td><td> %s </td><td> %s </td><td> %u </td><td> %u </td></TR>\r\n";
            //int8_t ret =
            uint8_t rc = minor->to_string( minor, buf, sizeof(buf) );
            if(!rc) fprintf_P(stream, mh, minor->name, buf, minor->io_count, minor->err_count );
        }

        return 1;
    }

    row_no -= n_minor_total;


    return 0;
}


int CgiOutputs( FILE * stream, REQUEST * req )
{
    return ShowTableCgi( stream, req, "Ports", CgiOutputsRow );
}


static char * getNamedParameter( const char *name );
static int setNamedParameter( const char *name, const char *value );


int CgiNetIO( FILE * stream, REQUEST * req )
{
    char *name = 0;
    char *value = 0;

    char tmp[100] = "";

    if (req->req_query)
    {
        char *pname;
        char *pvalue;
        int i;
        int count;

        strncpy( tmp, req->req_query, sizeof(tmp)-1 );

        count = NutHttpGetParameterCount(req);
        /* Extract count parameters. */
        for (i = 0; i < count; i++)
        {
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


    if( name == 0 )
    {
        //httpSendString( stream, "Error: must be 'name' and, possibly, 'value' parameters" );
//    errmsg:
        //NutHttpSendHeaderTop(stream, req, 500, "Must have ?name= or ?name=&value=, name: adc{0-7}, dig{0-63}, temp{0-7}");
        web_header_200(stream, req);
        static prog_char h1[] = "<HTML><body> Must have ?name= or ?name=&value=, name: adc{0-7}, temp{0-7}, q='%s' </body></HTML>";
        fprintf_P( stream, h1, tmp );
        return 0;
    }

    if( value )
    {
        // Write
        int rc = setNamedParameter( name, value );

        if( rc )
        {
            web_header_200(stream, req);
            static prog_char h1[] = "<HTML><body>set %s rc=%d</body></HTML>";
            fprintf_P(stream, h1, name, rc );
            return 0;
        }

        httpSendString( stream, req, "Ok" );
        notice_activity();
    }
    else
    {
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
        notice_activity();
    }
    return 0;
}




static char * getNamedParameter( const char *name )
{
    static char out[64];

    int8_t rc = dev_global_to_string( name, out, sizeof( out ) );	// 0 - success
    if(rc)
        return 0;

    return out;
}



static int setNamedParameter( const char *name, const char *value )
{
    int8_t rc = dev_global_from_string( name, value );          // 0 - success
    if( !rc ) return 0;

    return MODBUS_EXCEPTION_ILLEGAL_DATA_ADDFRESS;
}




