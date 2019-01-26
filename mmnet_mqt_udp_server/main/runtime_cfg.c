/**
 *
 * DZ-MMNET-WALL: Wall control panel based on MMNet01.
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



char modbus_device_id[] = MODBUS_DEVICE_ID; // TODO RENAME ME


uint8_t		onewire_available = 0;
uint8_t		sntp_available = 0;
uint8_t		error_flags; // bit per error type

struct eeprom_cfg        ee_cfg =
{
    .timezone = DEFAULT_TZ,

    .mac_addr = { DEFAULT_MAC },
/*
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
*/
    .dbg_baud = DEFAULT_BAUD,
//    .tun_baud = {DEFAULT_BAUD, DEFAULT_BAUD},
};



// MUST do very early!
void
init_runtime_cfg()
{
    int i;

    ee_cfg.timezone = DEFAULT_TZ;

    ee_cfg.ip_addr	= inet_addr( DEFAULT_IP );
    ee_cfg.ip_mask	= inet_addr( DEFAULT_MASK );
    // ee_cfg.ip_route;

    ee_cfg.ip_nntp	= 0; // Will use default route instead
    ee_cfg.ip_syslog	= inet_addr( DEFAULT_SYSLOGD );
    ee_cfg.ip_mqtt	= 0; //inet_addr( DEFAULT_MQTT );
#if 0
    strlcpy( ee_cfg.topics[0], "Light5A", sizeof( ee_cfg.topics[0] ) );
    strlcpy( ee_cfg.topics[1], "Light5B", sizeof( ee_cfg.topics[1] ) );
    strlcpy( ee_cfg.topics[2], "Light1A", sizeof( ee_cfg.topics[2] ) );
    strlcpy( ee_cfg.topics[3], "Light1B", sizeof( ee_cfg.topics[3] ) );

    strlcpy( ee_cfg.names[0], "Dinner L", sizeof( ee_cfg.names[0] ) );
    strlcpy( ee_cfg.names[1], "Dinner R", sizeof( ee_cfg.names[1] ) );
    strlcpy( ee_cfg.names[2], "Guest L", sizeof( ee_cfg.names[2] ) );
    strlcpy( ee_cfg.names[3], "Guest R", sizeof( ee_cfg.names[3] ) );
#endif
    //ee_cfg.ddr_e = 0x0F; // low 4 bits are default outputs

    unsigned char def_mac[] = { DEFAULT_MAC };
    memcpy( ee_cfg.mac_addr, def_mac, sizeof(ee_cfg.mac_addr) );

    //ee_cfg.io_enable = IO_LOG|IO_LCD4; // disable all by default, enable serial debug and 4-bit LCD IO
    ee_cfg.io_enable = IO_LOG; // disable all by default, enable serial debug 

    strlcpy( ee_cfg.topic_prefix, "/openhab", sizeof(ee_cfg.topic_prefix) );

    strlcpy( ee_cfg.mqtt_host, "smart", sizeof(ee_cfg.mqtt_host) );
    ee_cfg.mqtt_port = 1883;

    for( i = 0; i < SERVANT_N_DI; i++ )
    {
        ee_cfg.di_channel[i] = 4+i;
        ee_cfg.di_mode[i] = 0;
    }

    for( i = 0; i < SERVANT_N_DO; i++ )
        ee_cfg.do_channel[i] = 4+i;

    //for( i = 0; (i < SERVANT_N_DI) && (i < SERVANT_N_DO); i++ )
    //    do_channel[i] = di_channel[i];

}

/*
__ATTR_PURE__ static __inline__ uint8_t eeprom_read_byte (uint8_t addr)
{
    EEAR = addr;

 ////////////////////////////////////////////////////////////
  // here fixed ERRATA Bug "Reading EEPROM by using ST or STS
  // to set EERE bit triggers unexpected interrupt request"
  // EECR |= (1 << EERE);
  __asm__ __volatile__ (
        "sbi    %[__eecr], %[__eere]    \n\t"
        :
        : [__eecr] "i" (_SFR_IO_ADDR(EECR)),
         [__eere] "i" (EERE)
    );
  return EEDR;
}
*/

static void eeload( uint16_t base, void *buf, uint16_t size );
static void eesave( uint16_t base, void *buf, uint16_t size );


#define EESZ (sizeof(struct eeprom_cfg))

int
runtime_cfg_eeprom_read(void)
{
    unsigned char buf[EESZ+1];

    cli();
    //OnChipNvMemLoad( EEPROM_CFG_BASE, buf, EESZ + 1 );
    eeload( EEPROM_CFG_BASE, buf, EESZ + 1 );
    sei();

    uint8_t crc = crc8 ( buf+1, EESZ );

    if( crc != buf[0] ) return -1;

    cli();
    ee_cfg = *((struct eeprom_cfg *) (buf+1));
    sei();
    return 0;
}

int
runtime_cfg_eeprom_write(void)
{
    unsigned char buf[EESZ+1];

    cli();
    void *mem = &ee_cfg;
    uint8_t crc = crc8 ( mem, sizeof(struct eeprom_cfg) );

    //OnChipNvMemSave( EEPROM_CFG_BASE+1, mem, EESZ );
    //OnChipNvMemSave( EEPROM_CFG_BASE, &crc, 1 );

    eesave( EEPROM_CFG_BASE+1, mem, EESZ );
    eesave( EEPROM_CFG_BASE, &crc, 1 );

    // Now check
    //OnChipNvMemLoad( EEPROM_CFG_BASE, buf,  EESZ + 1 );
    eeload( EEPROM_CFG_BASE, buf,  EESZ + 1 );
    sei();

    crc = crc8 ( buf+1, EESZ );

    if( crc != buf[0] ) return -1;

    if( memcmp( buf+1, mem, EESZ ) ) return -1;

    return 0;
}

// -----------------------------------------------------------------------
// eeprom code dies on big IO, do in parts
// -----------------------------------------------------------------------


static void eeload( uint16_t base, void *buf, uint16_t size )
{
    while( size )
    {
        uint16_t part = size;

        if( part > 256 ) part = 256;

        OnChipNvMemLoad( base, buf, part );

        base += part;
        size -= part;

    }
}

static void eesave( uint16_t base, void *buf, uint16_t size )
{
    while( size )
    {
        uint16_t part = size;

        if( part > 256 ) part = 256;

        OnChipNvMemSave( base, buf, part );

        base += part;
        size -= part;
    }
}



// -----------------------------------------------------------------------
// One wire map
// -----------------------------------------------------------------------

unsigned char ow_id_map[MAX_OW_MAP];


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

// io code calls us when sensor is found, we update ow_id_map[]
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


void
ow_map_clear( void )
{
    memset( ee_cfg.ow_map, 0, sizeof( ee_cfg.ow_map ) );
}



