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
#include <dev/nvmem.h>

#include <crc8.h>
#include <string.h>

//uint8_t		onewire_2482_available = 0;
uint8_t		onewire_available = 0;
uint8_t		sntp_available = 0;
uint8_t		error_flags; // bit per error type

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
    ee_cfg.timezone = DEFAULT_TZ;

    ee_cfg.ip_addr	= inet_addr( DEFAULT_IP );
    ee_cfg.ip_mask	= inet_addr( DEFAULT_MASK );
    // ee_cfg.ip_route;

    ee_cfg.ip_nntp	= 0; // Will use default route instead
    ee_cfg.ip_syslog = inet_addr( DEFAULT_SYSLOGD );

    unsigned char def_mac[] = { DEFAULT_MAC };
    memcpy( ee_cfg.mac_addr, def_mac, sizeof(ee_cfg.mac_addr) );

}

#define EESZ (sizeof(struct eeprom_cfg))

int
runtime_cfg_eeprom_read(void)
{
    unsigned char buf[EESZ+1];

    cli();
    OnChipNvMemLoad( EEPROM_CFG_BASE, buf, EESZ + 1 );
    sei();

    uint8_t crc = crc8 ( buf+1, EESZ );

    if( crc != buf[0] ) return -1;

    ee_cfg = *((struct eeprom_cfg *) (buf+1));
    return 0;
}

int
runtime_cfg_eeprom_write(void)
{
    unsigned char buf[EESZ+1];

    void *mem = &ee_cfg;
    uint8_t crc = crc8 ( mem, sizeof(struct eeprom_cfg) );

    cli();
    OnChipNvMemSave( EEPROM_CFG_BASE+1, mem, EESZ );
    OnChipNvMemSave( EEPROM_CFG_BASE, &crc, 1 );

    // Now check
    OnChipNvMemLoad( EEPROM_CFG_BASE, buf,  EESZ + 1 );
    sei();

    crc = crc8 ( buf+1, EESZ );

    if( crc != buf[0] ) return -1;

    if( memcmp( buf+1, mem, EESZ ) ) return -1;

    return 0;
}



