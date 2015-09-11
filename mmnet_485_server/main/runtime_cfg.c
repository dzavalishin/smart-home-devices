/**
 *
 * DZ-MMNET-MODBUS: Modbus/TCP I/O module based on MMNet101.
 *
 * Runtime-configurable data. EEPROM config save/restore.
 *
**/

#include "runtime_cfg.h"
#include "defs.h"

#include <arpa/inet.h>

//uint8_t		onewire_2482_available = 0;
uint8_t		sntp_available = 0;

struct eeprom_cfg        ee_cfg =
{
    .timezone = DEFAULT_TZ,

    .mac_addr = { DEFAULT_MAC },


  /*
    .ddr_b
    ddr_d;
  ddr_e;      
  ddr_f;      
  ddr_g;      
              

  start_b;    
  start_d;    
  start_e;    
  start_f;    
  start_g;    
  */

};



// MUST do very early!
void
init_runtime_cfg()
{

    ee_cfg.ip_addr	= inet_addr( DEFAULT_IP );
    ee_cfg.ip_mask	= inet_addr( DEFAULT_MASK );
    // ee_cfg.ip_route;

    ee_cfg.ip_nntp	= 0; // Will use default route instead
    ee_cfg.ip_syslog = inet_addr( DEFAULT_SYSLOGD );

}


void    runtime_cfg_eeprom_read(void)
{
}

int    runtime_cfg_eeprom_write(void)
{
    return 0;
}



