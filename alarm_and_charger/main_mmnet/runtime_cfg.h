#ifndef RUNTIME_CFG_H_
#define RUNTIME_CFG_H_

#include <inttypes.h>

#include "defs.h"
#include "onewire.h"

// Is not zero if we can use 2482 to access 1wire devices.
//extern uint8_t		onewire_2482_available;

extern uint8_t		onewire_available;
extern uint8_t		sntp_available;

// ------------------------------------------------------------------
// Error flags
// ------------------------------------------------------------------

#define REPORT_ERROR(_flag) (error_flags |= (_flag))

#define ERR_FLAG_1WIRE_SCAN_FAULT  (1 << 0)
#define ERR_FLAG_1WIRE_START_FAULT (1 << 1)
#define ERR_FLAG_1WIRE_READ_FAULT  (1 << 2)

extern uint8_t         error_flags; // bit per error type


//#define MAX_OW_MAP 32
#define MAX_OW_MAP 16

struct ow_map_entry
{
    uint8_t        id[OW_ROMCODE_SIZE];
    uint8_t        index;
    uint8_t        spare;
};


// For defaults see runtime_cfg.c

struct eeprom_cfg
{
    signed char		timezone;       // For SNTP/time code

    unsigned char	mac_addr[6];    // Default MAC address

    // Startup port direction bits (1 = out)
    unsigned char	ddr_b;
    unsigned char	ddr_d;
    unsigned char	ddr_e;
    unsigned char	ddr_f;
    unsigned char	ddr_g;

    // Startup port data
    unsigned char	start_b;
    unsigned char	start_d;
    unsigned char	start_e;
    unsigned char	start_f;
    unsigned char	start_g;

    unsigned long	ip_addr;        // Default IP address
    unsigned long	ip_mask;        // Default IP net mask
    unsigned long	ip_route;       // Default IP route

    unsigned long	ip_nntp;       	// IP address of SNTP (time) server
    unsigned long	ip_syslog;     	// IP address of Syslog server

    unsigned int        io_enable;      // Enable/disable io units

    uint32_t 		tun_baud[2];    // TCP/485 tunnel baud rate
    uint32_t            dbg_baud;       // Debug com port baud rate

    struct ow_map_entry ow_map[MAX_OW_MAP];
};



#define IO_LOG           (1<<0)         // Serial logging on UART 1
#define IO_TUN0          (1<<1)         // Serial/TCP tunnel channel 0
#define IO_TUN1          (1<<2)         // Serial/TCP tunnel channel 1


//#define IO_BMP           (1<<3)         // BMP180 pressure meter

#define RT_IO_ENABLED(__WHAT_) ((ee_cfg.io_enable & (__WHAT_)) != 0 )
#define RT_IO_RESET(__WHAT_) ((ee_cfg.io_enable &= ~(__WHAT_)))
#define RT_IO_SET(__WHAT_) ((ee_cfg.io_enable |= (__WHAT_)))



extern struct eeprom_cfg        ee_cfg;
extern unsigned char ow_id_map[MAX_OW_MAP];




void    init_runtime_cfg(void);

int     runtime_cfg_eeprom_read(void);
int     runtime_cfg_eeprom_write(void);

void 	ow_map_add_found(uint8_t id[OW_ROMCODE_SIZE], int index);
void	ow_map_clear( void );


#endif /*RUNTIME_CFG_H_*/
