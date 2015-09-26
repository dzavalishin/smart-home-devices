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

    .ddr_b = 0,
    .ddr_d = 0,
    .ddr_e = 0x0F, // low 4 bits are default outputs
    .ddr_f = 0, 
    .ddr_g = 0,
              

    .start_b = 0,
    .start_d = 0,
    .start_e = 0,
    .start_f = 0,
    .start_g = 0,

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

    //ee_cfg.ddr_e = 0x0F; // low 4 bits are default outputs

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




// -----------------------------------------------------------------------
// One wire map
// -----------------------------------------------------------------------

char ow_id_map[MAX_OW_MAP];


static int find_entry_by_id(uint8_t id[OW_ROMCODE_SIZE])
{
    uint8_t i;
    for( i = 0; i < MAX_OW_MAP; i++ )
    {
        int diff = ow_cmp_rom( id, ee_cfg.ow_map[i].id );
        if( diff == 0 ) return i;

    }

    return -1;
}

void ow_map_add_found(uint8_t id[OW_ROMCODE_SIZE], int index )
{
    uint8_t empty[OW_ROMCODE_SIZE] = { 0, 0, 0, 0, 0, 0, 0, 0 };

    int pos = find_entry_by_id( id );

    if( pos >= 0 )
    {
        // Found
        int permanent_pos = ee_cfg.ow_map[pos].index;

        if( (permanent_pos < 0) || (permanent_pos >= MAX_OW_MAP ) ) goto new;

        ow_id_map[permanent_pos] = index;
        return;
    }

new:
    pos = find_entry_by_id( empty );

    if( pos >= 0 )
    {
        // Found empty space
        ee_cfg.ow_map[pos].index = -1;
        ee_cfg.ow_map[pos].spare = 0;
        ow_copy_rom( ee_cfg.ow_map[pos].id, id );
    }
}

