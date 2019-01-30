/**
 *
 * DZ-MMNET-WALL: Wall control panel based on MMNet01.
 *
 * HTTPD/CGI code for web configuration.
 *
 * URL: cgi-bin/form.cgi
 *
 **/


#include "defs.h"
#include "runtime_cfg.h"
#include "servant.h"

#include <sys/heap.h>


#include "web.h"

#include <time.h>

#include "io_temp.h"
#include "mqtt_udp_glue.h"



#if ENABLE_HTTP


static void printTime( FILE * stream, struct _tm *stm )
{
    fprintf(stream, "%02d/%02d/%04d, %02d:%02d:%02d%s",
            stm->tm_mday, stm->tm_mon + 1, stm->tm_year + 1900,
            stm->tm_hour, stm->tm_min, stm->tm_sec,
            stm->tm_isdst ? " DST" : ""
           );
}

static void subhdr( FILE * stream, char *txt )
{
        fprintf(stream, "<th colspan=\"2\" align=left>%s</th>", txt);
}

static int CgiStatusRow( FILE * stream, int row_no )
{
    time_t secs = time(NULL);

    switch( row_no )
    {

    case -1:
        subhdr( stream, "Time" ); break;

    case 0:
        {
            HTML("<TR><TD>&nbsp;GMT Time </TD><TD>&nbsp;");
            printTime( stream, gmtime(&secs) );
            HTML(" </TD></TR>\r\n");

            //ShowTableRow3( stream, char *c1, char *c2, char *c3 );

        }
        break;

    case 1:
        {
            HTML("<TR><TD>&nbsp;Local Time </TD><TD>&nbsp;");
            printTime( stream, localtime(&secs) );
            HTML(" </TD></TR>\r\n");
        }
        break;

    case 2:
        {
            char minus = _timezone < 0;
            int32_t seconds = labs(_timezone);
            int32_t minutes = seconds / 60UL;
            int32_t hours = minutes / 60UL;
            minutes %= 60UL;
            seconds %= 60UL;
            HTML("<TR><TD>&nbsp;TimeZone </TD><TD>&nbsp;");
            fprintf(stream, "%s%ld:%ld:%ld (%ld)", (minus ? "-" : ""), hours, minutes, seconds, _timezone );
            HTML(" </TD></TR>\r\n");
        }
        break;


    case 3:
        {
            uint32_t seconds = NutGetSeconds();
            uint32_t minutes = seconds / 60UL;
            uint32_t hours = minutes / 60UL;
            uint32_t days = hours / 24UL;
            minutes %= 60UL;
            seconds %= 60UL;
            hours %= 24UL;

            HTML("<TR><TD>&nbsp;UpTime </TD><TD>&nbsp;");
            fprintf(stream, "%lu days  %lu:%lu:%lu", days, hours, minutes, seconds);
            HTML(" </TD></TR>\r\n");
        }
        break;

    case 4: ShowTableRow2b( stream, "Used SNTP", sntp_available );		break;

    case 5: subhdr( stream, "FirmWare" ); break;

    case 6: ShowTableRow2( stream, "Build", makeDate );                		break;
    case 7: ShowTableRow2( stream, "Name", DEVICE_NAME );                	break;
    case 8: ShowTableRow2( stream, "ModBus Id", modbus_device_id );		break;

    case 9: //subhdr( stream, "ModBus" );
        break;

    case 10: //ShowTableRow2i( stream, "IO count", modbus_event_cnt );
        break;
    case 11: //ShowTableRow2i( stream, "CRC count", modbus_crc_cnt );
        break;
    case 12:
        //ShowTableRow2i( stream, "exceptions count", modbus_exceptions_cnt );
        //ShowTableRow2i( stream, "err flags", modbus_error_flags );
        break;

    case 13:
        subhdr( stream, "MQTT/UDP" );
        ShowTableRow2i( stream, "IO count", mqtt_io_count );
        //ShowTableRow2i( stream, "IO count", 0 );
        break;

    case 14: subhdr( stream, "1-Wire" ); break;

    case 15: ShowTableRow2b( stream, "Devices detected", onewire_available );	break;
    case 16: ShowTableRow2i( stream, "Temp sensors count", nTempSensors );	break;
    case 17:
#if SERVANT_1WMAC
        {
            HTML("<TR><TD>&nbsp;2401 Id </TD><TD>&nbsp;");
            fprintf(stream, "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",
                    serialNumber [7], serialNumber [6], serialNumber [5], serialNumber [4],
                    serialNumber [3], serialNumber [2], serialNumber [1], serialNumber [0]
                   );
            HTML(" </TD></TR>\r\n");
        }
#endif
        break;

    case 18: subhdr( stream, "OS" ); break;
    case 19: ShowTableRow2i( stream, "Free mem, K", NutHeapAvailable()/1024 );	break;
    case 20: ShowTableRow2i( stream, "Size of EEPROM cfg", sizeof(struct eeprom_cfg) );	break;
        
    case 21:
        {
            // empty
            break;
        }

//    case 22: subhdr( stream, "DHT11" ); break;
//    case 23: ShowTableRow2i( stream, "Errors count", dht11_errorCnt );	break;
        

    default:
        return 0;
    }

    return 1;
}



int CgiStatus( FILE * stream, REQUEST * req )
{
    return ShowTableCgi( stream, req, "Status", CgiStatusRow );
}



#endif // ENABLE_HTTP

