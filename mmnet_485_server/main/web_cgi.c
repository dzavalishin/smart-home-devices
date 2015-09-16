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

#include "io_dig.h"
#include "io_adc.h"
#include "io_pwm.h"
#include "io_dht.h"
#include "io_temp.h"
#include "io_bmp180.h"

#include "ds18x20.h"

#include <modbus.h> // err codes

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
        double fav = adc_value[row_no] / 400.0;
        char str[20];
        dtostrf( fav, 5, 3, str );
        static prog_char tfmt[] = "<TR><TD> ADC </TD><TD> %u </TD><TD> %s (0x%03X) </TD></TR>\r\n";
        fprintf_P(stream, tfmt, row_no, str, adc_value[row_no] );
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
        if( (row_no == 0) || (row_no == 2) ) return 1; // Skip ports A and C - used as address/data bus in MMNET

        unsigned char pv = dio_read_port( row_no );
        unsigned char ddrv = dio_get_port_ouput_mask( row_no );

        static prog_char tfmt[] = "<TR><TD> Dig </TD><TD> %u %c </TD><TD> 0x%02X (";
        //static prog_char dfmt[] = "<TR><TD> &nbsp;ddr </TD><TD> %u </TD><TD> 0x%02X (";
        static prog_char te[] = " ) </TD></TR>\r\n";

        fprintf_P(stream, tfmt,  row_no, 'A'+row_no, pv );	//dig_value[row_no] );

        char bit, c = pv, d  = ddrv;
        for( bit = 7; bit >=0; bit-- )
        {
            char isout = d & 0x80;
            d <<= 1;

            fputs(isout? " <b>": " ", stream);
            fputs( c & 0x80 ? "1" : "0", stream);
            fputs(isout? "</b>": "", stream);
            c <<= 1;
        }

        fputs_P(te, stream);
        return 1;
    }

    row_no -= SERVANT_NDIG;
#endif

#if SERVANT_NTEMP > 0
    if( row_no  < SERVANT_NTEMP )
    {
        char buf[30];

        uint8_t id = gTempSensorIDs[row_no][0];
        char *name = "?";

        if ( id == DS18S20_ID ) name = "18S";
        if ( id == DS18B20_ID ) name = "18B";

        static prog_char tfmt[] = "<TR><TD> Temp </TD><TD> %u (%s) </TD><TD> %s &deg;C(0x%04X) </TD></TR>\r\n";
        fprintf_P(stream, tfmt,  row_no, name, temptoa(currTemperature[row_no],buf) , currTemperature[row_no] );

        return 1;
    }

    row_no -= SERVANT_NTEMP;
#endif

#if SERVANT_DHT11
    if( row_no == 0 )
    {
        static prog_char tfmt[] = "<TR><TD> DHT11 Temperature </TD><TD>&nbsp;</TD><TD>%u &deg;C</TD></TR>\r\n";
        fprintf_P(stream, tfmt, dht_temperature );

        return 1;
    }
    if( row_no == 1 )
    {
        static prog_char tfmt[] = "<TR><TD> DHT11 Humidity </TD><TD>&nbsp;</TD><TD> %u %%</TD></TR>\r\n";
        fprintf_P(stream, tfmt, dht_humidity );

        return 1;
    }

    row_no -= 2;
#endif // SERVANT_DHT11

#if SERVANT_BMP180
    if( row_no == 0 )
    {
        double fav = bmp180_temperature / 10.0;
        char str[20];
        dtostrf( fav, 4, 1, str );
        //static prog_char tfmt[] = "<TR><TD> BMP180 Temp/Pressure &nbsp;</TD><TD> %ld (%d) </TD><TD> %ld (%ld) </TD></TR>\r\n";
        //fprintf_P(stream, tfmt, bmp180_temperature, bmp180_temperature_raw, bmp180_pressure, bmp180_pressure_raw );
        static prog_char tfmt[] = "<TR><TD> BMP180 Temperature &nbsp;</TD><TD>&nbsp;</TD><TD>%s &deg;C</TD></TR>\r\n";
        fprintf_P(stream, tfmt, str );

        return 1;
    }
    if( row_no == 1 )
    {
        //static prog_char tfmt[] = "<TR><TD> BMP180 Temp/Pressure &nbsp;</TD><TD> %ld (%d) </TD><TD> %ld (%ld) </TD></TR>\r\n";
        //fprintf_P(stream, tfmt, bmp180_temperature, bmp180_temperature_raw, bmp180_pressure, bmp180_pressure_raw );
        static prog_char tfmt[] = "<TR><TD> BMP180 Pressure &nbsp;</TD><TD>&nbsp;</TD><TD> %ld Pa</TD></TR>\r\n";
        fprintf_P(stream, tfmt, bmp180_pressure );

        return 1;
    }

    row_no -= 2;
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
        static prog_char tfmt[] = "<TR><TD> PWM </TD><TD> %u </TD><TD> 0x%03X </TD></TR>\r\n";
        fprintf_P(stream, tfmt, row_no, pwm[row_no] );
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
        static prog_char h1[] = "<HTML><body> Must have ?name= or ?name=&value=, name: adc{0-7}, dig{0-63}, temp{0-7}, q='%s' </body></HTML>";
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
    }
    return 0;
}




static char * getNamedParameter( const char *name )
{
    int nin;
    static char out[30];


#if SERVANT_NADC > 0
    if( 0 == strncmp( name, "adc", 3 ) )
    {
        nin = atoi( name + 3 );
        if( nin >= SERVANT_NADC ) nin = -1; // SERVANT_NADC-1;

        if( nin < 0 ) return 0;

        sprintf( out, "%d", adc_value[nin] );
        return out;
    }
#endif

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

#if SERVANT_NTEMP > 0
    if( 0 == strncmp( name, "temp", 4 ) )
    {
        nin = atoi( name + 4 );
        if( nin >= SERVANT_NTEMP ) nin = -1;

        if( nin < 0 ) return 0;

        // todo float point?
        sprintf( out, "%d", currTemperature[nin] );
        return out;
    }
#endif // SERVANT_NTEMP

#if SERVANT_DHT11
    if( 0 == strcmp( name, "dht-h" ) ) return itoa( dht_humidity, out, 10 );
    if( 0 == strcmp( name, "dht-t" ) ) return itoa( dht_temperature, out, 10 );
#endif // SERVANT_DHT11

#if SERVANT_BMP180
    if( 0 == strcmp( name, "bmp-p" ) ) return ltoa( bmp180_pressure, out, 10 );
    if( 0 == strcmp( name, "bmp-t" ) ) return ltoa( bmp180_temperature, out, 10 );
#endif // SERVANT_BMP180


    return 0;
}



static int setNamedParameter( const char *name, const char *value )
{
    int nout = atoi( name + 3 );
#if SERVANT_NPWM > 0
    if( 0 == strncmp( name, "pwm", 3 ) )
    {
        if( nout >= SERVANT_NPWM ) nout = -1;

        if( nout < 0 ) return MODBUS_EXCEPTION_ILLEGAL_DATA_ADDFRESS;

        set_an( (unsigned char)nout, (unsigned char) atoi(value) );

        return 0;
    }
#endif

#if SERVANT_NDIG > 0
    {
        unsigned char port = nout / 8;
        unsigned char bit = nout % 8;

        if( 0 == strncmp( name, "dig", 3 ) )
        {
            if( nout >= SERVANT_NDIG*8 ) nout = -1;
            if( nout < 0 ) return MODBUS_EXCEPTION_ILLEGAL_DATA_ADDFRESS;

            dio_write_port_bit( port, bit, atoi(value) ? 0xFF : 0 );
            return 0;
        }

        if( 0 == strncmp( name, "ddr", 3 ) )
        {
            if( nout >= SERVANT_NDIG*8 ) nout = -1;
            if( nout < 0 ) return MODBUS_EXCEPTION_ILLEGAL_DATA_ADDFRESS;

            dio_set_port_ouput_mask_bit( port, bit, atoi(value) ? 0xFF : 0 );
            return 0;
        }

    }
#endif

    return MODBUS_EXCEPTION_ILLEGAL_DATA_ADDFRESS;
}




