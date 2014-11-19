#include "defs.h"
#include "eeprom.h"


/**
 *
 * 0-1 - 'DZ' 'DZ'
 * 2-3 - '1W' 'T8'
 * 4-6 - 6 bytes of serial no
 *
**/

// + dev type
#define MB_REG_ID	0
#define MB_COUNT_ID	4
// + unique id
#define MB_REG_ROMID    4
#define MB_COUNT_ROMID  4

/**
 *
 * 8-15 - flags
 *
 * 8: data changed flag, bits: 0 - temp, 1 - DI, 2 - AI, 3 - counters
 * 9: temperature bus error bits, bit 0 - bus 0
 * 10: other error bits, error_flags var
 * 
 * 14-15: temperature readouts changed, bit per sensor, LSB[14] = temp sensor 0
 * 
**/
#define MB_REG_FLAGS	8
#define MB_REG_FLAGS_ERROR (MB_REG_FLAGS+2) // +

// - 16-31 - assorted IO
//   16: all digital inputs
//   17: all digital outputs
#define MB_REG_IO	16

// - stat/event/debug counters
#define MB_REG_COUNTERS	32
#define MB_COUNT_COUNTERS	32

#define MB_REG_COUNTER_IO       (MB_REG_COUNTERS+0) // + 485 io operations (repiles)
#define MB_REG_COUNTER_CRC      (MB_REG_COUNTERS+1) // + 485 CRC errors
#define MB_REG_COUNTER_EXC      (MB_REG_COUNTERS+2) // + modbus exceptions, dcon error replies
#define MB_REG_COUNTER_1WE      (MB_REG_COUNTERS+3) // + 1wire bus io errors

#define MB_REG_COUNTERS_1WBUS   (MB_REG_COUNTERS+16) // + 1wire bus io errors per bus
#define MB_COUNT_COUNTERS_1WBUS 8

// + 64-79 - 16 x AI (8 provided: 4 real AI, 2 internal, but visible, 2 variable resistors)
#define MB_REG_AI	64
#define MB_COUNT_AI     16

// - 80-95 - 16 x AO (none provided)
#define MB_REG_AO	80

// -
//96-127 - up to 8 counters, in groups
//96 - counter 0 (interrupt 0 input) value
//97 - counter 0 frequency, UNIMPL
//98 - counter 0 activity last N seconds (see config)
//99 - counter 0 flags, bit 0 = overflow, UNIMPL

#define MB_REG_CNT	96

#define MB_REG_SETUP	256

#define MB_REG_SETUP_BUS_ADDR		(MB_REG_SETUP+0) // +
#define MB_REG_SETUP_BUS_SPEED_LO	(MB_REG_SETUP+1) // +
#define MB_REG_SETUP_BUS_SPEED_HI	(MB_REG_SETUP+2) // - unused, impossible to generate on 16MHZ xtal, will get 125000 instead
#define MB_REG_SETUP_BUS_SPEED_ID       (MB_REG_SETUP+3) // - unused yet
#define MB_REG_SETUP_BUS_MODE		(MB_REG_SETUP+4) // -
#define MB_REG_SETUP_PROTOCOLS		(MB_REG_SETUP+5) // -

#define MB_REG_SETUP_CNT_TIME		(MB_REG_SETUP+30)// - counter activity sense interval, 0.1 sec steps (value*10 = seconds)


#define MB_REG_HWCONF	512

#define MB_REG_HWCONF_DI (MB_REG_HWCONF+0) // + DI count (2 counters as DI? +8 AI as DI?)
#define MB_REG_HWCONF_DO (MB_REG_HWCONF+1) // + Do count
#define MB_REG_HWCONF_AI (MB_REG_HWCONF+2) // +
#define MB_REG_HWCONF_AO (MB_REG_HWCONF+3) // +
#define MB_REG_HWCONF_CN (MB_REG_HWCONF+4) // + counters
#define MB_REG_HWCONF_TS (MB_REG_HWCONF+5) // + temperature sensors (max)
#define MB_REG_HWCONF_TA (MB_REG_HWCONF+6) // + temperature sensors (actual)


// + mapped (by id) temperature
#define MB_REG_TEMP     	1024
#define MB_COUNT_TEMP		N_TEMPERATURE_IN


// + direct, unmapped (by sequential sensor number) temperature - debug only
#define MB_REG_TEMP_DIRECT	2048
#define MB_COUNT_TEMP_DIRECT	N_TEMPERATURE_IN

/** -
 *
 *  32 x record:
 *   6 bytes 1wire ROM id
 *   2 bytes persistent id
 *   2 bytes current internal id (search order) or -1 if non-active, NO WR
 *   2 bytes bus number or -1, NO WR
 *   2 bytes curr temperature, NO WR
 *   2 bytes pad
 *
**/
#define MB_REG_ROM_RECSIZE      16
#define MB_REG_ROM_NREC         N_TEMP_SENS_EEPROM_RECORDS

// + mapped (by id) records
#define MB_REG_ROM              4096
#define MB_COUNT_ROM            (MB_REG_ROM_RECSIZE * (N_TEMP_SENS_EEPROM_RECORDS) / 2)

// + direct, unmapped (by sequential sensor number) records
#define MB_REG_MAP              5120
#define MB_COUNT_MAP            (MB_REG_ROM_RECSIZE * N_TEMPERATURE_IN / 2)


// ------------------------------------------------------------------
// Some of the global state
// ------------------------------------------------------------------

#define REPORT_ERROR(_flag) (error_flags |= (_flag))

#define ERR_FLAG_1WIRE_SCAN_FAULT  (1 << 0)
#define ERR_FLAG_1WIRE_START_FAULT (1 << 1)
#define ERR_FLAG_1WIRE_READ_FAULT  (1 << 2)

uint8_t         error_flags; // bit per error type


/*

176: clear temp sensors ROM, search again
177: just search sensors again using old mappings
178: unmap sensor with logical num = LSB
179: map MSB internal id sensor to LSB logical id


*/


