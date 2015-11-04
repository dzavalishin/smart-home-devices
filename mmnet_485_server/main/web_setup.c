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

#include "io_temp.h"


const unsigned char *enableBitNames = (unsigned char *)"\1ADC analogue inputs\2PWM analogue outputs\3DHT11 humidity sensor\4BMP180 pressure sensor\5TUN0 TCP-485 tunnel\6TUN1 TCP-485 tunnel\7B1W1 - single bus 1Wire\10B1W8 - multibus 1Wire\11Serial debug (UART1)";

static void subheader( FILE * stream, char *title );
static void form_element( FILE * stream, char *title, char *field_name, char *curr_value );
static void bit_form_elements( FILE * stream, unsigned int *data, const unsigned char *bitNames );
static void decode_bit( const char *pname, const char *pval, unsigned int *data );


static char * itox( int i )
{
    static char x[4];
    sprintf( x, "%02X", i );
    return x;
}

static int htoi( const char *v )
{
    int i = -1;
    sscanf( v, "%x", &i );
    return i;
}

#define DEC_X( __name_, __dest_ ) do { if( 0 == strcmp( pname, (__name_) ) ) ee_cfg.__dest_ = htoi( pvalue ); } while(0)
#define DEC_I( __name_, __dest_ ) do { if( 0 == strcmp( pname, (__name_) ) ) ee_cfg.__dest_ = atoi( pvalue ); } while(0)

static void (*boot_me)(void) = 0;


int ShowForm(FILE * stream, REQUEST * req)
{

    char buf[64];
    static char modified = 0;

    web_header_200(stream, req);

    HTML("<HTML><head><link href=\"/screen.css\" rel=\"stylesheet\" type=\"text/css\"></head><BODY>\r\n");
    HTML("<H2>Configuration</H2><BR>\r\n");


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

            DEC_X( "ddrb", ddr_b );
            DEC_X( "ddrd", ddr_d );
            DEC_X( "ddre", ddr_e );
            DEC_X( "ddrf", ddr_f );
            DEC_X( "ddrg", ddr_g );


            DEC_X( "initb", start_b );
            DEC_X( "initd", start_d );
            DEC_X( "inite", start_e );
            DEC_X( "initf", start_f );
            DEC_X( "initg", start_g );

            DEC_I( "dbg_baud", dbg_baud );
            DEC_I( "tun0_baud", tun_baud[0] );
            DEC_I( "tun1_baud", tun_baud[1] );



            if( 0 == strcmp( pname, "ip_addr" ) )		    ee_cfg.ip_addr	= inet_addr( pvalue );
            if( 0 == strcmp( pname, "ip_mask" ) )		    ee_cfg.ip_mask	= inet_addr( pvalue );
            if( 0 == strcmp( pname, "ip_route" ) )		    ee_cfg.ip_route	= inet_addr( pvalue );

            if( 0 == strcmp( pname, "mac5" ) )		    	    ee_cfg.mac_addr[5]	= atoi( pvalue );
            if( 0 == strcmp( pname, "tz" ) )		    	    ee_cfg.timezone	= atoi( pvalue );

            if( 0 == strcmp( pname, "ip_syslog" ) )		    ee_cfg.ip_syslog	= inet_addr( pvalue );
            if( 0 == strcmp( pname, "ip_nntp" ) )		    ee_cfg.ip_nntp	= inet_addr( pvalue );

            // 1w rom permanent id
            {
                char *name = "rom____";
                uint8_t i;
                for( i = 0; i < MAX_OW_MAP; i++ )
                {
                    struct ow_map_entry *e = ee_cfg.ow_map+i;

                    sprintf( name+4, "%02d", i );

                    // Have update?
                    if( 0 == strcmp( pname, name ) )
                    {
                        uint8_t v = (uint8_t)atoi( pvalue );
                        if( v >= MAX_OW_MAP ) v = -1;
                        e->index = v;
                    }



                }
            }

            decode_bit( pname, pvalue, &(ee_cfg.io_enable) );


            if( 0 == strcmp( pname, "eeprom" ))
            {
                if( 0 == strcmp( pvalue, "save" ) )
                {
                    if( 0 == runtime_cfg_eeprom_write() )
                    {
                        modified = 0;
                        HTML("<p>EEPROM write done</p>");
                    }
                    else

                        HTML("<p>EEPROM write FAILED!</p>");

                }

                if( 0 == strcmp( pvalue, "load" ) )
                {
                    if( 0 == runtime_cfg_eeprom_read() )
                    {
                        modified = 0;
                        HTML("<p>EEPROM read done</p>");
                    }
                    else
                        HTML("<p>EEPROM read FAILED!</p>");

                }

                if( 0 == strcmp( pvalue, "init" ) )
                {
                    init_runtime_cfg();         // Load defaults
                    HTML("<p>Done setting factory defaults</p>");
                }
            }

            if( 0 == strcmp( pname, "os" ))
            {
                if( 0 == strcmp( pvalue, "boot" ) )
                {
                    HTML("Reboot, continue with <a href=/>Main screen</a>\r\n</body>");
                    fflush( stream );
                    NutSleep(1000);
                    cli();
                    boot_me();
                }
            }

        }
    }

    // HTML("<form method=post>\r\n"); doesnt work
    HTML("<table cellspacing=40><tr valign=top><td>\r\n");
    HTML("<table>\r\n");

    HTML("<form class=\"setup\">\r\n");

    subheader( stream, "Time" );

    sprintf( buf, "%d", (signed char)ee_cfg.timezone );
    form_element( stream, "TimeZone", "tz", buf );

    subheader( stream, "Network" );

    sprintf( buf, "%u", ee_cfg.mac_addr[5] );
    form_element( stream, "MAC byte", "mac5", buf );

    form_element( stream, "IP Address", "ip_addr", inet_ntoa(ee_cfg.ip_addr) );
    form_element( stream, "Net Mask", "ip_mask", inet_ntoa(ee_cfg.ip_mask) );
    form_element( stream, "IP gateway", "ip_route", inet_ntoa(ee_cfg.ip_route) );

    form_element( stream, "Syslog server", "ip_syslog", inet_ntoa(ee_cfg.ip_syslog) );
    form_element( stream, "NNTP server", "ip_nntp", inet_ntoa(ee_cfg.ip_nntp) );

    subheader( stream, "Data direction" );

    form_element( stream, "DDR B", "ddrb", itox(ee_cfg.ddr_b) );
    form_element( stream, "DDR D", "ddrd", itox(ee_cfg.ddr_d) );
    form_element( stream, "DDR E", "ddre", itox(ee_cfg.ddr_e) );
    form_element( stream, "DDR F", "ddrf", itox(ee_cfg.ddr_f) );
    form_element( stream, "DDR G", "ddrg", itox(ee_cfg.ddr_g) );

    subheader( stream, "Initial value" );

    form_element( stream, "Init B", "initb", itox(ee_cfg.start_b) );
    form_element( stream, "Init D", "initd", itox(ee_cfg.start_d) );
    form_element( stream, "Init E", "inite", itox(ee_cfg.start_e) );
    form_element( stream, "Init F", "initf", itox(ee_cfg.start_f) );
    form_element( stream, "Init G", "initg", itox(ee_cfg.start_g) );

    subheader( stream, "Serial ports" );

    {
        char buf[32];
        sprintf(buf, "%ld", ee_cfg.dbg_baud );
        form_element( stream, "Debuf port baud rate", "dbg_baud", buf );
#if SERVANT_TUN0
        sprintf(buf, "%ld", ee_cfg.tun_baud[0] );
        form_element( stream, "Tunnel 0 baud rate", "tun0_baud", buf );
#endif
#if SERVANT_TUN1
        sprintf(buf, "%ld", ee_cfg.tun_baud[1] );
        form_element( stream, "Tunnel 1 baud rate", "tun1_baud", buf );
#endif
    }

    HTML("</table>");
    HTML("<br><input type = \"submit\" value=\"Send\" size=\"16\">\r\n");
    HTML("</form>\r\n");

    HTML("</td><td><br>\r\n");
    HTML("<table>\r\n");

    HTML("<form>\r\n");

    subheader( stream, "Enable peripherals" );

    bit_form_elements( stream, &(ee_cfg.io_enable), enableBitNames );


    HTML("</table>");
    HTML("<br><input type = \"submit\" value=\"Send\" size=\"16\">\r\n");
    HTML("</form>\r\n");

    HTML("</td><td>\r\n");
    HTML("<table>\r\n");

    HTML("<form>\r\n");
    subheader( stream, "1Wire t&deg; sensors" );

    {
        char *name = "rom____";
        char val[32];
        char text[64];

        uint8_t i;
        for( i = 0; i < MAX_OW_MAP; i++ )
        {
            //struct ow_map_entry *e = ee_cfg.ow_map+i;

            sprintf( name+4, "%02d", i );
/*
            // Have update?
            if( 0 == strcmp( pname, name ) )
            {
                uint8_t v = (uint8_t)atoi( pvalue );
                if( v >= MAX_OW_MAP ) v = -1;
                e->index = v;
            }
*/

            uint8_t *id = ee_cfg.ow_map[i].id;
            int perm_index = ee_cfg.ow_map[i].index;
            int cur_index = ow_id_map[perm_index];
            int8_t bus = -1;
#if !OW_ONE_BUS
            if( cur_index >= SERVANT_NTEMP ) cur_index = -1;
            bus = (cur_index < 0) ? -1 : gTempSensorBus[cur_index];
#endif
            if( (perm_index < 0) || (perm_index >= MAX_OW_MAP ) )
            {
                perm_index = cur_index = -1;
            }


            sprintf( text, "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X (@%d bus%d)",
                     id[0], id[1], id[2], id[3],
                     id[4], id[5], id[6], id[7],
                     cur_index, bus
                   );


            sprintf( val, "%d", perm_index );

            form_element( stream, text, name, val );


        }
    }

    HTML("</table>");
    HTML("<br><input type = \"submit\" value=\"Send\" size=\"16\">\r\n");
    HTML("</form>\r\n");

    HTML("</td></tr></table>\r\n");

    HTML("<BR>");




    HTML("<p></p>");
    if( modified )
        HTML("<a href=\"/cgi-bin/form.cgi?eeprom=save\"><button>Save to EEPROM</button></a> ");
    HTML("<a href=\"/cgi-bin/form.cgi?eeprom=load\"><button>Load from EEPROM</button></a> ");
    HTML("<a href=\"/cgi-bin/form.cgi?eeprom=init\"><button>Load factory defaults</button></a> ");
    HTML("<a href=\"/cgi-bin/form.cgi?os=boot\"><button>Reboot</button></a> ");
    HTML("<p></p>");

    HTML("<p><a href=\"/\">Return to main</a></p>");
    HTML("</BODY></HTML></p>");

    fflush(stream);

    return 0;
}


static void subheader( FILE * stream, char *title )
{
    static prog_char fmt[] =
        "<tr> <TH align=left height=\"22\" width=\"460\">%s:&nbsp;</TH> <TD height=\"22\" width=\"140\">&nbsp;</TD> </tr>";

    fprintf_P( stream, fmt, title );
}


static void form_element( FILE * stream, char *title, char *field_name, char *curr_value )
{
    static prog_char fmt[] =
        "<tr> <TD height=\"22\" width=\"460\">&nbsp;%s:&nbsp;</TD> <TD height=\"22\" width=\"140\"><input type=\"text\" name=\"%s\" size=\"16\" value=\"%s\"></TD> </tr>";

    fprintf_P( stream, fmt, title, field_name, curr_value );
}


static void putBitName( FILE * stream, const unsigned char *bitNames, uint8_t nBit )
{
    const unsigned char *p;
    for( p = bitNames; *p; )
    {
        char bn = *p++;

        if( bn >= ' ' ) continue; // Find bit number

        // bit number in string is +1 to actual
        if( bn-1 != nBit ) continue;

        break;
    }

    while( *p )
    {
        if( *p < ' ' )
            break;
        putc( *p++, stream );
    }
}

static void bit_form_element( FILE * stream, unsigned int *data, const unsigned char *bitNames, char nBit )
{
    static prog_char fmt1[] =
        "<tr> <TD height=\"22\" width=\"460\">&nbsp;";
    static prog_char fmt2[] =
        ":&nbsp;</TD> <TD height=\"22\" width=\"140\"><input type=\"text\" name=\"bit%d\" size=\"3\" value=\"%d\"></TD> </tr>";

    int curr_value = ((*data) >> nBit) & 1;

    fprintf_P( stream, fmt1 );
    putBitName( stream, bitNames, nBit );
    fprintf_P( stream, fmt2, nBit, curr_value );

}


static void bit_form_elements( FILE * stream, unsigned int *data, const unsigned char *bitNames )
{
    char nBit;

    for( nBit = 0; nBit < 9; nBit++ )
        bit_form_element( stream, data, bitNames, nBit );
}

static void decode_bit( const char *pname, const char *pval, unsigned int *data )
{
    if( strncmp( "bit", pname, 3 ) ) return;

    int b = atoi( pname+3 );

    int bv = 1 << b;

    if( *pval == '1' || (pval[0] == 'o' && pval[1] == 'n') )
        *data |= bv;
    else
        *data &= ~bv;

}





