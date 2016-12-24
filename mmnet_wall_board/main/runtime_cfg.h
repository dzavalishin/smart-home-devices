#ifndef RUNTIME_CFG_H_
#define RUNTIME_CFG_H_

#include <inttypes.h>

#include "defs.h"
#include "onewire.h"



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
#define MAX_OW_MAP 8

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

    // ---------------  Net

    unsigned char	mac_addr[6];    // Default MAC address

    unsigned long	ip_addr;        // Default IP address
    unsigned long	ip_mask;        // Default IP net mask
    unsigned long	ip_route;       // Default IP route

    unsigned long	ip_nntp;       	// IP address of SNTP (time) server
    unsigned long	ip_syslog;     	// IP address of Syslog server
    unsigned long       ip_mqtt;        // IP address of MQTT broker, but see host name below


    // ---------------  Etc

    unsigned int        io_enable;      // Enable/disable io units

    uint32_t            dbg_baud;       // Debug com port baud rate


    // ---------------  1Wire

    struct ow_map_entry ow_map[MAX_OW_MAP];

    // ---------------  MQTT

    // MQTT topics
    char                topics[EEPROM_CFG_N_TOPICS][EEPROM_CFG_MAX_TOPIC_LEN];
    // Corresponding human readable names
    //char                names[EEPROM_CFG_N_TOPICS][EEPROM_CFG_MAX_TOPIC_LEN];

    // MQTT topic prefix
    char                topic_prefix[EEPROM_CFG_MAX_TOPIC_LEN];

    // MQTT topic wildcard
    //char                topic_wildcard[EEPROM_CFG_MAX_TOPIC_LEN]; // subscription wildcard to get topic names in runtime for menu

    char                mqtt_host[EEPROM_CFG_MAX_TOPIC_LEN];
    uint16_t            mqtt_port;

    char                openhab_url[EEPROM_CFG_MAX_TOPIC_LEN]; // to get topic names? - unused yet

    // ---------------  DI/DO

    uint8_t             di_channel[SERVANT_N_DI];  // Numer of channel for Di, if 0-4, then parallelled with front panel button
    uint8_t             di_mode[SERVANT_N_DI];  	// Di modem lower bit 1=button, 0 = switch
    uint8_t             do_channel[SERVANT_N_DO];  // Numer of channel for Do (relay), if 0-4, then parallelled with front panel LED

};

// di_mode bits

#define DI_IS_BUTTON    (1<<0)

// io_enable bits

#define IO_XOR           (1<<0)         // Di/Do (relays) XOR style logic (internal switch -> relay connection)
#define IO_PWM           (1<<1)         // Have LCD backlight PWM
#define IO_DI            (1<<2)         // Have Di (switches)
#define IO_DO            (1<<3)         // Have Do (relays)

#define IO_LCD4          (1<<4)         // LCD in 4 bit mode (low 4 bits of PF are high 4 bits of LCD)
//#define IO_TUN1          (1<<5)         // Serial/TCP tunnel channel 1
#define IO_1W1           (1<<6)         // 1Wire on default 1-line bus
#define IO_1W8           (1<<7)         // 1Wire in multichannel mode

#define IO_LOG           (1<<8)         // Serial logging on UART 1

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
