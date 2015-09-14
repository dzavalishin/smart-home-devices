#ifndef RUNTIME_CFG_H_
#define RUNTIME_CFG_H_

#include <inttypes.h>

//#include "defs.h"

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
};



extern struct eeprom_cfg        ee_cfg;

void    init_runtime_cfg(void);

int     runtime_cfg_eeprom_read(void);
int     runtime_cfg_eeprom_write(void);


#endif /*RUNTIME_CFG_H_*/
